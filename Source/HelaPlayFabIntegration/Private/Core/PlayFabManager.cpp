#include "Core/PlayFabManager.h"

#include "PlayFab.h"
#include "PlayFabRuntimeSettings.h"
#include "Authentication/AuthenticationContext.h"

#include "Authentication/PlayFabLoginManager.h"
#include "Character/Manager/PlayFabCharacterManager.h"
#include "Core/PlayFabEconomyAPI.h"

//
// static const FString kIniSection = TEXT("/Script/Engine.GameUserSettings");
// static const FString kIniKey = TEXT("PlayFabDeviceId");
static const FString kTitleId = TEXT("<<YOUR_TITLE_ID>>"); // Replace with your PlayFab title ID

UPlayFabManager::UPlayFabManager()
{
	// Set once globally
	GetMutableDefault<UPlayFabRuntimeSettings>()->TitleId = kTitleId;
	TitleId = kTitleId;
	ClientAPI = IPlayFabModuleInterface::Get().GetClientAPI();

	EconomyAPI = IPlayFabModuleInterface::Get().GetEconomyAPI();

	AuthenticationAPI = IPlayFabModuleInterface::Get().GetAuthenticationAPI();
	CloudScriptAPI = IPlayFabModuleInterface::Get().GetCloudScriptAPI();
}

void UPlayFabManager::Initialize()
{
	InitializeSubManagers();
}

void UPlayFabManager::GetCatalogItems()
{
	PlayFab::EconomyModels::FSearchItemsRequest Request;
	//FString ContentType = "Character";
	FString tag = "cached";
	// Example filter: only get items where contentType is "weapon"
	Request.Filter = FString::Printf(TEXT("tags/any(t: t eq '%s')"), *tag);
	Request.Entity = MakeShared<PlayFab::EconomyModels::FEntityKey>();
	Request.Entity->Id = SharedAuthenticationContext->EntityId;
	Request.Entity->Type = SharedAuthenticationContext->EntityType;
	Request.AuthenticationContext = SharedAuthenticationContext->PlayFabAuthenticationContext;

	EconomyAPI->SearchItems(
		Request,
		PlayFab::UPlayFabEconomyAPI::FSearchItemsDelegate::CreateLambda([this](const PlayFab::EconomyModels::FSearchItemsResponse& Response)
		{
			for (const auto& Item : Response.Items)
			{
				CachedCatalogItems.Add(Item);
				UE_LOG(LogTemp, Log, TEXT("Cached Catalog Item: %s (%s)"), *Item.Title["NEUTRAL"], *Item.Id);
			}

			bIsCatalogLoaded = true;
			UE_LOG(LogTemp, Log, TEXT("Catalog loaded and cached successfully."));
		}),
		//PlayFab::UPlayFabEconomyAPI::FSearchItemsDelegate::CreateUObject(this, &UPlayFabCharacterManager::OnSearchItemsSuccess),
		PlayFab::FPlayFabErrorDelegate::CreateLambda([this](const PlayFab::FPlayFabCppError& PlayFabCppError)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to Cached Catalog: %s"), *PlayFabCppError.ErrorMessage);
		})
	);
}

void UPlayFabManager::InitializeSubManagers()
{
	LoginManager = NewObject<UPlayFabLoginManager>(this);
	LoginManager->InitializeManager(ClientAPI, this, AuthenticationAPI);

	CharacterManager = NewObject<UPlayFabCharacterManager>(this);
	CharacterManager->InitializeManager(ClientAPI, EconomyAPI, CloudScriptAPI, this);

	// UserInfoManager = NewObject<UPlayFabUserInfoManager>(this);
	// UserInfoManager->SetClientAPI(ClientAPI);
}
