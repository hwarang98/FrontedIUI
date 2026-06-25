// LeeHwaRang All Rights Reserved
// 게임 HUD에 일정 시간 동안 메시지를 표시하고 자동으로 닫히는 알림 위젯

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_HudNotification.generated.h"

class UCommonTextBlock;

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class FRONTEDUI_API UWidget_HudNotification : public UUserWidget
{
	GENERATED_BODY()

public:
	/** @brief 표시할 메시지와 지속 시간을 설정한다. AddToViewport 전에 호출해야 한다. */
	void InitNotification(const FText& Message, float Duration = 1.0f);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	void AutoRemove();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> CommonTextBlock_Message;

	FTimerHandle AutoRemoveTimerHandle;
	float NotificationDuration = 1.0f;
};
