// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "CommonInputTypeEnum.h"
#include "Widget_KeyRemapScreen.generated.h"

class UCommonRichTextBlock;
class FKeyRemapScreenInputPreProcessor;

/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class FRONTEDUI_API UWidget_KeyRemapScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()

public:
	void SetDesiredInputTypeToFilter(ECommonInputType InDesiredInputType);

	DECLARE_DELEGATE_OneParam(FOnKeyRemapScreenKeyPressedDelegate, const FKey& /*PressedKey*/)
	FOnKeyRemapScreenKeyPressedDelegate OnKeyRemapScreenKeyPressed;

	DECLARE_DELEGATE_OneParam(FOnKeyRemapScreenKeySelectCanceledDelegate, const FString& /*CanceledReason*/)
	FOnKeyRemapScreenKeySelectCanceledDelegate OnKeyRemapScreenKeySelectCanceled;

protected:
	#pragma region UCommonActivatableWidget Interface
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	#pragma endregion

private:
	// InputPreProcessor에서 유효한 키 입력이 감지됐을 때 호출됩니다.
	void OnValidKeyPressedDetected(const FKey& PressedKey);

	// 입력 타입 불일치 또는 ESC 입력으로 키 선택이 취소됐을 때 호출됩니다.
	void OnKeySelectCanceled(const FString& CanceledReason);

	/**
	 * @brief 한 틱 지연 후 PreDeactivateCallback을 실행하고 위젯을 비활성화합니다.
	 *
	 * @param PreDeactivateCallback 위젯 비활성화 직전에 실행할 콜백
	 * @note 키 입력 이벤트가 완전히 처리된 이후 위젯이 닫히도록 FTSTicker로 한 프레임 지연합니다.
	 */
	void RequestDeactivateWidget(TFunction<void()> PreDeactivateCallback);

	#pragma region Bound Widgets
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonRichTextBlock> CommonRichText_RemapMessage;
	#pragma endregion

	TSharedPtr<FKeyRemapScreenInputPreProcessor> CachedInputPreprocessor;

	ECommonInputType CachedDesiredInputType;

};