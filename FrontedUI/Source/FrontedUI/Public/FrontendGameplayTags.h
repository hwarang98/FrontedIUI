// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

namespace FrontendGameplayTags
{
	#pragma region Frontend widget stack
	FRONTEDUI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_WidgetStack_Modal);
	FRONTEDUI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_WidgetStack_GameMenu);
	FRONTEDUI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_WidgetStack_GameHud);
	FRONTEDUI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_WidgetStack_Frontend);
	#pragma endregion

	#pragma region Frontend widget
	FRONTEDUI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_Widget_PressAnyKeyScreen);
	FRONTEDUI_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_Widget_MainMenuScreen);
	#pragma endregion
}