// LeeHwaRang All Rights Reserved

#include "Floor404/Floor404GameMode.h"
#include "Floor404/Floor404GameState.h"
#include "Camera/PlayerCameraManager.h"
#include "FrontendFunctionLibrary.h"
#include "FrontendGameplayTags.h"
#include "GameFramework/PlayerController.h"
#include "Subsystems/FrontendUISubsystem.h"
#include "Widgets/Widget_ClearScreen.h"
#include "Widgets/Widget_GameOverScreen.h"

AFloor404GameMode::AFloor404GameMode()
{
	GameStateClass = AFloor404GameState::StaticClass();
}

void AFloor404GameMode::HandleGameClear()
{
	AFloor404GameState* Floor404GameState = GetWorld()->GetGameState<AFloor404GameState>();
	if (!Floor404GameState)
	{
		return;
	}

	Floor404GameState->HandleGameClear();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
	{
		return;
	}

	PlayerController->DisableInput(PlayerController);

	if (APlayerCameraManager* PlayerCameraManager = PlayerController->PlayerCameraManager)
	{
		PlayerCameraManager->StartCameraFade(0.f, 1.f, FadeOutDuration, FLinearColor::Black, false, true);
	}

	GetWorldTimerManager().SetTimer(ClearUITimerHandle, this, &AFloor404GameMode::OnFadeOutComplete, FadeOutDuration, false);
}

void AFloor404GameMode::HandleGameOver()
{
	AFloor404GameState* Floor404GameState = GetWorld()->GetGameState<AFloor404GameState>();
	if (!Floor404GameState)
	{
		return;
	}

	Floor404GameState->HandleGameOver();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
	{
		return;
	}

	PlayerController->DisableInput(PlayerController);

	if (APlayerCameraManager* PlayerCameraManager = PlayerController->PlayerCameraManager)
	{
		PlayerCameraManager->StartCameraFade(0.f, 1.f, GameOverFadeOutDuration, FLinearColor::Black, false, true);
	}

	GetWorldTimerManager().SetTimer(GameOverUITimerHandle, this, &AFloor404GameMode::OnGameOverFadeOutComplete, GameOverFadeOutDuration, false);
}

void AFloor404GameMode::OnFadeOutComplete()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
	{
		return;
	}

	PlayerController->EnableInput(PlayerController);
	PlayerController->bShowMouseCursor = true;
	PlayerController->SetInputMode(FInputModeUIOnly());

	AFloor404GameState* Floor404GameState = GetWorld()->GetGameState<AFloor404GameState>();

	UFrontendUISubsystem::Get(this)->PushSoftWidgetToStackAsync(
		FrontendGameplayTags::Frontend_WidgetStack_GameMenu,
		UFrontendFunctionLibrary::GetFrontendSoftWidgetClassByTag(FrontendGameplayTags::Frontend_Widget_ClearScreen),
		[Floor404GameState](EAsyncPushWidgetState State, UWidget_ActivatableBase* Widget)
		{
			if (State == EAsyncPushWidgetState::OnCreatedBeforePush)
			{
				CastChecked<UWidget_ClearScreen>(Widget)->InitClearScreen(
					Floor404GameState->GetElapsedSeconds(),
					Floor404GameState->GetTotalCorrectReports()
				);
			}
		}
	);
}

void AFloor404GameMode::OnGameOverFadeOutComplete()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
	{
		return;
	}

	PlayerController->EnableInput(PlayerController);
	PlayerController->bShowMouseCursor = true;
	PlayerController->SetInputMode(FInputModeUIOnly());

	UFrontendUISubsystem::Get(this)->PushSoftWidgetToStackAsync(
		FrontendGameplayTags::Frontend_WidgetStack_GameMenu,
		UFrontendFunctionLibrary::GetFrontendSoftWidgetClassByTag(FrontendGameplayTags::Frontend_Widget_GameOverScreen),
		[](EAsyncPushWidgetState, UWidget_ActivatableBase*) {}
	);
}