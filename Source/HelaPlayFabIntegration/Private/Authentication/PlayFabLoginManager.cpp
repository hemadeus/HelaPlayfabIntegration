#include "Authentication/PlayFabLoginManager.h"

#include "JsonObjectConverter.h"
#include "Misc/ConfigCacheIni.h"
#include "PlayFabClientModels.h"
#include "Authentication/AuthenticationContext.h"
#include "Authentication/LoginActor.h"
#include "Core/PlayFabClientAPI.h"
#include "Core/PlayFabAuthenticationAPI.h"
#include "Core/PlayFabManager.h"
using namespace PlayFab;
using namespace PlayFab::ClientModels;

UPlayFabLoginManager::UPlayFabLoginManager()
{
}


void UPlayFabLoginManager::RegisterNewUser_Implementation(const FString& Email, const FString& Password, const FString& Username)
{
	FRegisterPlayFabUserRequest Request;
	Request.Email = Email;
	Request.Password = Password;
	Request.DisplayName = Username;
	Request.TitleId = CoreManagerReference->TitleId;
	Request.RequireBothUsernameAndEmail = false;

	ClientAPI->RegisterPlayFabUser(
		Request,
		UPlayFabClientAPI::FRegisterPlayFabUserDelegate::CreateUObject(this, &UPlayFabLoginManager::OnRegisterSuccess),
		FPlayFabErrorDelegate::CreateUObject(this, &UPlayFabLoginManager::OnRegisterFailure));
}

void UPlayFabLoginManager::LoginExistingUser_Implementation(const FString& Email, const FString& Password)
{
	FLoginWithEmailAddressRequest Request;
	Request.Email = Email;
	Request.Password = Password;

	ClientAPI->LoginWithEmailAddress(
		Request,
		UPlayFabClientAPI::FLoginWithEmailAddressDelegate::CreateUObject(this, &UPlayFabLoginManager::OnLoginSuccess),
		FPlayFabErrorDelegate::CreateUObject(this, &UPlayFabLoginManager::OnRegisterFailure));
}

bool UPlayFabLoginManager::IsUserLoggedIn_Implementation()
{
	return AuthenticationContext != nullptr && AuthenticationContext->EntityToken != "";
}

void UPlayFabLoginManager::LoginOrCreateWithDeviceID_Implementation()
{
	// FString CustomId;
	//
	// // 1) Load or generate a stable ID stored in DefaultGame.ini
	// if (!GConfig->GetString(*kIniSection, *kIniKey, CustomId, GGameIni) || CustomId.IsEmpty())
	// {
	// 	CustomId = FGuid::NewGuid().ToString(EGuidFormats::Digits);
	// 	GConfig->SetString(*kIniSection, *kIniKey, *CustomId, GGameIni);
	// 	GConfig->Flush(false, GGameIni);
	// }
	//
	// FLoginWithCustomIDRequest Request;
	// Request.CustomId = CustomId;
	// Request.CreateAccount = true;
	// Request.TitleId = kTitleId;
	//
	// clientAPI->LoginWithCustomID(Request,
	//                              PlayFab::UPlayFabClientAPI::FLoginWithCustomIDDelegate::CreateUObject(this, &UPlayFabLoginManager::OnLoginSuccess),
	//                              FPlayFabErrorDelegate::CreateUObject(this, &UPlayFabLoginManager::OnLoginFailure));
}

void UPlayFabLoginManager::AutoLoginIfPossible_Implementation()
{
	if (LoadAuthenticationContextFromDisk())
	{
		ApiAuthentication();
	}
	else
	{
		AuthenticationContext = NewObject<UHelaAuthenticationContext>();
	}
}

UHelaAuthenticationContext* UPlayFabLoginManager::GetAuthenticationContext_Implementation()
{
	return AuthenticationContext;
}

/* ---------- private helpers ---------- */

void UPlayFabLoginManager::InitializeManager(TSharedPtr<PlayFab::UPlayFabClientAPI> InClientAPI, UPlayFabManager* InCoreManager, TSharedPtr<PlayFab::UPlayFabAuthenticationAPI> InAuthenticationAPI)
{
	ClientAPI = InClientAPI;
	CoreManagerReference = InCoreManager;
	AuthenticationAPI = InAuthenticationAPI;
}

