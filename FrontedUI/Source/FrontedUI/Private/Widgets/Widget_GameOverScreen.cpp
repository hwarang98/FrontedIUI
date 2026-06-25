// LeeHwaRang All Rights Reserved

#include "Widgets/Widget_GameOverScreen.h"

#include "Subsystems/FrontendUISubsystem.h"
#include "Widgets/Components/FrontendCommonButtonBase.h"

void UWidget_GameOverScreen::NativeConstruct()
{
	Super::NativeConstruct();

	Button_Retry->OnClicked().AddUObject(this, &ThisClass::HandleRetry);
	Button_Title->OnClicked().AddUObject(this, &ThisClass::HandleTitle);
}

UWidget* UWidget_GameOverScreen::NativeGetDesiredFocusTarget() const
{
	return Button_Retry;
}

void UWidget_GameOverScreen::HandleRetry()
{
	UFrontendUISubsystem::Get(this)->StartNewGame();
}

void UWidget_GameOverScreen::HandleTitle()
{
	UFrontendUISubsystem::Get(this)->ReturnToFrontend();
}
