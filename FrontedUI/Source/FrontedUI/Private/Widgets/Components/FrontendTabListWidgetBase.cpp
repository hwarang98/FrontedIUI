// LeeHwaRang All Rights Reserved


#include "Widgets/Components/FrontendTabListWidgetBase.h"
#include "Widgets/Components/FrontendCommonButtonBase.h"
#include "Editor/WidgetCompilerLog.h"

void UFrontendTabListWidgetBase::RequestRegisterTab(const FName& InTabID, const FText& InTabDisplayName)
{
	RegisterTab(InTabID, TabButtonEntryWidgetClass, nullptr);

	if (UFrontendCommonButtonBase* FoundButton = Cast<UFrontendCommonButtonBase>(GetTabButtonBaseByID(InTabID)))
	{
		FoundButton->SetButtonText(InTabDisplayName);
	}
}

#if WITH_EDITOR
void UFrontendTabListWidgetBase::ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	if (!TabButtonEntryWidgetClass)
	{
		CompileLog.Error(
			FText::FromString(
				TEXT("변수 TabButtonEntryWidgetClass에 유효한 항목이 지정되어 있지 않습니다. ") +
				GetClass()->GetName() +
				TEXT(" 정상적으로 작동하려면 유효한 Entry Widget Class가 필요합니다.")
				));
	}
}
#endif