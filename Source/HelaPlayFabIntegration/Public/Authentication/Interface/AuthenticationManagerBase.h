#pragma once

#include "AuthenticationManagerBase.generated.h"

class UHelaAuthenticationContext;

UCLASS(Blueprintable)
class HELAPLAYFABINTEGRATION_API UAuthenticationManagerBase : public UObject
{
	GENERATED_BODY()

public:
	/** Must be BlueprintImplementableEvent to be callable in BP */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Login")
	void RegisterNewUser(const FString& Email, const FString& Password, const FString& Username);
	virtual void RegisterNewUser_Implementation(const FString& Email, const FString& Password, const FString& Username);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Login")
	void LoginExistingUser(const FString& Email, const FString& Password);
	virtual void LoginExistingUser_Implementation(const FString& Email, const FString& Password);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Login")
	bool IsUserLoggedIn();
	virtual bool IsUserLoggedIn_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Login")
	void LoginOrCreateWithDeviceID();
	virtual void LoginOrCreateWithDeviceID_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Login")
	void AutoLoginIfPossible();
	virtual void AutoLoginIfPossible_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Login")
	UHelaAuthenticationContext* GetAuthenticationContext();
	virtual UHelaAuthenticationContext* GetAuthenticationContext_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Login")
	void SetAuthenticationContext(UHelaAuthenticationContext* InContext);
	virtual void SetAuthenticationContext_Implementation(UHelaAuthenticationContext* InContext);

protected:
	UPROPERTY()
	UHelaAuthenticationContext*AuthenticationContext = nullptr;
};
