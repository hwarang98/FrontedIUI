// LeeHwaRang All Rights Reserved


#include "Widgets/Options/DataObjects/ListDataObject_Base.h"

#include "FrontendSettings/FrontendGameUserSettings.h"

void UListDataObject_Base::InitDataObject()
{
	OnDataObjectInitialized();
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