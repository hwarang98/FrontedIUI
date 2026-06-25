// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"
#include "ListDataObject_StringResolution.generated.h"

/**
 * 
 */
UCLASS()
class FRONTEDUI_API UListDataObject_StringResolution : public UListDataObject_String
{
	GENERATED_BODY()

public:
	void InitResolutionValues();
	FORCEINLINE FString GetMaximumAllowedResolution() const { return MaximumAllowedResolution; }

protected:
	virtual void OnDataObjectInitialized() override;

private:
	FString ResToValueString(const FIntPoint& InResolution) const;
	FText ResToDisplayText(const FIntPoint& InResolution) const;

	FString MaximumAllowedResolution;
};