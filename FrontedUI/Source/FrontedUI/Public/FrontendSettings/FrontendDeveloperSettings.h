// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "FrontendDeveloperSettings.generated.h"

class UWidget_ActivatableBase;
/**
 * 
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Frontend UI Settings"))
class FRONTEDUI_API UFrontendDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Widget Reference", meta = (ForceInlineRow, Categories = "Frontend.Widget"))
	TMap<FGameplayTag, TSoftClassPtr<UWidget_ActivatableBase>> FrontendWidgetMap;

	UPROPERTY(Config, EditAnywhere, Category = "Widget Reference")
	TSoftClassPtr<UUserWidget> HudNotificationWidgetClass;

	UPROPERTY(Config, EditAnywhere, Category = "Options Image Reference", meta = (ForceInlineRow, Categories = "Frontend.Image"))
	TMap<FGameplayTag, TSoftObjectPtr<UTexture2D>> OptionsScreenSoftImageMap;

	UPROPERTY(Config, EditAnywhere, Category = "Audio", meta = ( AllowedClasses = "/Script/Engine.SoundClass" ))
	FSoftObjectPath MasterSoundClass;

	UPROPERTY(Config, EditAnywhere, Category = "Audio", meta = ( AllowedClasses = "/Script/Engine.SoundClass" ))
	FSoftObjectPath MusicSoundClass;

	UPROPERTY(Config, EditAnywhere, Category = "Audio", meta = ( AllowedClasses = "/Script/Engine.SoundClass" ))
	FSoftObjectPath SoundFXSoundClass;

	UPROPERTY(Config, EditAnywhere, Category = "Audio", meta = ( AllowedClasses = "/Script/Engine.SoundMix" ))
	FSoftObjectPath DefaultSoundMix;

	// 새 게임 시작 시 전환할 메인 게임 레벨
	UPROPERTY(Config, EditAnywhere, Category = "Level")
	TSoftObjectPtr<UWorld> MainGameLevel;

	// 타이틀로 돌아가기 시 전환할 프론트엔드 레벨
	UPROPERTY(Config, EditAnywhere, Category = "Level")
	TSoftObjectPtr<UWorld> FrontendLevel;
};