// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "Widget_PrimaryLayout.generated.h"

class UCommonActivatableWidgetContainerBase;

/**
 * @brief 게임플레이 태그 기반으로 위젯 스택을 관리하는 최상위 레이아웃 위젯
 *
 * CommonUserWidget을 상속받아 여러 CommonActivatableWidgetContainerBase 스택을
 * 게임플레이 태그로 식별하고 등록·조회하는 기능을 제공합니다.
 * 블루프린트에서 각 스택을 태그와 함께 등록한 뒤, 코드에서 태그로 스택을 꺼내 사용합니다.
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class FRONTEDUI_API UWidget_PrimaryLayout : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * @brief 게임플레이 태그에 해당하는 위젯 스택을 반환합니다.
	 * @param InTag 조회할 위젯 스택의 게임플레이 태그
	 * @return 해당 태그에 등록된 스택 포인터, 없으면 nullptr
	 */
	UCommonActivatableWidgetContainerBase* FindWidgetStackByTag(const FGameplayTag& InTag) const;


protected:
	/**
	 * @brief 게임플레이 태그와 위젯 스택을 내부 맵에 등록합니다.
	 * @param InStackTag 등록할 위젯 스택을 식별하는 게임플레이 태그
	 * @param InStack    등록할 UCommonActivatableWidgetContainerBase 포인터
	 * @note  디자인 타임(에디터 프리뷰)에는 동작하지 않으며,
	 *        동일한 태그가 이미 등록되어 있으면 중복 등록을 무시합니다.
	 */
	UFUNCTION(BlueprintCallable)
	void RegisterWidgetStack(UPARAM(meta = (Categories = "Frontend.WidgetStack")) FGameplayTag InStackTag, UCommonActivatableWidgetContainerBase* InStack);

private:
	/**
	 * @brief 게임플레이 태그를 키로, 위젯 스택 포인터를 값으로 저장하는 맵
	 *
	 * Transient로 지정되어 저장/로드 시 직렬화에서 제외됩니다.
	 * 런타임에 RegisterWidgetStack 호출로만 채워집니다.
	 */
	UPROPERTY(Transient)
	TMap<FGameplayTag, UCommonActivatableWidgetContainerBase*> RegisterWidgetStackMap;
};
