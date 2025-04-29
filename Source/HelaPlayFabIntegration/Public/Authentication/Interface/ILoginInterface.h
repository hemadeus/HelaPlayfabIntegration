#pragma once

#include "UObject/Interface.h"
#include "ILoginInterface.generated.h"

// This is the UInterface part (needed by Unreal)
UINTERFACE(Blueprintable)
class HELAPLAYFABINTEGRATION_API ULoginInterface : public UInterface
{
	GENERATED_BODY()
};

// This is the actual interface that classes will implement
class HELAPLAYFABINTEGRATION_API ILoginInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAuthenticationManagerBase* GetLoginManager();
};
