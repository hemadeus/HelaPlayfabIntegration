#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RegisterWidget.generated.h"

class UAuthenticationManagerBase;
class UButton;
class UEditableTextBox;
class ILoginInterface;
class UPlayFabLoginManager;
class UTextBlock;
class AMatchPlayerStateNew;

/**
 * 
 */
UCLASS()
class HELAPLAYFABINTEGRATION_API URegisterWidget : public UUserWidget
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
	UEditableTextBox* UserNameTextBox;

	UPROPERTY(meta = (BindWidget))
	UButton* RegisterButton;

	UFUNCTION()
	void OnRegisterClicked();

private:
	UPROPERTY()
	UAuthenticationManagerBase* LoginManager = nullptr;
};
