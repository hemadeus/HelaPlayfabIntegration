#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AccountOverlayWidget.generated.h"

class UImage;
class IAuthenticationManagerInterface;
class ILoginInterface;
class UEditableTextBox;
class UButton;
class UTextBlock;
class AMatchPlayerStateNew;

/**
 * 
 */
UCLASS()
class HELAPLAYFABINTEGRATION_API UAccountOverlayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void InitializeWidget(UAuthenticationManagerBase* AuthenticationManager);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DisplayNameText;

	UPROPERTY(meta = (BindWidget))
	UImage* AvatarImage;

private:
	UPROPERTY()
	UAuthenticationManagerBase* LoginManager = nullptr;

	void DownloadAndSetAvatarImage(const FString& AvatarUrl);
};
