#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoginWidget.generated.h"

class UAuthenticationManagerBase;
class ILoginInterface;
class UEditableTextBox;
class UButton;
class UTextBlock;
class AMatchPlayerStateNew;

/**
 * 
 */
UCLASS()
class HELAPLAYFABINTEGRATION_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void InitializeWidget(UAuthenticationManagerBase* AuthenticationManager);

protected:
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* EmailTextBox;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* PasswordTextBox;

	UPROPERTY(meta = (BindWidget))
	UButton* LoginButton;

	UFUNCTION()
	void OnLoginClicked();

private:
	UPROPERTY()
	UAuthenticationManagerBase* LoginManager = nullptr;
};
