// LeeHwaRang All Rights Reserved


#include "Widgets/Widget_ActivatableBase.h"
#include "Controllers/FrontendPlayerController.h"

AFrontendPlayerController* UWidget_ActivatableBase::GetOwningFrontendPlayerController()
{
	if (!CachedFrontendPlayerController.IsValid())
	{
		CachedFrontendPlayerController = GetOwningPlayer<AFrontendPlayerController>();
	}

	return CachedFrontendPlayerController.IsValid() ? CachedFrontendPlayerController.Get() : nullptr;
}