// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "ListDataObject_Base.h"
#include "ListDataObject_Value.generated.h"

class FOptionsDataInteractionHelper;

/**
 * 단일 값 선택형 리스트 항목 데이터의 기본 클래스.
 * 드롭다운, 슬라이더 등 하나의 값을 가지는 옵션 항목 타입에 사용됩니다.
 */
UCLASS(Abstract)
class FRONTEDUI_API UListDataObject_Value : public UListDataObject_Base
{
	GENERATED_BODY()

public:
	void SetDataDynamicGetter(const TSharedPtr<FOptionsDataInteractionHelper>& InDynamicGetter);
	void SetDataDynamicSetter(const TSharedPtr<FOptionsDataInteractionHelper>& InDynamicSetter);

	void SetDefaultValueFromString(const FString& InDefaultValue) { DefaultStringValue = InDefaultValue; }

	//~ Begin UListDataObject_Base Interface
	virtual bool HasDefaultValue() const override { return DefaultStringValue.IsSet(); }
	//~ End UListDataObject_Base Interface

protected:
	FString GetDefaultValueAsString() const { return DefaultStringValue.GetValue(); };

	TSharedPtr<FOptionsDataInteractionHelper> DataDynamicGetter;
	TSharedPtr<FOptionsDataInteractionHelper> DataDynamicSetter;

private:
	TOptional<FString> DefaultStringValue;
};