// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/ListDataObject_Value.h"
#include "CommonNumericTextBlock.h"
#include "ListDataObject_Scalar.generated.h"

/**
 * 
 */
UCLASS()
class FRONTEDUI_API UListDataObject_Scalar : public UListDataObject_Value
{
	GENERATED_BODY()

public:
	LIST_DATA_ACCESSOR(TRange<float>, DisplayValueRange)
	LIST_DATA_ACCESSOR(TRange<float>, OutputValueRange)
	LIST_DATA_ACCESSOR(float, SliderStepSize)
	LIST_DATA_ACCESSOR(ECommonNumericType, DisplayNumericType)
	LIST_DATA_ACCESSOR(FCommonNumberFormattingOptions, NumberFormattingOptions)

	static FCommonNumberFormattingOptions NoDecimals();
	static FCommonNumberFormattingOptions WithDecimals(int32 NumFractionalDigit);

	float GetCurrentValue() const;

	void SetCurrentValueFromSlider(float InNewValue);

private:
	#pragma region UListDataObject_Base Interface
	virtual bool CanResetBackToDefaultValue() const override;
	virtual bool TryResetBackToDefaultValue() override;
	virtual void OnEditDependencyDataModified(UListDataObject_Base* ModifiedDependencyData, EOptionsListDataModifyReason ModifiedReason) override;
	#pragma endregion

	float StringToFloat(const FString& InString) const;

	TRange<float> DisplayValueRange = TRange<float>(0.0f, 1.0f);
	TRange<float> OutputValueRange = TRange<float>(0.0f, 1.0f);
	float SliderStepSize = 0.1f;
	ECommonNumericType DisplayNumericType = ECommonNumericType::Number;
	FCommonNumberFormattingOptions NumberFormattingOptions;
};