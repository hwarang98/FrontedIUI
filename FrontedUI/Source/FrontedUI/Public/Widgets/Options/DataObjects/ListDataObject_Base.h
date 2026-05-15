// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "FrontendTypes/FrontendEnum.h"
#include "UObject/Object.h"
#include "ListDataObject_Base.generated.h"

/** Get/Set 접근자 쌍을 자동 생성하는 매크로. DataType 반환형, PropertyName 기반으로 Getter/Setter를 인라인 정의합니다. */
#define LIST_DATA_ACCESSOR(DataType,PropertyName) \
	FORCEINLINE DataType Get##PropertyName() const { return PropertyName;} \
	void Set##PropertyName(DataType In##PropertyName) { PropertyName = In##PropertyName;}

/**
 * 리스트 UI 항목 데이터의 기본 클래스.
 * 항목 식별자(DataID), 표시 이름, 설명/비활성화 Rich Text, 설명 이미지, 부모 데이터 참조 등
 * 모든 리스트 항목에 공통으로 필요한 데이터를 관리합니다.
 * 탭처럼 자식 항목을 가지는 경우 UListDataObject_Collection을 상속하여 사용합니다.
 */
UCLASS(Abstract)
class FRONTEDUI_API UListDataObject_Base : public UObject
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnListDataModifiedDelegate, UListDataObject_Base*, EOptionsListDataModifyReason)
	FOnListDataModifiedDelegate OnListDataModified;

	LIST_DATA_ACCESSOR(FName, DataID)                                    // 항목 고유 식별자
	LIST_DATA_ACCESSOR(FText, DataDisplayName)                           // UI에 표시되는 항목 이름
	LIST_DATA_ACCESSOR(FText, DescriptionRichText)                       // 항목 설명 Rich Text
	LIST_DATA_ACCESSOR(FText, DisabledRichText)                          // 항목 비활성화 시 표시할 Rich Text
	LIST_DATA_ACCESSOR(TSoftObjectPtr<UTexture2D>, SoftDescriptionImage) // 설명 영역에 표시할 이미지 (소프트 레퍼런스)
	LIST_DATA_ACCESSOR(TObjectPtr<UListDataObject_Base>, ParentData)     // 계층 구조에서 부모 항목 데이터

	/** 데이터 오브젝트를 초기화합니다. 내부적으로 OnDataObjectInitialized를 호출하며, AddChildListData 시점에 자동 호출됩니다. */
	void InitDataObject();

	/**
	 * 이 항목이 보유한 자식 설정 데이터 목록을 반환합니다.
	 * 기본 구현은 빈 배열을 반환합니다.
	 * 자식 항목을 가지는 탭 계층 구조는 UListDataObject_Collection에서 오버라이드해야 합니다.
	 */
	virtual TArray<UListDataObject_Base*> GetAllChildListData() const { return TArray<UListDataObject_Base*>(); }

	/** 자식 리스트 데이터 보유 여부를 반환합니다. UListDataObject_Collection에서 true를 반환하도록 오버라이드합니다. */
	virtual bool HasAnyChildListData() const { return false; }

	void SetShouldApplySettingsImmediately(bool bShouldApplyRightAway) { bShouldApplyChangeImmediately = bShouldApplyRightAway; }

protected:
	/** 초기화 시 호출되는 훅. 기본 구현은 비어 있으며, 자식 클래스에서 필요한 초기화 로직을 오버라이드합니다. */
	virtual void OnDataObjectInitialized();

	virtual void NotifyListDataModified(UListDataObject_Base* ModifiedData, EOptionsListDataModifyReason ModifiedReason = EOptionsListDataModifyReason::DirectlyModified);

private:
	FName DataID;                                    // 항목 고유 식별자
	FText DataDisplayName;                           // UI 표시 이름
	FText DescriptionRichText;                       // 설명 Rich Text
	FText DisabledRichText;                          // 비활성화 시 표시할 Rich Text
	TSoftObjectPtr<UTexture2D> SoftDescriptionImage; // 설명 이미지 (소프트 레퍼런스)

	UPROPERTY(Transient)
	TObjectPtr<UListDataObject_Base> ParentData; // 부모 항목 참조 (GC 추적, 직렬화 제외)

	bool bShouldApplyChangeImmediately = false;

};