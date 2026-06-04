// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "ListDataObject_String.h"
#include "ListDataObject_StringBool.generated.h"

/**
 * 
 */
UCLASS()
class FRONTEDUI_API UListDataObject_StringBool : public UListDataObject_String
{
	GENERATED_BODY()

public:
	/**
	 * @brief true 항목의 표시 텍스트를 교체한다.
	 *
	 * @param InNewTrueDisplayText 새로 표시할 텍스트
	 * @note 옵션이 아직 배열에 추가되지 않은 경우에만 적용된다. TryInitBoolValues 호출 전에 사용해야 한다.
	 */
	void OverrideTrueDisplayText(const FText& InNewTrueDisplayText);

	/**
	 * @brief false 항목의 표시 텍스트를 교체한다.
	 *
	 * @param InNewFalseDisplayText 새로 표시할 텍스트
	 * @note 옵션이 아직 배열에 추가되지 않은 경우에만 적용된다. TryInitBoolValues 호출 전에 사용해야 한다.
	 */
	void OverrideFalseDisplayText(const FText& InNewFalseDisplayText);

	// "true"를 기본값으로 설정한다.
	void SetTrueAsDefaultValue();
	// "false"를 기본값으로 설정한다.
	void SetFalseAsDefaultValue();

protected:
	//~ Begin UListDataObject_String Interface
	/**
	 * @brief Bool 옵션을 초기화한 뒤 부모 초기화를 수행한다.
	 *
	 * @note TryInitBoolValues를 먼저 호출해야 Super에서 CurrentStringValue를 올바르게 설정할 수 있다.
	 */
	virtual void OnDataObjectInitialized() override;
	//~ End UListDataObject_String Interface

private:
	// true/false 옵션이 배열에 없을 경우 기본 "ON"/"OFF" 텍스트로 추가한다.
	void TryInitBoolValues();

	const FString TrueString = TEXT("true");
	const FString FalseString = TEXT("false");
};