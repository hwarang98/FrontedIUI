// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widget_ListEntry_Base.h"
#include "Widget_ListEntry_Action.generated.h"

class UFrontendCommonButtonBase;
class UListDataObject_Action;

/** 실행형 액션 데이터에 대응하는 리스트 엔트리 위젯 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class FRONTEDUI_API UWidget_ListEntry_Action : public UWidget_ListEntry_Base
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject) override;

private:
	void OnActionButtonClicked();

	#pragma region Bound Widgets
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget, AllowPrivateAccess = "true"))
	TObjectPtr<UFrontendCommonButtonBase> CommonButton_Action;
	#pragma endregion

	UPROPERTY(Transient)
	TObjectPtr<UListDataObject_Action> CachedOwningActionDataObject;
};
