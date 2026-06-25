// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "FrontendTypes/FrontendEnum.h"
#include "UObject/Object.h"
#include "FrontendTypes/FrontendStructTypes.h"
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
	FOnListDataModifiedDelegate OnDependencyDataModified;

	LIST_DATA_ACCESSOR(FName, DataID)                                    // 항목 고유 식별자
	LIST_DATA_ACCESSOR(FText, DataDisplayName)                           // UI에 표시되는 항목 이름
	LIST_DATA_ACCESSOR(FText, DescriptionRichText)                       // 항목 설명 Rich Text
	LIST_DATA_ACCESSOR(FText, DisabledRichText)                          // 항목 비활성화 시 표시할 Rich Text
	LIST_DATA_ACCESSOR(TSoftObjectPtr<UTexture2D>, SoftDescriptionImage) // 설명 영역에 표시할 이미지 (소프트 레퍼런스)
	LIST_DATA_ACCESSOR(TObjectPtr<UListDataObject_Base>, ParentData)     // 계층 구조에서 부모 항목 데이터

	/** 데이터 오브젝트를 초기화합니다. 내부적으로 OnDataObjectInitialized를 호출하며, AddChildListData 시점에 자동 호출됩니다. */
	void InitDataObject();

	/** 설정 값 변경 시 즉시 적용 여부를 설정합니다. */
	void SetShouldApplySettingsImmediately(bool bShouldApplyRightAway) { bShouldApplyChangeImmediately = bShouldApplyRightAway; }

	/**
	 * 이 항목이 보유한 자식 설정 데이터 목록을 반환합니다.
	 * 기본 구현은 빈 배열을 반환합니다.
	 * 자식 항목을 가지는 탭 계층 구조는 UListDataObject_Collection에서 오버라이드해야 합니다.
	 */
	virtual TArray<UListDataObject_Base*> GetAllChildListData() const { return TArray<UListDataObject_Base*>(); }

	/** 자식 리스트 데이터 보유 여부를 반환합니다. UListDataObject_Collection에서 true를 반환하도록 오버라이드합니다. */
	virtual bool HasAnyChildListData() const { return false; }

	// 자식 클래스에서 데이터를 초기화(reset)하는 구현을 제공하도록 해당 함수들을 오버라이드해야 합니다.
	virtual bool HasDefaultValue() const { return false; }
	virtual bool CanResetBackToDefaultValue() const { return false; }
	virtual bool TryResetBackToDefaultValue() { return false; }

	/**
	 * @brief OptionsDataRegistry에서 생성된 목록 데이터 객체의 편집 조건을 추가합니다.
	 *
	 * @param InEditCondition 추가할 편집 조건 디스크립터
	 */
	void AddEditCondition(const FOptionsDataEditConditionDescriptor& InEditCondition);

	void AddEditDependencyData(UListDataObject_Base* InDependencyData);

	/**
	 * @brief 등록된 모든 편집 조건을 평가하여 현재 편집 가능 여부를 반환합니다.
	 *
	 * @return 모든 조건이 충족되면 true, 하나라도 미충족이면 false
	 * @note 조건 미충족 시 DisabledRichText를 갱신하고, 강제 값(ForcedStringValue)이 있으면 현재 값에 적용합니다.
	 */
	bool IsDataCurrentlyEditable();

protected:
	/** 초기화 시 호출되는 훅. 기본 구현은 비어 있으며, 자식 클래스에서 필요한 초기화 로직을 오버라이드합니다. */
	virtual void OnDataObjectInitialized();

	/**
	 * @brief 데이터 변경을 OnListDataModified 델리게이트로 브로드캐스트합니다.
	 *
	 * @param ModifiedData 변경된 데이터 오브젝트
	 * @param ModifiedReason 변경 이유 (기본값: DirectlyModified)
	 * @note bShouldApplyChangeImmediately가 true이면 변경 즉시 게임 유저 세팅에 적용합니다.
	 */
	virtual void NotifyListDataModified(UListDataObject_Base* ModifiedData, EOptionsListDataModifyReason ModifiedReason = EOptionsListDataModifyReason::DirectlyModified);

	// 자식 클래스는 이 값을 강제 문자열 값으로 설정할 수 있도록 이 값을 재정의해야 합니다.
	virtual bool CanSetToForcedStringValue(const FString& InForcedValue) const { return false; }

	// 자아 클래스는 현재 값을 강제 값으로 설정하는 방법을 지정하려면 이를 재정의해야 합니다.
	virtual void OnSetToForcedStringValue(const FString& InForcedValue) {}

	//이 함수는 종속성 데이터의 값이 변경되면 호출됩니다. 자식 클래스는 필요한 사용자 지정 로직을 처리하기 위해 이 함수를 재정의할 수 있습니다. 슈퍼 호출이 예상됩니다.
	virtual void OnEditDependencyDataModified(UListDataObject_Base* ModifiedDependencyData, EOptionsListDataModifyReason ModifiedReason);

private:
	FName DataID;                                    // 항목 고유 식별자
	FText DataDisplayName;                           // UI 표시 이름
	FText DescriptionRichText;                       // 설명 Rich Text
	FText DisabledRichText;                          // 비활성화 시 표시할 Rich Text
	TSoftObjectPtr<UTexture2D> SoftDescriptionImage; // 설명 이미지 (소프트 레퍼런스)

	UPROPERTY(Transient)
	TObjectPtr<UListDataObject_Base> ParentData; // 부모 항목 참조 (GC 추적, 직렬화 제외)

	bool bShouldApplyChangeImmediately = false;

	UPROPERTY(Transient)
	TArray<FOptionsDataEditConditionDescriptor> EditConditionDescArray;
};