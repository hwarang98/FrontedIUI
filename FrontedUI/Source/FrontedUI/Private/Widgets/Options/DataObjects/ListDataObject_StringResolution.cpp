// LeeHwaRang All Rights Reserved


#include "Widgets/Options/DataObjects/ListDataObject_StringResolution.h"
#include "FrontendDebugHelper.h"
#include "FrontendSettings/FrontendGameUserSettings.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Widgets/Options/OptionsDataInteractionHelper.h"

void UListDataObject_StringResolution::InitResolutionValues()
{
	TArray<FIntPoint> AvailableResolutions;

	UKismetSystemLibrary::GetSupportedFullscreenResolutions(AvailableResolutions);

	AvailableResolutions.Sort(
		[](const FIntPoint& A, const FIntPoint& B)-> bool {
			return A.SizeSquared() < B.SizeSquared();
		}
		);

	for (const FIntPoint& Resolution : AvailableResolutions)
	{
		AddDynamicOption(ResToValueString(Resolution), ResToDisplayText(Resolution));
	}

	MaximumAllowedResolution = ResToValueString(AvailableResolutions.Last());

	SetDefaultValueFromString(MaximumAllowedResolution);
}

void UListDataObject_StringResolution::OnDataObjectInitialized()
{
	Super::OnDataObjectInitialized();

	if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
	{
		CurrentDisplayText = ResToDisplayText(UFrontendGameUserSettings::Get()->GetScreenResolution());

	}

}

FString UListDataObject_StringResolution::ResToValueString(const FIntPoint& InResolution) const
{
	// DataDynamicGetter에서 가져온 해상도: (X=2560,Y=1440)
	return FString::Printf(TEXT("(X=%i, Y=%i)"), InResolution.X, InResolution.Y);
}

FText UListDataObject_StringResolution::ResToDisplayText(const FIntPoint& InResolution) const
{
	const FString DisplayString = FString::Printf(TEXT("%i x %i"), InResolution.X, InResolution.Y);

	return FText::FromString(DisplayString);
}