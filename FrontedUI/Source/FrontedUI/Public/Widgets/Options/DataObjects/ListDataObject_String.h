// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "ListDataObject_Value.h"
#include "ListDataObject_String.generated.h"

/**
 * 문자열 기반의 선택형 옵션 데이터 오브젝트.
 * 문자열-텍스트 쌍으로 구성된 옵션 목록을 관리하며, 로테이터 위젯과 연동해 현재 선택값을 유지한다.
 */
UCLASS()
class FRONTEDUI_API UListDataObject_String : public UListDataObject_Value
{
	GENERATED_BODY()

public:
	/**
	 * @brief 런타임에 선택 가능한 옵션을 추가한다. StringArray와 TextArray는 동일한 인덱스로 대응된다.
	 *
	 * @param InStringValue 내부적으로 저장되는 문자열 값
	 * @param InDisplayText 화면에 표시되는 텍스트
	 */
	void AddDynamicOption(const FString& InStringValue, const FText& InDisplayText);

	// 다음 옵션으로 이동한다. 마지막 옵션이면 첫 번째로 순환한다.
	void AdvanceToNextOption();

	// 이전 옵션으로 이동한다. 첫 번째 옵션이면 마지막으로 순환한다.
	void BackToPreviousOption();

	/**
	 * @brief 로테이터 위젯이 선택 항목을 변경했을 때 호출되어 현재 값을 갱신하고 데이터 수정을 알린다.
	 *
	 * @param InNewSelectedText 로테이터에서 새로 선택된 항목의 표시 텍스트
	 */
	void OnRotatorInitiatedValueChange(const FText& InNewSelectedText);

	FORCEINLINE const TArray<FText>& GetAvailableOptionsTextArray() const { return AvailableOptionsTextArray; }
	FORCEINLINE FText GetCurrentDisplayText() const { return CurrentDisplayText; }

protected:
	// ~ Begin UListDataObject_Base Interface
	/**
	 * @brief 현재 값을 결정한다. 우선순위: 배열 첫 항목 → 기본값 → 동적 Getter 순으로 덮어쓴다.
	 */
	virtual void OnDataObjectInitialized() override;

	// 기본값이 존재하고 현재 값과 다를 때 true를 반환한다.
	virtual bool CanResetBackToDefaultValue() const override;

	/**
	 * @brief 현재 값을 기본값으로 되돌린다.
	 *
	 * @return DataDynamicSetter가 유효하고 실제로 값이 변경된 경우에만 true
	 */
	virtual bool TryResetBackToDefaultValue() override;
	// ~ End UListDataObject_Base Interface

	/**
	 * @brief 문자열 값에 대응하는 표시 텍스트를 찾아 CurrentDisplayText에 설정한다.
	 *
	 * @param InStringValue 대응할 문자열 값
	 * @return 대응하는 텍스트를 찾아 설정에 성공하면 true
	 */
	bool TrySetDisplayTextFromStringValue(const FString& InStringValue);

	FString CurrentStringValue;
	FText CurrentDisplayText;
	TArray<FString> AvailableOptionsStringArray; // TextArray와 동일한 인덱스로 대응
	TArray<FText> AvailableOptionsTextArray;
};