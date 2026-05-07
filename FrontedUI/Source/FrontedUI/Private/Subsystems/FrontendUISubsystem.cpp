// LeeHwaRang All Rights Reserved


#include "Subsystems/FrontendUISubsystem.h"
#include "Engine/AssetManager.h"
#include "FrontendDebugHelper.h"
#include "FrontendFunctionLibrary.h"
#include "FrontendGameplayTags.h"
#include "Widgets/Widget_PrimaryLayout.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widgets/Widget_ConfirmScreen.h"

UFrontendUISubsystem* UFrontendUISubsystem::Get(const UObject* WorldContextObject)
{
	if (GEngine)
	{
		// Assert 모드로 월드를 가져와 nullptr 반환 대신 즉시 크래시로 잘못된 컨텍스트를 감지
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);

		return UGameInstance::GetSubsystem<UFrontendUISubsystem>(World->GetGameInstance());
	}

	return nullptr;
}

bool UFrontendUISubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// 데디케이티드 서버에서는 UI가 필요 없으므로 생성 생략
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> FoundClasses;
		GetDerivedClasses(GetClass(), FoundClasses);

		// 파생 클래스가 있으면 해당 클래스가 대신 생성되므로 기본 클래스는 생성하지 않음
		return FoundClasses.IsEmpty();
	}
	return false;
}

void UFrontendUISubsystem::RegisterCreatedPrimaryLayout(UWidget_PrimaryLayout* InCreatedWidget)
{
	// nullptr 전달은 프로그래밍 오류이므로 즉시 크래시로 감지
	check(InCreatedWidget);

	CreatedPrimaryLayout = InCreatedWidget;
}

void UFrontendUISubsystem::PushSoftWidgetToStackAsync(const FGameplayTag& InWidgetStackTag, TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass, TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableBase*)> AsyncPushStateCallback)
{
	// null 소프트 레퍼런스는 로드 자체가 불가능하므로 진입 즉시 크래시로 감지
	check(!InSoftWidgetClass.IsNull());

	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		InSoftWidgetClass.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
			[InSoftWidgetClass, this, InWidgetStackTag, AsyncPushStateCallback]() {
				UClass* LoadedWidgetClass = InSoftWidgetClass.Get();

				// 로드 실패 또는 PrimaryLayout 미등록 상태는 진행 불가
				check(LoadedWidgetClass && CreatedPrimaryLayout);

				UCommonActivatableWidgetContainerBase* FoundWidgetStack = CreatedPrimaryLayout->FindWidgetStackByTag(InWidgetStackTag);

				// AddWidget 내부에서 인스턴스 생성 직후 OnCreatedBeforePush 콜백을 실행하여 Push 전 초기화 기회 제공
				UWidget_ActivatableBase* CreatedWidget = FoundWidgetStack->AddWidget<UWidget_ActivatableBase>(
					LoadedWidgetClass,
					[AsyncPushStateCallback](UWidget_ActivatableBase& CreatedWidgetInstance) {
						AsyncPushStateCallback(EAsyncPushWidgetState::OnCreatedBeforePush, &CreatedWidgetInstance);
					}
					);

				// 스택 푸시 완료 후 AfterPush 콜백으로 결과 위젯 전달
				AsyncPushStateCallback(EAsyncPushWidgetState::AfterPush, CreatedWidget);
			}
			)
		);
}

void UFrontendUISubsystem::PushConfirmScreenToModalStackAsync(EConfirmScreenType InScreenType, const FText& InScreenTitle, const FText& InScreenMessage, TFunction<void(EConfirmScreenButtonType)> ButtonClickedCallback)
{
	// InScreenType에 따라 버튼 구성이 다른 InfoObject를 팩토리 함수로 생성
	UConfirmScreenInfoObject* CreatedInfoObject = nullptr;

	switch (InScreenType)
	{
		case EConfirmScreenType::Ok:
			// 단순 알림 — OK 버튼(Closed) 하나만 생성
			CreatedInfoObject = UConfirmScreenInfoObject::CreateOKScreen(InScreenTitle, InScreenMessage);
			break;

		case EConfirmScreenType::YesNo:
			// 선택 — Yes(Confirmed) / No(Cancelled) 두 버튼 생성
			CreatedInfoObject = UConfirmScreenInfoObject::CreateYesNoScreen(InScreenTitle, InScreenMessage);
			break;

		case EConfirmScreenType::OKCancel:
			// 선택 — OK(Confirmed) / Cancel(Cancelled) 두 버튼 생성
			CreatedInfoObject = UConfirmScreenInfoObject::CreateOkCancelScreen(InScreenTitle, InScreenMessage);
			break;

		case EConfirmScreenType::UnKnow:
			// UnKnow는 유효하지 않은 타입 — 이후 check()에서 크래시로 잘못된 호출을 감지
			break;

		default:
			break;
	}

	// InfoObject가 nullptr이면 유효하지 않은 InScreenType이 전달된 것
	check(CreatedInfoObject);

	PushSoftWidgetToStackAsync(
		FrontendGameplayTags::Frontend_WidgetStack_Modal,
		UFrontendFunctionLibrary::GetFrontendSoftWidgetClassByTag(FrontendGameplayTags::Frontend_Widget_ConfirmScreen),
		[CreatedInfoObject, ButtonClickedCallback](EAsyncPushWidgetState InPushState, UWidget_ActivatableBase* PushedWidget) {
			// OnCreatedBeforePush 단계에서만 초기화 — Push 전에 데이터를 주입해야 위젯이 올바르게 표시됨
			if (InPushState == EAsyncPushWidgetState::OnCreatedBeforePush)
			{
				UWidget_ConfirmScreen* CreatedConfirmScreen = CastChecked<UWidget_ConfirmScreen>(PushedWidget);
				CreatedConfirmScreen->InitConfirmScreen(CreatedInfoObject, ButtonClickedCallback);
			}
		}
		);
}