// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GameplayTagContainer.h"
#include "AsyncAction_PushSoftWidget.generated.h"


class UWidget_ActivatableBase;

/**
 * @brief 위젯이 푸시될 때 발생하는 델리게이트
 * @param PushedWidget 생성 또는 푸시된 UWidget_ActivatableBase 인스턴스
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPushSoftWidgetDelegate, UWidget_ActivatableBase*, PushedWidget);

/**
 * @brief 소프트 클래스 레퍼런스로 위젯을 비동기 로드하여 위젯 스택에 푸시하는 블루프린트 비동기 액션
 *
 * UBlueprintAsyncActionBase를 상속받아 블루프린트 잠복 노드로 노출됩니다.
 * PushSoftWidget(팩토리)에서 파라미터를 캐시한 뒤, Activate()에서 실제 비동기 로드와 푸시를 수행합니다.
 * 에셋 로드 완료 후 OnWidgetCreatedBeforePush → AfterPush 순서로 델리게이트가 실행됩니다.
 */
UCLASS()
class FRONTEDUI_API UAsyncAction_PushSoftWidget : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	/**
	 * @brief 블루프린트 노드가 실행될 때 UE가 자동으로 호출하는 진입점
	 *
	 * 캐시된 파라미터로 UFrontendUISubsystem::PushSoftWidgetToStackAsync를 호출하고
	 * 콜백에서 델리게이트를 브로드캐스트합니다.
	 * AfterPush 처리 완료 후 SetReadyToDestroy()로 노드 수명을 종료합니다.
	 */
	virtual void Activate() override;

	/**
	 * @brief 블루프린트에서 호출하는 비동기 위젯 푸시 노드의 팩토리 함수
	 * @param WorldContextObject        현재 월드를 식별하기 위한 컨텍스트 오브젝트 (자동 바인딩)
	 * @param OwningPlayerController    위젯을 소유할 플레이어 컨트롤러
	 * @param InSoftWidgetClass         비동기로 로드할 위젯의 소프트 클래스 포인터 (null 불허)
	 * @param InWidgetStackTag          위젯을 추가할 스택을 식별하는 게임플레이 태그 (Frontend.WidgetStack 카테고리)
	 * @param bFocusOnNewlyPushedWidget 푸시 후 새 위젯의 포커스 타깃에 포커스를 설정할지 여부 (기본값 true)
	 * @return 파라미터가 캐시된 UAsyncAction_PushSoftWidget 노드, 월드를 가져올 수 없으면 nullptr
	 */
	UFUNCTION(BlueprintCallable, meta=(WorldContext = "WorldContextObject", HidePin = "WorldContextObject", BlueprintInternalUseOnly = "true", DisplayName = "Push Soft Widget To Widget Stack"))
	static UAsyncAction_PushSoftWidget* PushSoftWidget(
		const UObject* WorldContextObject,
		APlayerController* OwningPlayerController,
		TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass,
		UPARAM(meta = (Categories = "Frontend.WidgetStack")) FGameplayTag InWidgetStackTag,
		bool bFocusOnNewlyPushedWidget = true
		);

	/** @brief 위젯 인스턴스가 생성된 직후, 스택에 푸시되기 전에 실행되는 델리게이트 — 초기 데이터 바인딩에 활용 */
	UPROPERTY(BlueprintAssignable)
	FOnPushSoftWidgetDelegate OnWidgetCreatedBeforePush;

	/** @brief 위젯이 스택에 푸시된 직후 실행되는 델리게이트 — 푸시 완료 후 후처리에 활용 */
	UPROPERTY(BlueprintAssignable)
	FOnPushSoftWidgetDelegate AfterPush;

private:
	/** @brief Activate()에서 서브시스템 조회에 사용할 월드 (WeakPtr로 수명 안전 보장) */
	TWeakObjectPtr<UObject> CachedOnlyWorld;

	/** @brief OnCreatedBeforePush 시점에 위젯 소유자로 설정할 플레이어 컨트롤러 */
	TWeakObjectPtr<APlayerController> CachedOnlyPlayerController;

	/** @brief 비동기 로드할 위젯의 소프트 클래스 레퍼런스 */
	TSoftClassPtr<UWidget_ActivatableBase> CachedSoftWidgetClass;

	/** @brief 위젯을 추가할 스택을 식별하는 게임플레이 태그 */
	FGameplayTag CachedWidgetStackTag;

	/** @brief AfterPush 시점에 위젯의 포커스 타깃에 포커스를 설정할지 여부 */
	bool bCachedFocusOnNewlyPushedWidget = false;
};
