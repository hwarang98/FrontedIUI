// LeeHwaRang All Rights Reserved
// 게임 오버 시 재시도/타이틀 버튼을 표시하는 위젯

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widget_GameOverScreen.generated.h"

class UFrontendCommonButtonBase;

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class FRONTEDUI_API UWidget_GameOverScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

private:
	void HandleRetry();
	void HandleTitle();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFrontendCommonButtonBase> Button_Retry;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFrontendCommonButtonBase> Button_Title;
};
