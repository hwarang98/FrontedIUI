// LeeHwaRang All Rights Reserved

#include "Widgets/Widget_HudNotification.h"
#include "CommonTextBlock.h"

void UWidget_HudNotification::InitNotification(const FText& Message, float Duration)
{
	NotificationDuration = Duration;
	if (CommonTextBlock_Message)
	{
		CommonTextBlock_Message->SetText(Message);
	}
}

void UWidget_HudNotification::NativeConstruct()
{
	Super::NativeConstruct();
	GetWorld()->GetTimerManager().SetTimer(AutoRemoveTimerHandle, this, &UWidget_HudNotification::AutoRemove, NotificationDuration, false);
}

void UWidget_HudNotification::NativeDestruct()
{
	Super::NativeDestruct();
	GetWorld()->GetTimerManager().ClearTimer(AutoRemoveTimerHandle);
}

void UWidget_HudNotification::AutoRemove()
{
	RemoveFromParent();
}
