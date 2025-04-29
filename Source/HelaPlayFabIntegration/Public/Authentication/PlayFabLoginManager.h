#pragma once
#include "CoreMinimal.h"
#include "AuthenticationContext.h"
#include "PlayFabClientDataModels.h"
#include "Interface/AuthenticationManagerBase.h"
#include "PlayFabLoginManager.generated.h"

class UPlayFabManager;

namespace PlayFab
{
	class UPlayFabAuthenticationAPI;

	namespace ClientModels
	{
		struct FLoginResult;
		struct FRegisterPlayFabUserResult;
	}

	class UPlayFabClientAPI;
	struct FPlayFabCppError;
}


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayFabLoginSuccess);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayFabLoginFail, int32, ErrorCode, const FString&, ErrorMessage);

/**
 *  Simple component that handles PlayFab registration & login.
 *  Attach to GameInstance or a persistent subsystem actor.
 */
UCLASS(ClassGroup=(PlayFab), meta=(BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class HELAPLAYFABINTEGRATION_API UPlayFabLoginManager : public UAuthenticationManagerBase
{
	GENERATED_BODY()

public:
	UPlayFabLoginManager();
	/** Register a new account with email + password, then log in. */
	virtual void RegisterNewUser_Implementation(const FString& Email, const FString& Password, const FString& Username) override;
	virtual void LoginExistingUser_Implementation(const FString& Email, const FString& Password) override;
	virtual bool IsUserLoggedIn_Implementation() override;
	virtual void LoginOrCreateWithDeviceID_Implementation() override;
	virtual void AutoLoginIfPossible_Implementation() override;

	virtual UHelaAuthenticationContext* GetAuthenticationContext_Implementation() override;

	/** Success / failure events for Blueprints. */
	UPROPERTY(BlueprintAssignable, Category="PlayFab|Login")
	FOnPlayFabLoginSuccess OnPlayFabLoginSuccess;

	UPROPERTY(BlueprintAssignable, Category="PlayFab|Login")
	FOnPlayFabLoginFail OnPlayFabLoginFail;

	void InitializeManager(TSharedPtr<PlayFab::UPlayFabClientAPI> InClientAPI, UPlayFabManager* InCoreManager, TSharedPtr<PlayFab::UPlayFabAuthenticationAPI> InAuthenticationAPI);

	void ApiAuthentication();
	void OnTokenValidated(const PlayFab::ClientModels::FGetAccountInfoResult& GetAccountInfoResult);
	void OnTokenInvalid(const PlayFab::FPlayFabCppError& PlayFabCppError);

private:
	/* Helpers that wrap PlayFab callbacks into lambdas. */
	void OnRegisterSuccess(const PlayFab::ClientModels::FRegisterPlayFabUserResult& Result);
	void OnRegisterFailure(const PlayFab::FPlayFabCppError& Error);

	void OnLoginSuccess(const PlayFab::ClientModels::FLoginResult& Result);
	void OnLoginFailure(const PlayFab::FPlayFabCppError& Error);


	TSharedPtr<PlayFab::UPlayFabClientAPI> ClientAPI = nullptr;
	TSharedPtr<PlayFab::UPlayFabAuthenticationAPI> AuthenticationAPI = nullptr;

	void SaveAuthenticationContextToDisk() const;
	bool LoadAuthenticationContextFromDisk();
	void GetAccountInfo();


	UPROPERTY()
	UPlayFabManager* CoreManagerReference = nullptr;
};
