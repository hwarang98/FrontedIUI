// LeeHwaRang All Rights Reserved


#include "Widgets/Options/DataObjects/ListDataObject_StringInteger.h"
#include "Widgets/Options/OptionsDataInteractionHelper.h"

void UListDataObject_StringInteger::AddIntegerOption(int32 InIntegerValue, const FText& InDisplayName)
{
	AddDynamicOption(LexToString(InIntegerValue), InDisplayName);
}

void UListDataObject_StringInteger::OnDataObjectInitialized()
{
	Super::OnDataObjectInitialized();

	if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
	{
		CurrentDisplayText = FText::FromString(TEXT("사용자 정의"));
	}
}

void UListDataObject_StringInteger::OnEditDependencyDataModified(UListDataObject_Base* ModifiedDependencyData, EOptionsListDataModifyReason ModifiedReason)
{
	if (DataDynamicGetter)
	{
		if (CurrentStringValue == DataDynamicGetter->GetValueAsString())
		{
			return;
		}

		CurrentStringValue = DataDynamicGetter->GetValueAsString();

		if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
		{
			CurrentDisplayText = FText::FromString(TEXT("사용자 정의"));
		}

		NotifyListDataModified(this, EOptionsListDataModifyReason::DependencyModified);
	}

	Super::OnEditDependencyDataModified(ModifiedDependencyData, ModifiedReason);
}