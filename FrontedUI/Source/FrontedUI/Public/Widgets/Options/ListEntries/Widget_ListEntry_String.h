// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widget_ListEntry_Base.h"
#include "Widget_ListEntry_String.generated.h"

class UFrontendCommonRotator;
class UFrontendCommonButtonBase;
class UListDataObject_String;
/**
 * 문자열 목록 중 하나를 선택하는 옵션 리스트 엔트리 위젯.
 * 감소/증가 버튼과 로테이터를 통해 사용 가능한 문자열 값을 순환 선택합니다.
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class FRONTEDUI_API UWidget_ListEntry_String : public UWidget_ListEntry_Base
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	// ~UWidget_ListEntry_Base Interface
	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject) override;
	virtual void OnOwningListDataObjectModified(UListDataObject_Base* OwningModifiedData, EOptionsListDataModifyReason ModifyReason) override;
	// ~UWidget_ListEntry_Base Interface

private:
	void OnPreviousOptionButtonClicked();
	void OnNextOptionButtonClicked();
	void OnRotatorValueChanged(int32 Value, bool bUserInitiated);

	#pragma region Bound Widgets
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UFrontendCommonButtonBase> CommonButton_PreviousOption; // 이전 값으로 이동하는 버튼

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UFrontendCommonRotator> CommonRotator_AvailableOptions; // 선택 가능한 문자열 값을 순환하는 로테이터

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UFrontendCommonButtonBase> CommonButton_NextOption; // 다음 값으로 이동하는 버튼

	#pragma endregion

	UPROPERTY(Transient)
	TObjectPtr<UListDataObject_String> CachedOwningStringDataObject;
};