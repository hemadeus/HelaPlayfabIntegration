#include "Character/Manager/PlayFabCharacterManager.h"

#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "PlayFabClientAPI.h"
#include "PlayFabClientDataModels.h"
#include "PlayFabClientModels.h"

#include "PlayFabJsonObject.h"

#include "Authentication/AuthenticationContext.h"
#include "Authentication/LoginActor.h"
#include "Character/Models/CharacterItem.h"
#include "Components/Image.h"
#include "Core/PlayFabClientAPI.h"
#include "Core/PlayFabEconomyAPI.h"
#include "Core/PlayFabManager.h"
#include "Core/PlayFabCloudScriptAPI.h"

void UPlayFabCharacterManager::Initialize(const FString& PlayerId)
{
	CachedPlayerId = PlayerId;
}

void UPlayFabCharacterManager::InitializeManager(const TSharedPtr<PlayFab::UPlayFabClientAPI>& InClientAPI, const TSharedPtr<PlayFab::UPlayFabEconomyAPI>& InEconomyAPI,
                                                 const TSharedPtr<PlayFab::UPlayFabCloudScriptAPI>& InCloudScriptAPI, UPlayFabManager* InCoreManager)
{
	ClientAPI = InClientAPI;
	EconomyAPI = InEconomyAPI;
	CoreManagerReference = InCoreManager;
	CloudScriptAPI = InCloudScriptAPI;
}

 void UPlayFabCharacterManager::SetImageFromURL(UImage* ImageWidget, const FString& ImageURL)
{
	if (!ImageWidget) return;

	FHttpModule* Http = &FHttpModule::Get();
	if (!Http) return;

	// Download and set the image
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindLambda([ImageWidget](FHttpRequestPtr Request, const FHttpResponsePtr& Response, bool bWasSuccessful)
	{
		if (bWasSuccessful && Response.IsValid())
		{
			TArray<uint8> ImageData = Response->GetContent();

			IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
			TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

			if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
			{
				TArray64<uint8> UncompressedBGRA;
				if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
				{
					UTexture2D* AvatarTexture = UTexture2D::CreateTransient(
						ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

					void* TextureData = AvatarTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
					FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
					AvatarTexture->GetPlatformData()->Mips[0].BulkData.Unlock();

					AvatarTexture->UpdateResource();

					if (ImageWidget)
					{
						ImageWidget->SetBrushFromTexture(AvatarTexture);
					}
				}
			}
		}
	});

	HttpRequest->SetURL(ImageURL);
	HttpRequest->SetVerb("GET");
	HttpRequest->ProcessRequest();
}

FCharacterItem UPlayFabCharacterManager::FromCatalogItem(const PlayFab::EconomyModels::FCatalogItem& CatalogItem, const PlayFab::EconomyModels::FInventoryItem& InventoryItem, const FString& Language)
{
	FCharacterItem CharacterItem;
	CharacterItem.Id = CatalogItem.Id;

	if (!CatalogItem.Description.IsEmpty() && CatalogItem.Description.Contains(Language))
	{
		CharacterItem.Description = CatalogItem.Description[Language];
	}
	else
	{
		CharacterItem.Description = "";
	}
	CharacterItem.Tags = CatalogItem.Tags;

	//Getting user display values.
	// For now only the name is something created by the user for the character
	if (!InventoryItem.DisplayProperties.isNull())
	{
		FString JsonString = InventoryItem.DisplayProperties.toJSONString();

		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			FString Name;
			if (JsonObject->TryGetStringField(TEXT("name"), Name))
			{
				CharacterItem.DisplayName = Name;
			}
		}
	}

	return CharacterItem;
}

void UPlayFabCharacterManager::AddCatalogItemInfo(const PlayFab::EconomyModels::FCatalogItem& CatalogItem, FCharacterItem& Character, const FString& Language)
{
	if (!CatalogItem.Description.IsEmpty() && CatalogItem.Description.Contains(Language))
	{
		Character.Description = CatalogItem.Description[Language];
	}
	else
	{
		Character.Description = "";
	}
	Character.Tags = CatalogItem.Tags;

	//return CharacterItem;
}

