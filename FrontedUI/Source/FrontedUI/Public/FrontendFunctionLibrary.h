// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FrontendFunctionLibrary.generated.h"

class UWidget_ActivatableBase;

/**
 * @brief Frontend UI 관련 블루프린트 유틸리티 함수 모음
 *
 * UBlueprintFunctionLibrary를 상속받아 어느 블루프린트에서도 인스턴스 없이 호출 가능합니다.
 * FrontendDeveloperSettings에 등록된 위젯 맵을 조회하는 헬퍼 함수를 제공합니다.
 */
UCLASS()
class FRONTEDUI_API UFrontendFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * @brief 게임플레이 태그에 해당하는 위젯의 소프트 클래스 레퍼런스를 반환합니다.
	 * @param InWidgetTag 조회할 위젯을 식별하는 게임플레이 태그 (Frontend.Widget 카테고리)
	 * @return 해당 태그에 등록된 UWidget_ActivatableBase 소프트 클래스 포인터
	 * @warning 태그에 등록된 위젯이 없으면 checkf로 크래시가 발생합니다.
	 *          FrontendDeveloperSettings의 FrontendWidgetMap에 반드시 사전 등록이 필요합니다.
	 * @note BlueprintPure로 노출되어 블루프린트에서 실행 핀 없이 순수 함수 노드로 사용됩니다.
	 */
	UFUNCTION(BlueprintPure, Category = "Frontend Function Library")
	static TSoftClassPtr<UWidget_ActivatableBase> GetFrontendSoftWidgetClassByTag(UPARAM(meta = (Categories = "Frontend.Widget")) FGameplayTag InWidgetTag);
};