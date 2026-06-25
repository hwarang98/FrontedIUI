// LeeHwaRang All Rights Reserved


#include "Widgets/Widget_ActivatableBase.h"
#include "Controllers/FrontendPlayerController.h"

TOptional<FUIInputConfig> UWidget_ActivatableBase::GetDesiredInputConfig() const
{
	if (bOverrideInputConfig)
	{
		return FUIInputConfig(InputModeOverride, MouseCaptureModeOverride);
	}

	return TOptional<FUIInputConfig>();
}

AFrontendPlayerController* UWidget_ActivatableBase::GetOwningFrontendPlayerController()
{
	if (!CachedFrontendPlayerController.IsValid())
	{
		CachedFrontendPlayerController = GetOwningPlayer<AFrontendPlayerController>();
	}

	return CachedFrontendPlayerController.IsValid() ? CachedFrontendPlayerController.Get() : nullptr;
}