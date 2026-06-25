// LeeHwaRang All Rights Reserved

#include "Widgets/Options/ListEntries/Widget_ListEntry_Action.h"

#include "Widgets/Components/FrontendCommonButtonBase.h"
#include "Widgets/Options/DataObjects/ListDataObject_Action.h"

void UWidget_ListEntry_Action::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	CommonButton_Action->OnClicked().AddUObject(this, &ThisClass::OnActionButtonClicked);
}

void UWidget_ListEntry_Action::OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);
	CachedOwningActionDataObject = CastChecked<UListDataObject_Action>(InOwningListDataObject);
	CommonButton_Action->SetButtonText(InOwningListDataObject->GetDataDisplayName());
}

void UWidget_ListEntry_Action::OnActionButtonClicked()
{
	SelectThisEntryWidget();

	if (CachedOwningActionDataObject)
	{
		CachedOwningActionDataObject->ExecuteAction();
	}
}
