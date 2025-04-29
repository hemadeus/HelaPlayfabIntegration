#pragma once
#include "PlayFabAuthenticationDataModels.h"
#include "AuthenticationPlayFabManager.generated.h"
namespace PlayFab
{
	struct FPlayFabCppError;

	namespace ClientModels
	{
		struct FGetAccountInfoResult;
	}

	class UPlayFabAuthenticationAPI;
}

UCLASS(ClassGroup=(PlayFab), meta=(BlueprintSpawnableComponent), Blueprintable, BlueprintType)

class HELAPLAYFABINTEGRATION_API UAuthenticationPlayFabManager : public UObject
{
	GENERATED_BODY()

public:
	void GetEntityToken() const;
	void OnTokenValidated(const PlayFab::ClientModels::FGetAccountInfoResult& GetAccountInfoResult);
	void OnTokenInvalid(const PlayFab::FPlayFabCppError& PlayFabCppError);

protected:
	TSharedPtr<PlayFab::UPlayFabAuthenticationAPI> AuthenticationAPI = nullptr;
};
