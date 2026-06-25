// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "FrontendTypes/FrontendEnum.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "FrontendUISubsystem.generated.h"

class UCommonActivatableWidgetContainerBase;
class UWidget_ActivatableBase;
class UWidget_PrimaryLayout;

/**
 * @brief PushSoftWidgetToStackAsync 콜백이 호출되는 시점을 나타내는 열거형
 *
 * 비동기 로드 완료 후 위젯 추가 흐름에서 두 단계에 걸쳐 콜백이 발생합니다.
 */
enum class EAsyncPushWidgetState : uint8
{
	OnCreatedBeforePush, // 위젯 인스턴스가 생성된 직후, 스택에 푸시되기 전 — 초기화 작업에 사용
	AfterPush            // 위젯이 스택에 푸시된 직후 — 푸시 완료 후 추가 처리에 사용
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnButtonDescriptionTextUpdatedDelegate, UFrontendCommonButtonBase*, BroadcastingButton, FText, DescriptionText);

/**
 * @brief Frontend UI 전반을 관리하는 GameInstance 서브시스템
 *
 * UGameInstanceSubsystem을 상속받아 게임 인스턴스 생명주기 동안 유지됩니다.
 * 데디케이티드 서버에서는 생성되지 않으며, 파생 클래스가 존재하면 기본 클래스는 생성을 생략합니다.
 */
UCLASS()
class FRONTEDUI_API UFrontendUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * @brief WorldContextObject로부터 UFrontendUISubsystem 인스턴스를 반환하는 정적 헬퍼
	 * @param WorldContextObject 현재 월드를 식별하기 위한 컨텍스트 오브젝트
	 * @return 해당 GameInstance에 속한 UFrontendUISubsystem 포인터, GEngine이 없으면 nullptr
	 */
	static UFrontendUISubsystem* Get(const UObject* WorldContextObject);

	// ~ Begin USubSystem Interface

	/**
	 * @brief 이 서브시스템의 생성 여부를 결정합니다.
	 * @param Outer 이 서브시스템을 소유할 UGameInstance 오브젝트
	 * @return 데디케이티드 서버가 아니고 파생 클래스가 없을 때 true
	 * @note 파생 클래스가 존재하면 기본 구현 대신 파생 클래스가 사용되도록 false를 반환합니다.
	 */
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	// ~ End USubSystem Interface

	/**
	 * @brief 생성된 PrimaryLayout 위젯을 서브시스템에 등록합니다.
	 * @param InCreatedWidget 등록할 UWidget_PrimaryLayout 포인터 (nullptr 불허)
	 * @note 블루프린트에서 PrimaryLayout이 생성된 직후 호출하여 서브시스템이 참조를 보유하도록 합니다.
	 */
	UFUNCTION(BlueprintCallable)
	void RegisterCreatedPrimaryLayout(UWidget_PrimaryLayout* InCreatedWidget);

	/**
	 * @brief 소프트 클래스 레퍼런스로 위젯을 비동기 로드한 뒤 지정한 스택에 푸시합니다.
	 * @param InWidgetStackTag         위젯을 추가할 스택을 식별하는 게임플레이 태그
	 * @param InSoftWidgetClass        비동기로 로드할 위젯의 소프트 클래스 포인터 (null 불허)
	 * @param AsyncPushStateCallback   로드 완료 후 두 시점(OnCreatedBeforePush, AfterPush)에 호출되는 콜백
	 * @note 에셋 로드는 UAssetManager의 StreamableManager를 통해 비동기로 처리됩니다.
	 *       콜백은 로드 완료 후 같은 프레임에서 순차적으로 두 번 호출됩니다.
	 */
	void PushSoftWidgetToStackAsync(const FGameplayTag& InWidgetStackTag, TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass, TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableBase*)> AsyncPushStateCallback);

	/**
	 * @brief DeveloperSettings에 지정된 메인 게임 레벨로 전환합니다.
	 *
	 * @note LoadingScreenSubsystem이 PreLoadMapWithContext 델리게이트를 통해 로딩 화면을 자동으로 처리합니다.
	 */
	UFUNCTION(BlueprintCallable)
	void StartNewGame();

	/**
	 * @brief DeveloperSettings에 지정된 프론트엔드 레벨로 전환합니다.
	 *
	 * @note LoadingScreenSubsystem이 PreLoadMapWithContext 델리게이트를 통해 로딩 화면을 자동으로 처리합니다.
	 */
	UFUNCTION(BlueprintCallable)
	void ReturnToFrontend();

	/**
	 * @brief 확인 팝업을 Modal 스택에 비동기로 푸시합니다.
	 * @param InScreenType          표시할 버튼 구성 타입 (Ok / YesNo / OKCancel)
	 * @param InScreenTitle         팝업 제목 텍스트
	 * @param InScreenMessage       팝업 본문 메시지 텍스트
	 * @param ButtonClickedCallback 버튼 클릭 시 호출되는 콜백 — 클릭된 버튼의 EConfirmScreenButtonType을 전달
	 * @note InScreenType이 UnKnow이면 InfoObject가 nullptr이 되어 check()에서 크래시합니다.
	 */
	void PushConfirmScreenToModalStackAsync(EConfirmScreenType InScreenType, const FText& InScreenTitle, const FText& InScreenMessage, TFunction<void(EConfirmScreenButtonType)> ButtonClickedCallback);

	/**
	 * @brief HUD 스택에 메시지 알림을 푸시하고 Duration 초 후 자동으로 닫는다.
	 *
	 * @param Message  화면에 표시할 텍스트
	 * @param Duration 표시 지속 시간 (초, 기본값 1.0)
	 */
	void ShowHudNotification(const FText& Message, float Duration = 1.0f);

	/**
	 * @brief 지정한 스택 태그에 대기 중인 푸시 요청을 소진합니다.
	 * @note Widget_PrimaryLayout::RegisterWidgetStack 에서 스택이 등록된 직후 호출합니다.
	 *       에셋이 캐시된 경우 async 콜백이 스택 등록 전에 실행되어 큐에 쌓인 요청을 처리합니다.
	 */
	void FlushPendingPushesForStack(const FGameplayTag& InStackTag, UCommonActivatableWidgetContainerBase* InStack);

	/** 버튼의 설명 텍스트가 갱신될 때 브로드캐스트 — 버튼 설명 UI를 구독하는 쪽에서 바인딩 */
	UPROPERTY(BlueprintAssignable)
	FOnButtonDescriptionTextUpdatedDelegate OnButtonDescriptionTextUpdated;

private:
	struct FPendingWidgetPush
	{
		FGameplayTag WidgetStackTag;
		TWeakObjectPtr<UClass> LoadedWidgetClass;
		TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableBase*)> Callback;
	};

	/**
	 * @brief 현재 활성화된 PrimaryLayout 위젯에 대한 참조
	 *
	 * Transient로 지정되어 직렬화에서 제외됩니다.
	 * RegisterCreatedPrimaryLayout 호출 시 설정되며, 다른 시스템이 레이아웃에 접근할 때 사용됩니다.
	 */
	UPROPERTY(Transient)
	TObjectPtr<UWidget_PrimaryLayout> CreatedPrimaryLayout;

	/** 스택이 아직 등록되지 않아 지연된 위젯 푸시 요청 목록 */
	TArray<FPendingWidgetPush> PendingWidgetPushes;
};