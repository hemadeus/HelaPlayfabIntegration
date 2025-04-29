#pragma once

#include "CoreMinimal.h"
#include "AuthenticationContext.generated.h"

// 🔼 Move the struct here BEFORE using it in the class
USTRUCT(BlueprintType)
struct FAuthenticationData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString EntityToken;

	UPROPERTY(BlueprintReadWrite)
	FString SessionTicket;

	UPROPERTY(BlueprintReadWrite)
	FString EntityId;

	UPROPERTY(BlueprintReadWrite)
	FString EntityType;

	UPROPERTY(BlueprintReadWrite)
	FString Id;

	UPROPERTY(BlueprintReadWrite)
	FString UserName;

	UPROPERTY(BlueprintReadWrite)
	FString AvatarUrl;
};

class UPlayFabAuthenticationContext;

UCLASS(BlueprintType)
class HELAPLAYFABINTEGRATION_API UHelaAuthenticationContext : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FString EntityToken;

	UPROPERTY(BlueprintReadOnly)
	FString EntityId;

	UPROPERTY(BlueprintReadOnly)
	FString EntityType;

	UPROPERTY(BlueprintReadOnly)
	FString UserName;

	UPROPERTY(BlueprintReadOnly)
	FString AvatarUrl;

	UPROPERTY(BlueprintReadOnly)
	FString Id;

	UPROPERTY(BlueprintReadOnly)
	FString SessionTicket;

	TSharedPtr<UPlayFabAuthenticationContext> PlayFabAuthenticationContext;

	UFUNCTION(BlueprintCallable, Category = "Authentication")
	void SetBasicAuthenticationContext(FString InEntityToken, FString InEntityId, FString InEntityType, FString InSessionTicket, FString InId);

	UFUNCTION(BlueprintCallable, Category = "Authentication")
	static UHelaAuthenticationContext* FromStruct(const FAuthenticationData& Data);

	UFUNCTION(BlueprintCallable, Category = "Authentication")
	FAuthenticationData ToStruct() const;
};
