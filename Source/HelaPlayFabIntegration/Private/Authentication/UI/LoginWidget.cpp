#include "Authentication/UI/LoginWidget.h"

#include "Authentication/Interface/AuthenticationManagerBase.h"
#include "Authentication/Interface/ILoginInterface.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (LoginButton)
	{
		LoginButton->OnClicked.AddDynamic(this, &ULoginWidget::OnLoginClicked);
	}
}

void ULoginWidget::InitializeWidget(UAuthenticationManagerBase* AuthenticationManager)
{
	LoginManager = AuthenticationManager;
}

void ULoginWidget::OnLoginClicked()
{
	FString Email = EmailTextBox ? EmailTextBox->GetText().ToString() : TEXT("");
	FString Password = PasswordTextBox ? PasswordTextBox->GetText().ToString() : TEXT("");
	LoginManager->LoginExistingUser(Email, Password);
}