void UPlayFabLoginManager::ApiAuthentication()
{
	PlayFab::AuthenticationModels::FGetEntityTokenRequest TokenRequest;

	auto RawContext = NewObject<UPlayFabAuthenticationContext>();
	RawContext->SetEntityToken(AuthenticationContext->EntityToken);
	RawContext->SetPlayFabId(AuthenticationContext->Id);
	RawContext->SetClientSessionTicket(AuthenticationContext->SessionTicket);
	TSharedPtr<UPlayFabAuthenticationContext> SafeContext = TSharedPtr<UPlayFabAuthenticationContext>(
		RawContext,
		[](UPlayFabAuthenticationContext* Ptr)
		{
			// No delete here — GC will clean up
		});

	TokenRequest.AuthenticationContext = SafeContext;
	AuthenticationAPI->GetEntityToken(
		TokenRequest,
		PlayFab::UPlayFabAuthenticationAPI::FGetEntityTokenDelegate::CreateLambda(
			[this](const PlayFab::AuthenticationModels::FGetEntityTokenResponse& Response)
			{
				auto RawContext = NewObject<UPlayFabAuthenticationContext>();


				RawContext->SetEntityToken(AuthenticationContext->EntityToken);
				RawContext->SetPlayFabId(AuthenticationContext->Id);
				RawContext->SetClientSessionTicket(AuthenticationContext->SessionTicket);
				// TSharedPtr<UPlayFabAuthenticationContext> PlayFabContext = MakeShareable(rawContext);
				// Request.AuthenticationContext = PlayFabContext;
				TSharedPtr<UPlayFabAuthenticationContext> SafeContext = TSharedPtr<UPlayFabAuthenticationContext>(
					RawContext,
					[](UPlayFabAuthenticationContext* Ptr)
					{
						// No delete here — GC will clean up
					});

				AuthenticationContext->EntityId = Response.Entity->Id;
				AuthenticationContext->EntityType = Response.Entity->Type;
				AuthenticationContext->EntityToken = Response.EntityToken;
				AuthenticationContext->PlayFabAuthenticationContext = SafeContext;

				CoreManagerReference->SetSharedAuthenticationContext(AuthenticationContext);
				SaveAuthenticationContextToDisk();
				CoreManagerReference->GetCatalogItems();
				OnPlayFabLoginSuccess.Broadcast();
				UE_LOG(LogTemp, Log, TEXT("Got EntityToken: %s"), *AuthenticationContext->EntityToken);
			}),
		PlayFab::FPlayFabErrorDelegate::CreateLambda([this](const PlayFab::FPlayFabCppError& Error)
		{
			AuthenticationContext = nullptr;
			UE_LOG(LogTemp, Error, TEXT("Failed to get entity token: %s"), *Error.ErrorMessage);
		}));
}

void UPlayFabLoginManager::OnRegisterSuccess(const FRegisterPlayFabUserResult& Result)
{
	UE_LOG(LogTemp, Log, TEXT("Successfully registered PlayFab user! Username: %s"), *Result.Username);
	AuthenticationContext = NewObject<UHelaAuthenticationContext>();
	AuthenticationContext->SetBasicAuthenticationContext(Result.EntityToken->EntityToken, Result.EntityToken->Entity->Id, Result.EntityToken->Entity->Type, Result.SessionTicket, Result.PlayFabId);
	FUpdateAvatarUrlRequest Request;

	// Default image. To be replaced with config or something.
	Request.ImageUrl = TEXT("https://pub-e2149c3a5e7e473d9b6577fc11fa3e33.r2.dev/DefaultAvatar.png");

	ClientAPI->UpdateAvatarUrl(
		Request,
		PlayFab::UPlayFabClientAPI::FUpdateAvatarUrlDelegate::CreateLambda([this](const FEmptyResponse& Result)
		{
			UE_LOG(LogTemp, Log, TEXT("Avatar URL updated successfully."));
			GetAccountInfo();
		}),
		FPlayFabErrorDelegate::CreateLambda([](const FPlayFabCppError& Error)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to update avatar URL: %s"), *Error.ErrorMessage);
		}));
}

void UPlayFabLoginManager::OnRegisterFailure(const FPlayFabCppError& Error)
{
	UE_LOG(LogTemp, Error, TEXT("PlayFab registration failed: %s"), *Error.ErrorMessage);
}

