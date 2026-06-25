// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/ListDataObject_Base.h"
#include "CommonInputTypeEnum.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "ListDataObject_KeyRemap.generated.h"

class UEnhancedInputUserSettings;
class UEnhancedPlayerMappableKeyProfile;
/**
 * 
 */
UCLASS()
class FRONTEDUI_API UListDataObject_KeyRemap : public UListDataObject_Base
{
	GENERATED_BODY()

public:
	void InitKeyRemapData(UEnhancedInputUserSettings* InOwningInputUserSettings, UEnhancedPlayerMappableKeyProfile* InKeyProfile, ECommonInputType InDesiredInputKeyType, const FPlayerKeyMapping& InOwningPlayerKeyMapping);

	FSlateBrush GetIconFromCurrentKey() const;

	FORCEINLINE ECommonInputType GetDesiredInputKeyType() const { return CachedDesiredInputKeyType; }

	void BindNewInputKey(const FKey& InNewKey);

	#pragma region UListDataObject_Base Interface
	virtual bool HasDefaultValue() const override;
	virtual bool CanResetBackToDefaultValue() const override;
	virtual bool TryResetBackToDefaultValue() override;
	#pragma endregion

private:
	FPlayerKeyMapping* GetOwningKeyMapping() const;

	UPROPERTY(Transient)
	TObjectPtr<UEnhancedInputUserSettings> CachedOwningInputUserSettings;

	UPROPERTY(Transient)
	TObjectPtr<UEnhancedPlayerMappableKeyProfile> CachedOwningKeyProfile;

	ECommonInputType CachedDesiredInputKeyType;

	FName CachedOwningMappingName;

	EPlayerMappableKeySlot CachedOwningMappableKeySlot;
};