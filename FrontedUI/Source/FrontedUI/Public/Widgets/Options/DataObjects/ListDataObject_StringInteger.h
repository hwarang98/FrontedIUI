// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "ListDataObject_String.h"
#include "ListDataObject_StringInteger.generated.h"

/**
 * 
 */
UCLASS()
class FRONTEDUI_API UListDataObject_StringInteger : public UListDataObject_String
{
	GENERATED_BODY()

public:
	void AddIntegerOption(int32 InIntegerValue, const FText& InDisplayName);

protected:
	virtual void OnDataObjectInitialized() override;
	virtual void OnEditDependencyDataModified(UListDataObject_Base* ModifiedDependencyData, EOptionsListDataModifyReason ModifiedReason) override;
};