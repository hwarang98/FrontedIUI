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

	USoundClass* MusicSoundClass = nullptr;
	if (UObject* LoadedObject = FrontendDeveloperSettings->MusicSoundClass.TryLoad())
	{
		MusicSoundClass = CastChecked<USoundClass>(LoadedObject);
	}

	USoundMix* DefaultSoundMix = nullptr;
	if (UObject* LoadedObject = FrontendDeveloperSettings->DefaultSoundMix.TryLoad())
	{
		DefaultSoundMix = CastChecked<USoundMix>(LoadedObject);
	}

	MusicVolume = InVolume;

	UGameplayStatics::SetSoundMixClassOverride(
		InAudioWorld,
		DefaultSoundMix,
		MusicSoundClass,
		MusicVolume,
		1.f,
		0.2f
		);

	UGameplayStatics::PushSoundMixModifier(InAudioWorld, DefaultSoundMix);
}

void UFrontendGameUserSettings::SetSoundFXVolume(float InVolume)
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

	USoundClass* SoundFXSoundClass = nullptr;
	if (UObject* LoadedObject = FrontendDeveloperSettings->SoundFXSoundClass.TryLoad())
	{
		SoundFXSoundClass = CastChecked<USoundClass>(LoadedObject);
	}

	USoundMix* DefaultSoundMix = nullptr;
	if (UObject* LoadedObject = FrontendDeveloperSettings->DefaultSoundMix.TryLoad())
	{
		DefaultSoundMix = CastChecked<USoundMix>(LoadedObject);
	}

	SoundFXVolume = InVolume;

	UGameplayStatics::SetSoundMixClassOverride(
		InAudioWorld,
		DefaultSoundMix,
		SoundFXSoundClass,
		SoundFXVolume,
		1.f,
		0.2f
		);

	UGameplayStatics::PushSoundMixModifier(InAudioWorld, DefaultSoundMix);
}

void UFrontendGameUserSettings::SetAllowBackGroundAudio(bool IsAllowed)
{
	bAllowBackGroundAudio = IsAllowed;
}

void UFrontendGameUserSettings::SetUseHDRAudioMode(bool IsAllowed)
{
	bUseHDRAudioMode = IsAllowed;
}

float UFrontendGameUserSettings::GetCurrentDisplayGamma() const
{
	if (GEngine)
	{
		return GEngine->GetDisplayGamma();
	}

	return 0.0f;
}

void UFrontendGameUserSettings::SetCurrentDisplayGamma(float InNewGamma)
{
	if (GEngine)
	{
		GEngine->DisplayGamma = InNewGamma;
	}

}