void UPlayFabLoginManager::OnLoginSuccess(const FLoginResult& Result)
{
	UE_LOG(LogTemp, Log, TEXT("Login Success! PlayFab ID: %s"), *Result.PlayFabId);
	AuthenticationContext = NewObject<UHelaAuthenticationContext>();
	AuthenticationContext->SetBasicAuthenticationContext(Result.EntityToken->EntityToken, Result.EntityToken->Entity->Id, Result.EntityToken->Entity->Type, Result.SessionTicket, Result.PlayFabId);
	GetAccountInfo();
}

void UPlayFabLoginManager::OnLoginFailure(const FPlayFabCppError& Error)
{
	UE_LOG(LogTemp, Error, TEXT("Login failed: %s"), *Error.ErrorMessage);
}

void UPlayFabLoginManager::SaveAuthenticationContextToDisk() const
{
	FString JsonOutput;

	FAuthenticationData Data;

	if (AuthenticationContext != nullptr)
	{
		Data.EntityToken = AuthenticationContext->EntityToken;
		Data.SessionTicket = AuthenticationContext->SessionTicket;
		Data.Id = AuthenticationContext->Id;
		Data.EntityId = AuthenticationContext->EntityId;
		Data.EntityType = AuthenticationContext->EntityType;
		Data.UserName = AuthenticationContext->UserName;
		Data.AvatarUrl = AuthenticationContext->AvatarUrl;
	}

	if (FJsonObjectConverter::UStructToJsonObjectString(Data, JsonOutput))
	{
		FFileHelper::SaveStringToFile(JsonOutput, *(FPaths::ProjectSavedDir() + "auth.json"));
	}
}

bool UPlayFabLoginManager::LoadAuthenticationContextFromDisk()
{
	FString JsonInput;
	if (FFileHelper::LoadFileToString(JsonInput, *(FPaths::ProjectSavedDir() + "auth.json")))
	{
		FAuthenticationData Data;
		if (FJsonObjectConverter::JsonObjectStringToUStruct(JsonInput, &Data, 0, 0))
		{
			AuthenticationContext = NewObject<UHelaAuthenticationContext>();
			AuthenticationContext->EntityToken = Data.EntityToken;
			AuthenticationContext->SessionTicket = Data.SessionTicket;
			AuthenticationContext->Id = Data.Id;
			AuthenticationContext->EntityId = Data.EntityId;
			AuthenticationContext->EntityType = Data.EntityType;
			AuthenticationContext->UserName = Data.UserName;
			AuthenticationContext->AvatarUrl = Data.AvatarUrl;
			return true;
		}
	}
	return false;
}

void UPlayFabLoginManager::OnTokenValidated(const PlayFab::ClientModels::FGetAccountInfoResult& GetAccountInfoResult)
{
}

void UPlayFabLoginManager::OnTokenInvalid(const PlayFab::FPlayFabCppError& PlayFabCppError)
{
	AuthenticationContext = nullptr;
	OnPlayFabLoginFail.Broadcast(PlayFabCppError.ErrorCode, PlayFabCppError.ErrorMessage);
}

void UPlayFabLoginManager::GetAccountInfo()
{
	FGetAccountInfoRequest Request;

	ClientAPI->GetAccountInfo(Request, UPlayFabClientAPI::FGetAccountInfoDelegate::CreateLambda([this](const FGetAccountInfoResult& GetAccountInfoResult)
	                          {
		                          AuthenticationContext->UserName = GetAccountInfoResult.AccountInfo->TitleInfo->DisplayName;
		                          AuthenticationContext->AvatarUrl = GetAccountInfoResult.AccountInfo->TitleInfo->AvatarUrl;

		                          UE_LOG(LogTemp, Display, TEXT("user is authenticated with username: %s"), *AuthenticationContext->UserName);
		                          ApiAuthentication();
	                          }),
	                          FPlayFabErrorDelegate::CreateLambda(
		                          [this](const PlayFab::FPlayFabCppError& PlayFabCppError)
		                          {
			                          AuthenticationContext = nullptr;
			                          OnPlayFabLoginFail.Broadcast(PlayFabCppError.ErrorCode, PlayFabCppError.ErrorMessage);
		                          }));
}
