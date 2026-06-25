#pragma once

#include "FrontendStructTypes.generated.h"

USTRUCT()
struct FOptionsDataEditConditionDescriptor
{
	GENERATED_BODY()

	void SetEditConditionFunction(const TFunction<bool()>& InEditConditionFunction)
	{
		EditConditionFunction = InEditConditionFunction;
	}

	bool IsValid() const
	{
		return EditConditionFunction != nullptr;
	}

	bool IsEditConditionMet() const
	{
		if (IsValid())
		{
			return EditConditionFunction();
		}

		return true;
	}

	FString GetDisabledRichReason() const
	{
		return DisabledRichReason;
	}

	void SetDisabledRichReason(const FString& InRichReason)
	{
		DisabledRichReason = InRichReason;
	}

	bool HasForcedStringValue() const
	{
		return DisabledForcedStringValue.IsSet();
	}

	FString GetDisabledForcedStringValue() const
	{
		return DisabledForcedStringValue.GetValue();
	}

	void SetDisabledForcedStringValue(const FString& InForcedValue)
	{
		DisabledForcedStringValue = InForcedValue;
	}

private:
	TFunction<bool()> EditConditionFunction;
	FString DisabledRichReason;
	TOptional<FString> DisabledForcedStringValue;
};