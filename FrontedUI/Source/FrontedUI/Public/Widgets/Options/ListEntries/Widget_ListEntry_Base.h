// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "FrontendTypes/FrontendEnum.h"
#include "Widget_ListEntry_Base.generated.h"

class UCommonTextBlock;
class UListDataObject_Base;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class FRONTEDUI_API UWidget_ListEntry_Base : public UCommonUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On List Entry Widget Hovered"))
	void BP_OnListEntryWidgetHovered(bool bWasHovered, bool bIsEntryWidgetStillSelected);
	void NativeOnListEntryWidgetHovered(bool bWasHovered);

protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Get Widget To Focus For Gamepad"))
	UWidget* BP_GetWidgetToFocusForGamepad() const;

	// 자식 위젯 블루프린트는 이 항목 위젯이 호출되거나 선택될 때 강조 상태를 처리하도록 이를 재정의해야 합니다
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Toggle Entry Widget Highlight State"))
	void BP_OnToggleEntryWidgetHighlightState(bool bShouldHighlight) const;

	#pragma region IUserObjectListEntry Interface
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	virtual void NativeOnEntryReleased() override;
	#pragma endregion


	#pragma region UUserWidget Interface
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	#pragma endregion

	// 자식 클래스는 필요한 초기화 처리를 위해 이 함수를 오버라이드해야 합니다. 또한 `Super` 호출이 필요합니다.
	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject);

	// 자식 클래스는 데이터 오브젝트가 수정된 이후 UI 값을 갱신하기 위해 이 함수를 오버라이드해야 합니다. `Super` 호출은 필요하지 않습니다.
	virtual void OnOwningListDataObjectModified(UListDataObject_Base* OwningModifiedData, EOptionsListDataModifyReason ModifyReason);

	virtual void OnOwningDependencyDataObjectModified(UListDataObject_Base* OwningDependencyModifiedData, EOptionsListDataModifyReason ModifyReason);

	// 자식 클래스가 소유한 위젯의 편집 가능한 상태를 변경하려면 이를 재정의해야 합니다. 슈퍼 호출 필요합니다
	virtual void OnToggleEditableState(bool bIsEditable);

	void SelectThisEntryWidget();

private:
	#pragma region Bound Widgets
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional, AllowPrivateAccess = "true"))
	TObjectPtr<UCommonTextBlock> CommonText_SettingDisplayName;
	#pragma endregion

	UPROPERTY(Transient)
	UListDataObject_Base* CachedOwningListDataObject;
};