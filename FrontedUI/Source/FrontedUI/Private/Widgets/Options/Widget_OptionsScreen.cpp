// LeeHwaRang All Rights Reserved


#include "Widgets/Options/Widget_OptionsScreen.h"

#include "FrontendDebugHelper.h"
#include "ICommonInputModule.h"
#include "FrontendSettings/FrontendGameUserSettings.h"
#include "Input/CommonUIInputTypes.h"
#include "Subsystems/FrontendUISubsystem.h"
#include "Widgets/Components/FrontendCommonButtonBase.h"
#include "Widgets/Components/FrontendCommonListView.h"
#include "Widgets/Components/FrontendTabListWidgetBase.h"
#include "Widgets/Options/OptionsDataRegistry.h"
#include "Widgets/Options/Widget_OptionsDetailsView.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
#include "Widgets/Options/ListEntries/Widget_ListEntry_Base.h"

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
	CommonListView_OptionsList->OnItemIsHoveredChanged().AddUObject(this, &ThisClass::OnListViewItemHovered);
	CommonListView_OptionsList->OnItemSelectionChanged().AddUObject(this, &ThisClass::OnListViewItemSelected);
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

void UWidget_OptionsScreen::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	UFrontendGameUserSettings::Get()->ApplySettings(true);
}

UWidget* UWidget_OptionsScreen::NativeGetDesiredFocusTarget() const
{
	if (UObject* SelectedObject = CommonListView_OptionsList->GetSelectedItem())
	{
		if (UUserWidget* SelectedEntryWidget = CommonListView_OptionsList->GetEntryWidgetFromItem(SelectedObject))
		{
			return SelectedEntryWidget;
		}

	}
	return Super::NativeGetDesiredFocusTarget();
}

void UWidget_OptionsScreen::OnOptionsTabSelected(FName TabID)
{
	DetailsView_ListEntryInfo->ClearDetailsViewInfo();

	const TArray<UListDataObject_Base*> FoundListSourceItems = GetOrCreateDataRegistry()->GetListSourceItemsBySelectedTabID(TabID);

	CommonListView_OptionsList->SetListItems(FoundListSourceItems);
	CommonListView_OptionsList->RequestRefresh();

	if (CommonListView_OptionsList->GetSelectedItem() == nullptr)
	{
		CommonListView_OptionsList->NavigateToIndex(0);
		CommonListView_OptionsList->SetSelectedIndex(0);
	}

	ResettableDataArray.Empty();

	for (UListDataObject_Base* FoundListSourceItem : FoundListSourceItems)
	{
		if (!FoundListSourceItem)
		{
			continue;
		}

		if (!FoundListSourceItem->OnListDataModified.IsBoundToObject(this))
		{
			FoundListSourceItem->OnListDataModified.AddUObject(this, &ThisClass::OnListViewListDataModified);
		}

		if (FoundListSourceItem->CanResetBackToDefaultValue())
		{
			ResettableDataArray.AddUnique(FoundListSourceItem);
		}
	}

	if (ResettableDataArray.IsEmpty())
	{
		RemoveActionBinding(ResetActionHandle);
	}
	else
	{
		if (!GetActionBindings().Contains(ResetActionHandle))
		{
			AddActionBinding(ResetActionHandle);
		}
	}
}

void UWidget_OptionsScreen::OnListViewItemHovered(UObject* InHoveredItem, bool bWasHovered)
{
	if (!InHoveredItem)
	{
		return;
	}

	UWidget_ListEntry_Base* HoveredEntryWidget = CommonListView_OptionsList->GetEntryWidgetFromItem<UWidget_ListEntry_Base>(InHoveredItem);

	check(HoveredEntryWidget)

	HoveredEntryWidget->NativeOnListEntryWidgetHovered(bWasHovered);

	if (bWasHovered)
	{
		DetailsView_ListEntryInfo->UpdateDetailsViewInfo(CastChecked<UListDataObject_Base>(InHoveredItem), TryGetEntryWidgetClassName(InHoveredItem));
	}
	else
	{
		if (UListDataObject_Base* SelectedItem = CommonListView_OptionsList->GetSelectedItem<UListDataObject_Base>())
		{
			DetailsView_ListEntryInfo->UpdateDetailsViewInfo(SelectedItem, TryGetEntryWidgetClassName(InHoveredItem));
		}
	}
}

