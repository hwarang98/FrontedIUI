// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/ListDataObject_Base.h"
#include "ListDataObject_Collection.generated.h"

/**
 * 자식 데이터 목록을 보유하는 컬렉션형 리스트 데이터 오브젝트.
 * 탭처럼 하위 항목을 가지는 계층 구조에서 사용하며,
 * UListDataObject_Base의 GetAllChildSettingData / HasAnyChildListData를 오버라이드하여 자식 배열을 관리합니다.
 */
UCLASS()
class FRONTEDUI_API UListDataObject_Collection : public UListDataObject_Base
{
	GENERATED_BODY()

public:
	/** 자식 데이터를 추가합니다. 추가 시 InitDataObject 호출 후 ParentData를 this로 설정합니다. */
	void AddChildListData(UListDataObject_Base* InChildListData);

	/** 보유한 자식 데이터 목록을 반환합니다. */
	virtual TArray<UListDataObject_Base*> GetAllChildListData() const override;

	/** 자식 데이터 보유 여부를 반환합니다. ChildListDataArray가 비어 있지 않으면 true를 반환합니다. */
	virtual bool HasAnyChildListData() const override;

private:
	UPROPERTY(Transient)
	TArray<UListDataObject_Base*> ChildListDataArray; // 자식 항목 배열 (GC 추적, 직렬화 제외)
};