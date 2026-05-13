// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAsset_DataListEntryMapping.generated.h"

class UWidget_ListEntry_Base;
class UListDataObject_Base;
/**
 * 데이터 오브젝트 클래스와 리스트 엔트리 위젯 클래스를 매핑하는 데이터 에셋.
 * FindEntryWidgetClassByDataObject를 통해 클래스 계층을 탐색하여 적합한 위젯 클래스를 반환합니다.
 */
UCLASS()
class FRONTEDUI_API UDataAsset_DataListEntryMapping : public UDataAsset
{
	GENERATED_BODY()

public:
	/**
	 * @brief 데이터 오브젝트의 클래스 계층을 탐색해 매핑된 엔트리 위젯 클래스를 반환합니다.
	 * 정확한 타입이 없으면 부모 클래스로 순차적으로 올라가며 탐색합니다.
	 *
	 * @param InDataObject 위젯 클래스를 찾을 데이터 오브젝트 (nullptr 불가)
	 * @return 매핑된 엔트리 위젯 클래스. 매핑이 없으면 빈 TSubclassOf 반환
	 */
	TSubclassOf<UWidget_ListEntry_Base> FindEntryWidgetClassByDataObject(UListDataObject_Base* InDataObject) const;

private:
	UPROPERTY(EditDefaultsOnly)
	TMap<TSubclassOf<UListDataObject_Base>, TSubclassOf<UWidget_ListEntry_Base>> DataObjectListEntryMap; // 데이터 오브젝트 클래스 → 엔트리 위젯 클래스 매핑 테이블


};