// LeeHwaRang All Rights Reserved

#include "Widgets/Widget_ClearScreen.h"

#include "CommonTextBlock.h"
#include "Subsystems/FrontendUISubsystem.h"
#include "Widgets/Components/FrontendCommonButtonBase.h"

void UWidget_ClearScreen::InitClearScreen(float ElapsedSeconds, int32 TotalReports)
{
	const int32 Minutes = FMath::FloorToInt(ElapsedSeconds / 60.f);
	const int32 Seconds = FMath::FloorToInt(FMath::Fmod(ElapsedSeconds, 60.f));

	CommonTextBlock_ElapsedTime->SetText(FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds)));

	CommonTextBlock_AnomalyCount->SetText(FText::FromString(FString::Printf(TEXT("%d개"), TotalReports)));

	Button_Retry->OnClicked().AddUObject(this, &ThisClass::HandleRetry);
	Button_Title->OnClicked().AddUObject(this, &ThisClass::HandleTitle);
	Button_Credits->OnClicked().AddUObject(this, &ThisClass::OnCreditsClicked);
}

UWidget* UWidget_ClearScreen::NativeGetDesiredFocusTarget() const
{
	return Button_Retry;
}

void UWidget_ClearScreen::HandleRetry()
{
	UFrontendUISubsystem::Get(this)->StartNewGame();
}

void UWidget_ClearScreen::HandleTitle()
{
	UFrontendUISubsystem::Get(this)->ReturnToFrontend();
}