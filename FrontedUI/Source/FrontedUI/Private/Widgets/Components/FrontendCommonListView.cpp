// LeeHwaRang All Rights Reserved


#include "Widgets/Components/FrontendCommonListView.h"
#include "Editor/WidgetCompilerLog.h"
#include "Widgets/Options/DataAsset_DataListEntryMapping.h"
#include "Widgets/Options/DataObjects/ListDataObject_Base.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
#include "Widgets/Options/ListEntries/Widget_ListEntry_Base.h"


UUserWidget& UFrontendCommonListView::OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (IsDesignTime())
	{
		return Super::OnGenerateEntryWidgetInternal(Item, DesiredEntryClass, OwnerTable);
	}

	if (const TSubclassOf<UWidget_ListEntry_Base> FoundWidgetClass = DataListEntryMapping->FindEntryWidgetClassByDataObject(CastChecked<UListDataObject_Base>(Item)))
	{
		return GenerateTypedEntry<UWidget_ListEntry_Base>(FoundWidgetClass, OwnerTable);

	}
	return Super::OnGenerateEntryWidgetInternal(Item, DesiredEntryClass, OwnerTable);
}

bool UFrontendCommonListView::OnIsSelectableOrNavigableInternal(UObject* FirstSelectedItem)
{
	return !FirstSelectedItem->IsA<UListDataObject_Collection>();
}

#if WITH_EDITOR
void UFrontendCommonListView::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	if (!DataListEntryMapping)
	{
		CompileLog.Error(FText::FromString(
			TEXT("변수 DataListEntryMapping에 유효한 데이터 에셋이 할당되지 않았습니다. ") +
			GetClass()->GetName() +
			TEXT(" 정상적으로 동작하려면 유효한 데이터 에셋이 필요합니다.")
			));
	}
}
#endif