// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "CommonInputBaseTypes.h"
#include "Widget_ActivatableBase.generated.h"

class AFrontendPlayerController;

UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class FRONTEDUI_API UWidget_ActivatableBase : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

protected:
	UFUNCTION(BlueprintPure)
	AFrontendPlayerController* GetOwningFrontendPlayerController();

	// 위젯 활성화 시 입력 모드를 자동으로 전환할지 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input Config")
	bool bOverrideInputConfig = false;

	// bOverrideInputConfig가 true일 때 적용할 입력 모드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input Config", meta = (EditCondition = "bOverrideInputConfig"))
	ECommonInputMode InputModeOverride = ECommonInputMode::Menu;

	// bOverrideInputConfig가 true일 때 적용할 마우스 캡처 모드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input Config", meta = (EditCondition = "bOverrideInputConfig"))
	EMouseCaptureMode MouseCaptureModeOverride = EMouseCaptureMode::NoCapture;

private:
	TWeakObjectPtr<AFrontendPlayerController> CachedFrontendPlayerController;
};