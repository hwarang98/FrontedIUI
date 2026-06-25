// LeeHwaRang All Rights Reserved
// GameStateClass를 AFloor404GameState로 지정하는 GameMode

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Floor404GameMode.generated.h"

UCLASS()
class FRONTEDUI_API AFloor404GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFloor404GameMode();

	/**
	 * @brief EscapeTrigger가 호출한다.
	 *        입력 차단 → Fade Out → Clear UI 표시 순서로 클리어 시퀀스를 처리한다.
	 */
	void HandleGameClear();

	/**
	 * @brief BP_Enemy가 호출한다.
	 *        입력 차단 → Fade Out → GameOver UI 표시 순서로 게임 오버 시퀀스를 처리한다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Floor404|GameOver")
	void HandleGameOver();

private:
	void OnFadeOutComplete();
	void OnGameOverFadeOutComplete();

	/** Fade Out 지속 시간 (초). */
	UPROPERTY(EditDefaultsOnly, Category = "Floor404|Clear")
	float FadeOutDuration = 2.f;

	/** 게임 오버 Fade Out 지속 시간 (초). */
	UPROPERTY(EditDefaultsOnly, Category = "Floor404|GameOver")
	float GameOverFadeOutDuration = 2.f;

	FTimerHandle ClearUITimerHandle;
	FTimerHandle GameOverUITimerHandle;
};
