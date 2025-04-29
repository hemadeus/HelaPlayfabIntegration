#include "Character/Interface/CharacterManagerBase.h"

void UCharacterManagerBase::GetUserCharacters_Implementation(const FOnCharactersLoaded& OnCharactersReady)
{
	UE_LOG(LogTemp, Warning, TEXT("Base GetCharacters_Implementation called — override me"));
}

void UCharacterManagerBase::GetCatalogAvailableCharacters_Implementation(const FOnAvailableCharactersLoaded& OnCharactersReady)
{
	UE_LOG(LogTemp, Warning, TEXT("Base GetCatalogAvailableCharacters_Implementation called — override me"));
}

void UCharacterManagerBase::CreateUserCharacter_Implementation(const FString& CatalogItemId, const FString& CharacterName, const FOnCharacterCreated& OnCharacterCreated)
{
	UE_LOG(LogTemp, Warning, TEXT("Base CreateCharacter_Implementation called — override me"));
}

void UCharacterManagerBase::DeleteUserCharacter_Implementation(const FString& CharacterId)
{
	UE_LOG(LogTemp, Warning, TEXT("Base DeleteCharacter_Implementation called — override me"));
}
