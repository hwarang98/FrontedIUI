// LeeHwaRang All Rights Reserved


#include "Widgets/Options/DataObjects/ListDataObject_Base.h"

#include "FrontendSettings/FrontendGameUserSettings.h"

void UListDataObject_Base::InitDataObject()
{
	// 자식 클래스의 초기화 훅을 호출합니다. AddChildListData 시점에 자동으로 실행됩니다.
	OnDataObjectInitialized();
}

void UListDataObject_Base::AddEditCondition(const FOptionsDataEditConditionDescriptor& InEditCondition)
{
	// 조건 배열에 추가하고 IsDataCurrentlyEditable 평가 시 순서대로 검사됩니다.
	EditConditionDescArray.Add(InEditCondition);
}

void UListDataObject_Base::AddEditDependencyData(UListDataObject_Base* InDependencyData)
{
	if (!InDependencyData->OnListDataModified.IsBoundToObject(this))
	{
		InDependencyData->OnListDataModified.AddUObject(this, &ThisClass::OnEditDependencyDataModified);
	}
}

bool UListDataObject_Base::IsDataCurrentlyEditable()
{
	// 편집 가능 여부의 초기값. 조건이 하나라도 미충족되면 false로 전환됩니다.
	bool bIsEditable = true;

	// 등록된 편집 조건이 없으면 무조건 편집 가능으로 간주하고 즉시 반환합니다.
	if (EditConditionDescArray.IsEmpty())
	{
		return bIsEditable;
	}

	// 여러 조건이 동시에 미충족될 수 있으므로 이유 문자열을 누적합니다.
	FString CachedDisabledRichReason;

	for (const FOptionsDataEditConditionDescriptor& Condition : EditConditionDescArray)
	{
		// 디스크립터가 유효하지 않거나 조건이 이미 충족된 경우 처리를 건너뜁니다.
		if (!Condition.IsValid() || Condition.IsEditConditionMet())
		{
			continue;
		}

		// 조건이 충족되지 않았으므로 편집 불가 상태로 전환합니다.
		bIsEditable = false;

		// 이 조건의 비활성화 이유를 기존 문자열에 누적합니다.
		CachedDisabledRichReason.Append(Condition.GetDisabledRichReason());

		// 누적된 비활성화 이유 전체를 UI 표시용 DisabledRichText로 갱신합니다.
		SetDisabledRichText(FText::FromString(CachedDisabledRichReason));

		// 조건에 강제 값이 지정된 경우, 자식 클래스가 해당 값을 지원하면 현재 값을 강제 값으로 덮어씁니다.
		if (Condition.HasForcedStringValue())
		{
			const FString ForcedStringValue = Condition.GetDisabledForcedStringValue();

			if (CanSetToForcedStringValue(ForcedStringValue))
			{
				OnSetToForcedStringValue(ForcedStringValue);
			}
		}
	}

	return bIsEditable;
}

void UListDataObject_Base::OnDataObjectInitialized()
{
	// 기본 구현은 비어 있습니다. 자식 클래스에서 오버라이드하여 초기화 로직을 추가하세요.
}

void UListDataObject_Base::NotifyListDataModified(UListDataObject_Base* ModifiedData, EOptionsListDataModifyReason ModifiedReason)
{
	OnListDataModified.Broadcast(ModifiedData, ModifiedReason);

	if (bShouldApplyChangeImmediately)
	{
		UFrontendGameUserSettings::Get()->ApplySettings(true);
	}
}

void UListDataObject_Base::OnEditDependencyDataModified(UListDataObject_Base* ModifiedDependencyData, EOptionsListDataModifyReason ModifiedReason)
{
	OnDependencyDataModified.Broadcast(ModifiedDependencyData, ModifiedReason);
}