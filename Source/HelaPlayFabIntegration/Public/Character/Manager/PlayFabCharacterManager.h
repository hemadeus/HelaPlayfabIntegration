#pragma once

#include "CoreMinimal.h"
#include "PlayFabEconomyDataModels.h"
#include "PlayFabError.h"
#include "Character/Interface/CharacterManagerBase.h"
#include "Character/Models/CharacterItem.h"
#include "PlayFabCharacterManager.generated.h"

class UImage;
struct FClientExecuteCloudScriptResult;
struct FPlayFabError;
class UPlayFabManager;
class UPlayFabLoginManager;

namespace PlayFab
{
	class UPlayFabCloudScriptAPI;
	class UPlayFabEconomyAPI;
}

namespace PlayFab
{
	class UPlayFabClientAPI;
}

UCLASS(ClassGroup=(PlayFab), meta=(BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class HELAPLAYFABINTEGRATION_API UPlayFabCharacterManager : public UCharacterManagerBase
{
	GENERATED_BODY()

public:
	virtual void GetUserCharacters_Implementation(const FOnCharactersLoaded& OnCharactersReady) override;
	virtual void CreateUserCharacter_Implementation(const FString& CatalogItemId, const FString& CharacterName, const FOnCharacterCreated& OnCharacterCreated) override;
	virtual void DeleteUserCharacter_Implementation(const FString& CharacterId) override;
	virtual void GetCatalogAvailableCharacters_Implementation(const FOnAvailableCharactersLoaded& OnCharactersReady);
	// You can also add C++-only helper methods here
	void Initialize(const FString& PlayerId);

	void InitializeManager(const TSharedPtr<PlayFab::UPlayFabClientAPI>& InClientAPI, const TSharedPtr<PlayFab::UPlayFabEconomyAPI>& InEconomyAPI,
	                       const TSharedPtr<PlayFab::UPlayFabCloudScriptAPI>& InCloudScriptAPI,
	                       UPlayFabManager* InCoreManager);


	UPROPERTY()
	UPlayFabManager* CoreManagerReference;

	/** Downloads an image from a URL and applies it to a UImage widget */
	UFUNCTION(BlueprintCallable, Category = "UI")
	static void SetImageFromURL(UImage* ImageWidget, const FString& ImageURL);

private:
	FString CachedPlayerId;
	// This is your API access object
	TSharedPtr<PlayFab::UPlayFabClientAPI> ClientAPI = nullptr;
	TSharedPtr<PlayFab::UPlayFabEconomyAPI> EconomyAPI = nullptr;
	TSharedPtr<PlayFab::UPlayFabCloudScriptAPI> CloudScriptAPI = nullptr;

	static FCharacterItem FromCatalogItem(const PlayFab::EconomyModels::FCatalogItem& CatalogItem, const PlayFab::EconomyModels::FInventoryItem& InventoryItem, const FString& Language);
	static void AddCatalogItemInfo(const PlayFab::EconomyModels::FCatalogItem& CatalogItem, FCharacterItem& Character, const FString& Language);
};
