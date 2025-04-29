#include "Authentication/UI/RegisterWidget.h"

#include "Authentication/Interface/AuthenticationManagerBase.h"
#include "Authentication/Interface/ILoginInterface.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"

void URegisterWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (RegisterButton)
	{
		RegisterButton->OnClicked.AddDynamic(this, &URegisterWidget::OnRegisterClicked);
	}
}

void URegisterWidget::InitializeWidget(UAuthenticationManagerBase* AuthenticationManager)
{
	LoginManager = AuthenticationManager;
}

void URegisterWidget::OnRegisterClicked()
{
	FString Email = EmailTextBox ? EmailTextBox->GetText().ToString() : TEXT("");
	FString Password = PasswordTextBox ? PasswordTextBox->GetText().ToString() : TEXT("");
	FString UserName = UserNameTextBox ? UserNameTextBox->GetText().ToString() : TEXT("");

	// Basic validation
	if (Email.IsEmpty() || Password.IsEmpty() || UserName.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("All fields are required."));
		return;
	}

	if (!Email.Contains("@") || !Email.Contains("."))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid email format."));
		return;
	}

	if (Password.Len() < 6)
	{
		UE_LOG(LogTemp, Warning, TEXT("Password must be at least 6 characters."));
		return;
	}

	if (UserName.Len() < 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Username must be at least 3 characters."));
		return;
	}

	LoginManager->RegisterNewUser(Email, Password, UserName);
}