void UPlayFabCharacterManager::CreateUserCharacter_Implementation(const FString& CatalogItemId, const FString& CharacterName, const FOnCharacterCreated& OnCharacterCreated)
{
	// 1. Prepare the request
	PlayFab::CloudScriptModels::FExecuteFunctionRequest Request;

	// Set the Entity that will call the function (usually the player)
	auto AuthContext = CoreManagerReference->GetSharedAuthenticationContext();
	Request.Entity = MakeShared<PlayFab::CloudScriptModels::FEntityKey>();
	Request.Entity->Id = AuthContext->EntityId;
	Request.Entity->Type = AuthContext->EntityType;

	// Set the function you want to execute
	Request.FunctionName = TEXT("AddCharacter");

	// Set the parameters for the function
	TSharedPtr<FJsonObject> Params = MakeShared<FJsonObject>();
	Params->SetStringField(TEXT("itemId"), CatalogItemId);
	Params->SetStringField(TEXT("name"), CharacterName);
	Params->SetStringField(TEXT("catalogVersion"), TEXT("Main"));

	Request.FunctionParameter = PlayFab::FJsonKeeper(Params);

	// Set the Authentication Context (so PlayFab knows which player is calling)
	Request.AuthenticationContext = AuthContext->PlayFabAuthenticationContext;
	// 3. Execute the function
	CloudScriptAPI->ExecuteFunction(
		Request,
		PlayFab::UPlayFabCloudScriptAPI::FExecuteFunctionDelegate::CreateLambda(
			[this, CatalogItemId, OnCharacterCreated, CharacterName](const PlayFab::CloudScriptModels::FExecuteFunctionResult& Result)
			{
				const auto* CatalogItem = Algo::FindByPredicate(CoreManagerReference->CachedCatalogItems, [&CatalogItemId](const auto& Cat)
				{
					return Cat.Id == CatalogItemId;
				});

				if (!CatalogItem)
				{
					UE_LOG(LogTemp, Error, TEXT("Catalog item not found: %s"), *CatalogItemId);
					return;
				}
				FCharacterItem CreatedCharacter;
				CreatedCharacter.Id = CatalogItemId;
				CreatedCharacter.DisplayName = CharacterName;
				// Build your FCharacterItem
				AddCatalogItemInfo(*CatalogItem, CreatedCharacter, TEXT("NEUTRAL"));
				OnCharacterCreated.Execute(CreatedCharacter);
			}),
		PlayFab::FPlayFabErrorDelegate::CreateLambda(
			[](const PlayFab::FPlayFabCppError& Error)
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to fetch inventory: %s"),
				       *Error.ErrorMessage);
			})
	);
}

void UPlayFabCharacterManager::GetUserCharacters_Implementation(const FOnCharactersLoaded& OnCharactersReady)
{
	FString ContentType = "Character";
	TArray<PlayFab::EconomyModels::FCatalogItem> CharacterCatalogItems;


	for (const auto& Item : CoreManagerReference->CachedCatalogItems)
	{
		if (Item.ContentType == ContentType)
		{
			CharacterCatalogItems.Add(Item);
		}
	}

	PlayFab::EconomyModels::FGetInventoryItemsRequest Request;
	auto AuthContext = CoreManagerReference->GetSharedAuthenticationContext();
	Request.Entity = MakeShared<PlayFab::EconomyModels::FEntityKey>();
	Request.Entity->Id = AuthContext->EntityId;
	Request.Entity->Type = AuthContext->EntityType;
	Request.AuthenticationContext = AuthContext->PlayFabAuthenticationContext;
	Request.CollectionId = TEXT("characters");
	EconomyAPI->GetInventoryItems(
		Request,
		PlayFab::UPlayFabEconomyAPI::FGetInventoryItemsDelegate::CreateLambda(
			[this, CharacterCatalogItems, OnCharactersReady](const PlayFab::EconomyModels::FGetInventoryItemsResponse& Response)
			{
				TArray<FCharacterItem> CharacterItems;
				for (auto UserItem : Response.Items)
				{
					const PlayFab::EconomyModels::FCatalogItem* CharacterCatalogItem = Algo::FindByPredicate(CharacterCatalogItems, [UserItem](const PlayFab::EconomyModels::FCatalogItem& CatalogItem)
					{
						return CatalogItem.Id == UserItem.Id;
					});

					if (CharacterCatalogItem)
					{
						auto Item = FromCatalogItem(*CharacterCatalogItem, UserItem, "NEUTRAL");
						CharacterItems.Add(Item);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Character item with ID %s not found in catalog."), *UserItem.Id);
					}
				}

				OnCharactersReady.Execute(CharacterItems);
			}),
		PlayFab::FPlayFabErrorDelegate::CreateLambda([](const PlayFab::FPlayFabCppError& Error)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to fetch inventory: %s"), *Error.ErrorMessage);
		})
	);
}


void UPlayFabCharacterManager::DeleteUserCharacter_Implementation(const FString& CharacterId)
{
	UE_LOG(LogTemp, Warning, TEXT("Deleting character with ID: %s"), *CharacterId);

	// Add PlayFab logic here (likely a cloud script or title function).
}

void UPlayFabCharacterManager::GetCatalogAvailableCharacters_Implementation(const FOnAvailableCharactersLoaded& OnCharactersReady)
{
	FString ContentType = "Character";
	TArray<FCharacterCatalogItem> CharacterCatalogItems;
	for (const auto& Item : CoreManagerReference->CachedCatalogItems)
	{
		if (Item.ContentType == ContentType)
		{
			FCharacterCatalogItem CharacterCatalogItem = FCharacterCatalogItem(); // Construct properly
			CharacterCatalogItem.Id = Item.Id;
			CharacterCatalogItem.DisplayName = Item.Title["NEUTRAL"];
			CharacterCatalogItem.Description = Item.Description["NEUTRAL"];
			CharacterCatalogItem.Tags = Item.Tags;
			if (Item.Images.Num() > 0)
			{
				CharacterCatalogItem.ImageUrl = Item.Images[0].Url;
			}
			// Fill CharacterCatalogItem with data from Item if needed
			CharacterCatalogItems.Add(CharacterCatalogItem);
		}
	}

	OnCharactersReady.Execute(CharacterCatalogItems);
}
