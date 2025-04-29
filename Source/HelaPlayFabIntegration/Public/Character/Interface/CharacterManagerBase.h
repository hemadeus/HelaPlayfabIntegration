#pragma once

#include "CharacterManagerBase.generated.h"


struct FCharacterCatalogItem;
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCharacterCreated, const FCharacterItem&, Character);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCharactersLoaded, const TArray<FCharacterItem>&, Characters);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnAvailableCharactersLoaded, const TArray<FCharacterCatalogItem>&, Characters);

UCLASS(Blueprintable)
class HELAPLAYFABINTEGRATION_API UCharacterManagerBase : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
	void GetUserCharacters(const FOnCharactersLoaded& OnCharactersReady);
	virtual void GetUserCharacters_Implementation(const FOnCharactersLoaded& OnCharactersReady);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
	void GetCatalogAvailableCharacters(const FOnAvailableCharactersLoaded& OnCharactersReady);
	virtual void GetCatalogAvailableCharacters_Implementation(const FOnAvailableCharactersLoaded& OnCharactersReady);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
	void CreateUserCharacter(const FString& CatalogItemId, const FString& CharacterName, const FOnCharacterCreated& OnCharacterCreated);
	virtual void CreateUserCharacter_Implementation(const FString& CatalogItemId, const FString& CharacterName, const FOnCharacterCreated& OnCharacterCreated);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Character")
	void DeleteUserCharacter(const FString& CharacterId);
	virtual void DeleteUserCharacter_Implementation(const FString& CharacterId);
};
