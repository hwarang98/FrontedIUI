// LeeHwaRang All Rights Reserved


#include "AsyncActions/AsyncAction_PushSoftWidget.h"
#include "Subsystems/FrontendUISubsystem.h"

void UAsyncAction_PushSoftWidget::Activate()
{
	UFrontendUISubsystem* FrontendUISubsystem = UFrontendUISubsystem::Get(CachedOnlyWorld.Get());

	FrontendUISubsystem->PushSoftWidgetToStackAsync(CachedWidgetStackTag, CachedSoftWidgetClass,
		[this](EAsyncPushWidgetState InPushState, UWidget_ActivatableBase* PushedWidget) {
			switch (InPushState)
			{
				case EAsyncPushWidgetState::OnCreatedBeforePush:
					// 푸시 전 소유자를 설정하여 위젯이 올바른 플레이어에 귀속되도록 함
					PushedWidget->SetOwningPlayer(CachedOnlyPlayerController.Get());
					OnWidgetCreatedBeforePush.Broadcast(PushedWidget);
					break;

				case EAsyncPushWidgetState::AfterPush:
					AfterPush.Broadcast(PushedWidget);

					// 옵션이 활성화된 경우 위젯이 원하는 포커스 타깃으로 포커스 이동
					if (bCachedFocusOnNewlyPushedWidget)
					{
						if (UWidget* WidgetToFocus = PushedWidget->GetDesiredFocusTarget())
						{
							WidgetToFocus->SetFocus();
						}
					}

					// 모든 처리 완료 후 노드 수명 종료 -> GC 허용
					SetReadyToDestroy();
					break;

				default:
					break;
			}
		}
		);
}

UAsyncAction_PushSoftWidget* UAsyncAction_PushSoftWidget::PushSoftWidget(
	const UObject* WorldContextObject,
	APlayerController* OwningPlayerController,
	TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass,
	UPARAM(meta = (Categories = "Frontend.WidgetStack")) FGameplayTag InWidgetStackTag,
	bool bFocusOnNewlyPushedWidget
	)
{
	// null 소프트 클래스는 이후 비동기 로드가 불가능하므로 진입 즉시 크래시로 감지
	checkf(!InSoftWidgetClass.IsNull(), TEXT("PushSoftWidgetToStack에 비어 있는 Soft Widget Class가 전달되었습니다."));

	// LogAndReturnNull 모드로 월드를 가져와 유효하지 않은 컨텍스트는 nullptr 반환으로 처리
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		UAsyncAction_PushSoftWidget* Node = NewObject<UAsyncAction_PushSoftWidget>();

		// Activate()에서 사용할 파라미터를 노드에 캐시
		Node->CachedOnlyWorld = World;
		Node->CachedOnlyPlayerController = OwningPlayerController;
		Node->CachedSoftWidgetClass = InSoftWidgetClass;
		Node->CachedWidgetStackTag = InWidgetStackTag;
		Node->bCachedFocusOnNewlyPushedWidget = bFocusOnNewlyPushedWidget;

		// GameInstance에 등록하여 레벨 전환 중에도 노드가 GC되지 않도록 수명 보장
		Node->RegisterWithGameInstance(World);
		return Node;
	}

	return nullptr;
}