void UWidget_OptionsScreen::OnListViewItemSelected(UObject* InSelectedItem)
{
	if (!InSelectedItem)
	{
		return;
	}

	DetailsView_ListEntryInfo->UpdateDetailsViewInfo(CastChecked<UListDataObject_Base>(InSelectedItem), TryGetEntryWidgetClassName(InSelectedItem));

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
	if (ResettableDataArray.IsEmpty())
	{
		return;
	}

	UCommonButtonBase* SelectedTabButton = TabListWidget_OptionsTabs->GetTabButtonBaseByID(TabListWidget_OptionsTabs->GetActiveTab());

	const FString SelectedTabButtonName = CastChecked<UFrontendCommonButtonBase>(SelectedTabButton)->GetButtonDisplayText().ToString();

	UFrontendUISubsystem::Get(this)->PushConfirmScreenToModalStackAsync(
		EConfirmScreenType::YesNo,
		FText::FromString(TEXT("초기화")),
		FText::FromString(TEXT("모든 ") + SelectedTabButtonName + TEXT(" 탭 설정을 초기화하시겠습니까?")),
		[this](EConfirmScreenButtonType ClickedButtonType) {
			// Yes가 아닌 버튼(No, 닫기 등)이 눌리면 초기화 취소
			if (ClickedButtonType != EConfirmScreenButtonType::Confirmed)
			{
				return;\
			}

			// true로 설정하면 OnListViewListDataModified 상단 가드에서 조기 반환되어 ResettableDataArray 변경을 차단
			bIsResettingData = true;
			bool bHasDataFailedToReset = false;

			for (UListDataObject_Base* DataToReset : ResettableDataArray)
			{
				// GC로 인해 배열 내 항목이 무효화될 수 있으므로 유효성 검사
				if (!DataToReset)
				{
					continue;
				}

				// 반환값: true = 초기화 성공, false = 초기화 실패 (데이터 타입이 초기화를 지원하지 않는 경우)
				if (DataToReset->TryResetBackToDefaultValue())
				{
				}
				else
				{
					// 초기화 실패 항목이 하나라도 있으면 루프 완료 후 배열을 유지
					bHasDataFailedToReset = true;
				}
			}

			// 일부 항목이라도 초기화에 실패하면 배열을 유지해 Reset 바인딩을 남겨둠
			if (!bHasDataFailedToReset)
			{
				ResettableDataArray.Empty();
				RemoveActionBinding(ResetActionHandle);
			}

			bIsResettingData = false;
		}
		);

}

void UWidget_OptionsScreen::OnBackBoundActionTriggered()
{
	DeactivateWidget();
}

FString UWidget_OptionsScreen::TryGetEntryWidgetClassName(const UObject* InOwningListItem) const
{
	if (const UUserWidget* FoundEntryWidget = CommonListView_OptionsList->GetEntryWidgetFromItem(InOwningListItem))
	{
		return FoundEntryWidget->GetClass()->GetName();
	}

	return TEXT("Entry Widget Not Valid");
}

void UWidget_OptionsScreen::OnListViewListDataModified(UListDataObject_Base* ModifiedData, EOptionsListDataModifyReason ModifyReason)
{
	if (!ModifiedData || bIsResettingData)
	{
		return;
	}

	// 기본값과 달라진 항목이면 초기화 대상 배열에 추가하고 Reset 바인딩 활성화
	if (ModifiedData->CanResetBackToDefaultValue())
	{
		ResettableDataArray.AddUnique(ModifiedData);

		// 중복 바인딩 방지
		if (!GetActionBindings().Contains(ResetActionHandle))
		{
			AddActionBinding(ResetActionHandle);
		}
	}
	else
	{
		// 기본값으로 돌아온 항목은 초기화 대상에서 제거
		if (ResettableDataArray.Contains(ModifiedData))
		{
			ResettableDataArray.Remove(ModifiedData);
		}
	}

	// 초기화할 항목이 없으면 Reset 바인딩 제거
	if (ResettableDataArray.IsEmpty())
	{
		RemoveActionBinding(ResetActionHandle);
	}
}