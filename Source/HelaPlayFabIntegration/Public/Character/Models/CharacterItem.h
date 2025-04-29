#pragma once

#include "CoreMinimal.h"
#include "CharacterItem.generated.h"

USTRUCT(BlueprintType)
struct FCharacterItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Id;

	UPROPERTY(BlueprintReadOnly)
	FString DisplayName;

	UPROPERTY(BlueprintReadOnly)
	FString Description;

	UPROPERTY(BlueprintReadOnly)
	FString ImageUrl;

	UPROPERTY(BlueprintReadOnly)
	TArray<FString> Tags;
};

USTRUCT(BlueprintType)
struct FCharacterCatalogItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Id;

	UPROPERTY(BlueprintReadOnly)
	FString DisplayName;

	UPROPERTY(BlueprintReadOnly)
	FString Description;

	UPROPERTY(BlueprintReadOnly)
	FString ImageUrl;

	UPROPERTY(BlueprintReadOnly)
	TArray<FString> Tags;
};