// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "ListDataObject_String.h"
#include "ListDataObject_StringEnum.generated.h"

/**
 * 
 */
UCLASS()
class FRONTEDUI_API UListDataObject_StringEnum : public UListDataObject_String
{
	GENERATED_BODY()

public:
	template <typename EnumType>
	void AddEnumOption(EnumType InEnumOption, const FText& InDisplayName)
	{
		const UEnum* StaticEnumOption = StaticEnum<EnumType>();
		const FString ConvertedEnumString = StaticEnumOption->GetNameStringByValue(InEnumOption);

		AddDynamicOption(ConvertedEnumString, InDisplayName);
	}

	template <typename EnumType>
	EnumType GetCurrentValueAsEnum() const
	{
		const UEnum* StaticEnumOption = StaticEnum<EnumType>();

		return static_cast<EnumType>(StaticEnumOption->GetValueByNameString(CurrentStringValue));
	}

	template <typename EnumType>
	void SetDefaultValueFromEnumOption(EnumType InEnumOption)
	{
		const UEnum* StaticEnumOption = StaticEnum<EnumType>();
		const FString ConvertedEnumString = StaticEnumOption->GetNameStringByValue(InEnumOption);

		SetDefaultValueFromString(ConvertedEnumString);
	}
};