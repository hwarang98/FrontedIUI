// LeeHwaRang All Rights Reserved


#include "FrontendSettings/FrontendGameUserSettings.h"

#include "FrontendSettings/FrontendDeveloperSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"

UFrontendGameUserSettings::UFrontendGameUserSettings()
	: OverallVolume(1.f)
	, MusicVolume(1.f)
	, SoundFXVolume(1.f)
	, bAllowBackGroundAudio(false)
	, bUseHDRAudioMode(false) {}

UFrontendGameUserSettings* UFrontendGameUserSettings::Get()
{
	if (GEngine)
	{
		return CastChecked<UFrontendGameUserSettings>(GEngine->GetGameUserSettings());
	}

	return nullptr;
}

void UFrontendGameUserSettings::SetOverallVolume(float InVolume)
{
	UWorld* InAudioWorld = nullptr;
	const UFrontendDeveloperSettings* FrontendDeveloperSettings = GetDefault<UFrontendDeveloperSettings>();

	if (GEngine)
	{
		InAudioWorld = GEngine->GetCurrentPlayWorld();
	}

	if (!InAudioWorld || !FrontendDeveloperSettings)
	{
		return;
	}

	USoundClass* MasterSoundClass = nullptr;
	if (UObject* LoadedObject = FrontendDeveloperSettings->MasterSoundClass.TryLoad())
	{
		MasterSoundClass = CastChecked<USoundClass>(LoadedObject);
	}

	USoundMix* DefaultSoundMix = nullptr;
	if (UObject* LoadedObject = FrontendDeveloperSettings->DefaultSoundMix.TryLoad())
	{
		DefaultSoundMix = CastChecked<USoundMix>(LoadedObject);
	}

	OverallVolume = InVolume;

	UGameplayStatics::SetSoundMixClassOverride(
		InAudioWorld,
		DefaultSoundMix,
		MasterSoundClass,
		OverallVolume,
		1.f,
		0.2f
		);

	UGameplayStatics::PushSoundMixModifier(InAudioWorld, DefaultSoundMix);
}

void UFrontendGameUserSettings::SetMusicVolume(float InVolume)
{
	MusicVolume = InVolume;
}

void UFrontendGameUserSettings::SetSoundFXVolume(float InVolume)
{
	SoundFXVolume = InVolume;
}

void UFrontendGameUserSettings::SetAllowBackGroundAudio(bool IsAllowed)
{
	bAllowBackGroundAudio = IsAllowed;
}

void UFrontendGameUserSettings::SetUseHDRAudioMode(bool IsAllowed)
{
	bUseHDRAudioMode = IsAllowed;
}