#include "Authentication/Interface/AuthenticationManagerBase.h"

void UAuthenticationManagerBase::RegisterNewUser_Implementation(const FString& Email, const FString& Password, const FString& Username)
{
	UE_LOG(LogTemp, Warning, TEXT("[UAuthenticationManagerBase] RegisterNewUser_Implementation not overridden."));
}

void UAuthenticationManagerBase::LoginExistingUser_Implementation(const FString& Email, const FString& Password)
{
	UE_LOG(LogTemp, Warning, TEXT("[UAuthenticationManagerBase] LoginExistingUser_Implementation not overridden."));
}

bool UAuthenticationManagerBase::IsUserLoggedIn_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("[UAuthenticationManagerBase] IsUserLoggedIn_Implementation not overridden."));
	return false;
}

void UAuthenticationManagerBase::LoginOrCreateWithDeviceID_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("[UAuthenticationManagerBase] LoginOrCreateWithDeviceID_Implementation not overridden."));
}

void UAuthenticationManagerBase::AutoLoginIfPossible_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("[UAuthenticationManagerBase] AutoLoginIfPossible_Implementation not overridden."));
}

UHelaAuthenticationContext* UAuthenticationManagerBase::GetAuthenticationContext_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("[UAuthenticationManagerBase] GetAuthenticationContext_Implementation not overridden."));
	return AuthenticationContext;
}

void UAuthenticationManagerBase::SetAuthenticationContext_Implementation(UHelaAuthenticationContext* InContext)
{
	AuthenticationContext = InContext;
}
