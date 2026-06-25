// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "FrontendGameUserSettings.generated.h"

/**
 * 
 */
UCLASS()
class FRONTEDUI_API UFrontendGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UFrontendGameUserSettings();

	static UFrontendGameUserSettings* Get();

	#pragma region Gameplay Collection Tab
	UFUNCTION()
	FString GetCurrentGameDifficulty() const { return CurrentGameDifficulty; }

	UFUNCTION()
	void SetCurrentGameDifficulty(const FString& InNewDifficulty) { CurrentGameDifficulty = InNewDifficulty; }
	#pragma endregion

	#pragma region Audio Collection Tab
	UFUNCTION()
	float GetOverallVolume() const { return OverallVolume; }

	UFUNCTION()
	void SetOverallVolume(float InVolume);

	UFUNCTION()
	float GetMusicVolume() const { return MusicVolume; }

	UFUNCTION()
	void SetMusicVolume(float InVolume);

	UFUNCTION()
	float GetSoundFXVolume() const { return SoundFXVolume; }

	UFUNCTION()
	void SetSoundFXVolume(float InVolume);

	UFUNCTION()
	bool GetAllowBackGroundAudio() const { return bAllowBackGroundAudio; }

	UFUNCTION()
	void SetAllowBackGroundAudio(bool IsAllowed);

	UFUNCTION()
	bool GetUseHDRAudioMode() const { return bUseHDRAudioMode; }

	UFUNCTION()
	void SetUseHDRAudioMode(bool IsAllowed);
	#pragma endregion

	#pragma region Video Collection Tab
	UFUNCTION()
	float GetCurrentDisplayGamma() const;

	UFUNCTION()
	void SetCurrentDisplayGamma(float InNewGamma);

	#pragma endregion

private:
	#pragma region Gameplay Collection Tab
	UPROPERTY(Config)
	FString CurrentGameDifficulty;
	#pragma endregion

	#pragma region Audio Collection Tab
	UPROPERTY(Config)
	float OverallVolume;

	UPROPERTY(Config)
	float MusicVolume;

	UPROPERTY(Config)
	float SoundFXVolume;

	UPROPERTY(Config)
	bool bAllowBackGroundAudio;

	UPROPERTY(Config)
	bool bUseHDRAudioMode;
	#pragma endregion
};