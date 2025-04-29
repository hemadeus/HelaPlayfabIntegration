#pragma once

#include "CoreMinimal.h"
#include "PlayFabManager.generated.h"

namespace PlayFab
{
	class UPlayFabCloudScriptAPI;
	class UPlayFabAuthenticationAPI;

	namespace EconomyModels
	{
		struct FCatalogItem;
	}

	class UPlayFabEconomyAPI;
	class UPlayFabClientAPI;
}

class UPlayFabLoginManager;
class UPlayFabCharacterManager;
class UPlayFabUserInfoManager;
class UHelaAuthenticationContext;

UCLASS(Blueprintable)
class HELAPLAYFABINTEGRATION_API UPlayFabManager : public UObject
{
	GENERATED_BODY()

public:
	UPlayFabManager();

	UFUNCTION(BlueprintCallable, Category = "PlayFab")
	void Initialize();

	UFUNCTION(BlueprintCallable, Category = "PlayFab")
	UPlayFabLoginManager* GetLoginManager() const { return LoginManager; }

	UFUNCTION(BlueprintCallable, Category = "PlayFab")
	UPlayFabCharacterManager* GetCharacterManager() const { return CharacterManager; }

	UHelaAuthenticationContext* GetSharedAuthenticationContext() const { return SharedAuthenticationContext; }
	void SetSharedAuthenticationContext(UHelaAuthenticationContext* InContext) { SharedAuthenticationContext = InContext; }

	void GetCatalogItems();

	TArray<PlayFab::EconomyModels::FCatalogItem> CachedCatalogItems;

	FString TitleId;

private:
	TSharedPtr<PlayFab::UPlayFabClientAPI> ClientAPI = nullptr;
	TSharedPtr<PlayFab::UPlayFabEconomyAPI> EconomyAPI = nullptr;
	TSharedPtr<PlayFab::UPlayFabAuthenticationAPI> AuthenticationAPI = nullptr;
	TSharedPtr<PlayFab::UPlayFabCloudScriptAPI> CloudScriptAPI = nullptr;

	UPROPERTY()
	UPlayFabLoginManager* LoginManager;

	UPROPERTY()
	UPlayFabCharacterManager* CharacterManager;

	// UPROPERTY()
	// UPlayFabUserInfoManager* UserInfoManager;

	void InitializeSubManagers();

	bool bIsCatalogLoaded = false;

	UPROPERTY()
	UHelaAuthenticationContext* SharedAuthenticationContext = nullptr;
};
