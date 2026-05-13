// LeeHwaRang All Rights Reserved


#include "Widgets/Options/Widget_OptionsScreen.h"

#include "FrontendDebugHelper.h"
#include "ICommonInputModule.h"
#include "Input/CommonUIInputTypes.h"
#include "Widgets/Components/FrontendCommonListView.h"
#include "Widgets/Components/FrontendTabListWidgetBase.h"
#include "Widgets/Options/OptionsDataRegistry.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"

void UWidget_OptionsScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// ResetAction이 설정된 경우에만 바인딩 (미설정 시 핸들 없이 무시)
	if (!ResetAction.IsNull())
	{
		ResetActionHandle = RegisterUIActionBinding(
			FBindUIActionArgs(ResetAction, true, FSimpleDelegate::CreateUObject(this, &ThisClass::OnResetBoundActionTriggered))
			);
	}

	// Back 액션은 CommonUI 기본 설정에서 가져오므로 항상 등록
	RegisterUIActionBinding(
		FBindUIActionArgs(
			ICommonInputModule::GetSettings().GetDefaultBackAction(),
			true,
			FSimpleDelegate::CreateUObject(this, &ThisClass::OnBackBoundActionTriggered))
		);

	TabListWidget_OptionsTabs->OnTabSelected.AddUniqueDynamic(this, &ThisClass::OnOptionsTabSelected);
}

void UWidget_OptionsScreen::NativeOnActivated()
{
	Super::NativeOnActivated();

	// DataRegistry에서 탭 컬렉션 목록을 받아 TabListWidget에 이미 버튼이 등록된 탭만 활성화합니다.
	for (UListDataObject_Collection* TabCollection : GetOrCreateDataRegistry()->GetRegisteredOptionsTabCollections())
	{
		if (!TabCollection)
		{
			continue;
		}

		const FName TabID = TabCollection->GetDataID();

		// BP에서 해당 ID의 탭 버튼이 이미 추가되어 있을 때만 등록 (없으면 무시)
		if (TabListWidget_OptionsTabs->GetTabButtonBaseByID(TabID) != nullptr)
		{
			continue;
		}

		TabListWidget_OptionsTabs->RequestRegisterTab(TabID, TabCollection->GetDataDisplayName());
	}
}

void UWidget_OptionsScreen::OnOptionsTabSelected(FName TabID)
{
	TArray<UListDataObject_Base*> FoundListSourceItems = GetOrCreateDataRegistry()->GetListSourceItemsBySelectedTabID(TabID);

	CommonListView_OptionsList->SetListItems(FoundListSourceItems);
	CommonListView_OptionsList->RequestRefresh();

	if (CommonListView_OptionsList->GetSelectedItem() == nullptr)
	{
		CommonListView_OptionsList->NavigateToIndex(0);
		CommonListView_OptionsList->SetSelectedIndex(0);
	}
}

UOptionsDataRegistry* UWidget_OptionsScreen::GetOrCreateDataRegistry()
{
	// 지연 초기화: 처음 요청 시 생성 및 초기화
	if (!CreatedOwningDataRegistry)
	{
		CreatedOwningDataRegistry = NewObject<UOptionsDataRegistry>();
		CreatedOwningDataRegistry->InitOptionsDataRegistry(GetOwningLocalPlayer());
	}

	checkf(CreatedOwningDataRegistry, TEXT("옵션 화면용 데이터 레지스트리가 유효하지 않습니다."))

	return CreatedOwningDataRegistry;
}

void UWidget_OptionsScreen::OnResetBoundActionTriggered()
{
	Debug::Print(TEXT("재설정 바운드 작업이 트리거됨"));
}

void UWidget_OptionsScreen::OnBackBoundActionTriggered()
{
	DeactivateWidget();
}