#include "Authentication/UI/AccountOverlayWidget.h"

#include "HttpModule.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Authentication/AuthenticationContext.h"
#include "Authentication/Interface/AuthenticationManagerBase.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

void UAccountOverlayWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UAccountOverlayWidget::InitializeWidget(UAuthenticationManagerBase* AuthenticationManager)
{
	LoginManager = AuthenticationManager;

	if (!LoginManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("AccountOverlayWidget not fully initialized yet"));
		return;
	}


	auto AuthenticationContext = LoginManager->GetAuthenticationContext();

	if (!AuthenticationContext)
	{
		UE_LOG(LogTemp, Error, TEXT("AuthenticationContext is null"));
		return;
	}

	if (DisplayNameText)
	{
		DisplayNameText->SetText(FText::FromString(AuthenticationContext->UserName));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DisplayNameText is null"));
	}

	DownloadAndSetAvatarImage(AuthenticationContext->AvatarUrl);
}

void UAccountOverlayWidget::DownloadAndSetAvatarImage(const FString& AvatarUrl)
{
	if (AvatarUrl.IsEmpty())
	{
		return;
	}

	TWeakObjectPtr<UAccountOverlayWidget> WeakThis(this);


	// Download and set the image
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindLambda([WeakThis](FHttpRequestPtr Request, const FHttpResponsePtr& Response, bool bWasSuccessful)
	{
		if (!WeakThis.IsValid()) return;

		UAccountOverlayWidget* Widget = WeakThis.Get();

		if (bWasSuccessful && Response.IsValid())
		{
			TArray<uint8> ImageData = Response->GetContent();

			IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
			TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

			if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
			{
				TArray64<uint8> UncompressedBGRA;
				if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
				{
					UTexture2D* AvatarTexture = UTexture2D::CreateTransient(
						ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

					void* TextureData = AvatarTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
					FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
					AvatarTexture->GetPlatformData()->Mips[0].BulkData.Unlock();

					AvatarTexture->UpdateResource();

					if (Widget->AvatarImage)
					{
						Widget->AvatarImage->SetBrushFromTexture(AvatarTexture);
					}
				}
			}
		}
	});

	HttpRequest->SetURL(AvatarUrl);
	HttpRequest->SetVerb("GET");
	HttpRequest->ProcessRequest();
}
