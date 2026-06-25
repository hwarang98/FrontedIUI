// LeeHwaRang All Rights Reserved
// 게임 클리어 시 소요 시간과 이상현상 수를 표시하고 다음 행동을 선택하는 위젯

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widget_ClearScreen.generated.h"

class UCommonTextBlock;
class UFrontendCommonButtonBase;

UCLASS(Abstract, BlueprintType, meta = (DisableNativeTick))
class FRONTEDUI_API UWidget_ClearScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()

public:
	/**
	 * @brief 클리어 화면을 초기화한다.
	 *
	 * @param ElapsedSeconds 게임 시작부터 클리어까지 소요된 시간 (초)
	 * @param TotalReports   전체 게임에서 정답 처리된 총 이상현상 신고 수
	 */
	void InitClearScreen(float ElapsedSeconds, int32 TotalReports);

	/** 크레딧 버튼 클릭 시 호출된다. Blueprint에서 크레딧 위젯 표시를 구현한다. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Floor404|Clear")
	void OnCreditsClicked();

protected:
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

private:
	void HandleRetry();
	void HandleTitle();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> CommonTextBlock_ElapsedTime;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> CommonTextBlock_AnomalyCount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFrontendCommonButtonBase> Button_Retry;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFrontendCommonButtonBase> Button_Title;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFrontendCommonButtonBase> Button_Credits;
};
