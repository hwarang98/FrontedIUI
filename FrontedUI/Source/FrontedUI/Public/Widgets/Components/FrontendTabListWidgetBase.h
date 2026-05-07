// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonTabListWidgetBase.h"
#include "FrontendTabListWidgetBase.generated.h"

class UFrontendCommonButtonBase;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class FRONTEDUI_API UFrontendTabListWidgetBase : public UCommonTabListWidgetBase
{
	GENERATED_BODY()

public:
	void RequestRegisterTab(const FName& InTabID, const FText& InTabDisplayName);

private:
	#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;
	#endif

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontend Tab List Setting", meta=(AllowPrivateAccess = "true", ClampMin = "1", ClampMax = "10"))
	int32 DebugEditorTabCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontend Tab List Setting", meta=(AllowPrivateAccess = "true"))
	TSubclassOf<UFrontendCommonButtonBase> TabButtonEntryWidgetClass;
};