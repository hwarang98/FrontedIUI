// LeeHwaRang All Rights Reserved


#include "Subsystems/FrontendLoadingScreenSubsystem.h"
#include "PreLoadScreenManager.h"
#include "Blueprint/UserWidget.h"
#include "FrontendSettings/FrontendLoadingScreenSettings.h"
#include "interfaces/FrontendLoadingScreenInterface.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Pawn.h"

bool UFrontendLoadingScreenSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> FoundClasses;
		GetDerivedClasses(GetClass(), FoundClasses);

		return FoundClasses.IsEmpty();
	}

	return false;
}

void UFrontendLoadingScreenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	FCoreUObjectDelegates::PreLoadMapWithContext.AddUObject(this, &ThisClass::OnMapPreLoaded);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::OnMapPostLoaded);
}

void UFrontendLoadingScreenSubsystem::Deinitialize()
{
	FCoreUObjectDelegates::PreLoadMapWithContext.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
}

UWorld* UFrontendLoadingScreenSubsystem::GetTickableGameObjectWorld() const
{
	if (const UGameInstance* OwningGameInstance = GetGameInstance())
	{
		return OwningGameInstance->GetWorld();
	}

	return nullptr;
}

void UFrontendLoadingScreenSubsystem::Tick(float DeltaTime)
{
	TryUpdateLoadingScreen();
}

ETickableTickType UFrontendLoadingScreenSubsystem::GetTickableTickType() const
{
	if (IsTemplate())
	{
		return ETickableTickType::Never;
	}

	return ETickableTickType::Conditional;
}

bool UFrontendLoadingScreenSubsystem::IsTickable() const
{
	return GetGameInstance() && GetGameInstance()->GetGameViewportClient();
}

TStatId UFrontendLoadingScreenSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UFrontendLoadingScreenSubsystem, STATGROUP_Tickables);
}

void UFrontendLoadingScreenSubsystem::OnMapPreLoaded(const FWorldContext& WorldContext, const FString& MapName)
{
	if (WorldContext.OwningGameInstance != GetGameInstance())
	{
		return;
	}

	SetTickableTickType(ETickableTickType::Conditional);

	bIsCurrentlyLoadingMap = true;

	TryUpdateLoadingScreen();
}

void UFrontendLoadingScreenSubsystem::OnMapPostLoaded(UWorld* LoadedWorld)
{
	// 로딩이 완료 되었을때
	if (LoadedWorld && LoadedWorld->GetGameInstance() == GetGameInstance())
	{
		bIsCurrentlyLoadingMap = false;
	}
}

void UFrontendLoadingScreenSubsystem::TryUpdateLoadingScreen()
{
	// 현재 활성화되어 있는 시작 로딩 화면이 있는지 확인
	if (IsPreLoadScreenActive())
	{
		return;
	}

	// 로딩 화면을 표시해야 하는지 확인
	if (ShouldShowLoadingScreen())
	{
		TryDisplayLoadingScreenIfNone();

		OnLoadingReasonUpdated.Broadcast(CurrentLoadingReason);
	}
	else
	{
		// 현재 활성화된 로딩 화면 제거 시도
		TryRemoveLoadingScreen();

		HoldLoadingScreenStartUpTime = -1.f;

		// 로딩 완료 알림
		NotifyLoadingScreenVisibilityChanged(false);

		// Tick 비활성화
		SetTickableTickType(ETickableTickType::Never);
	}
}

void UFrontendLoadingScreenSubsystem::TryDisplayLoadingScreenIfNone()
{
	//If there's already an active loading screen, return early if yes
	if (CachedCreatedLoadingScreenWidget)
	{
		return;
	}

	const UFrontendLoadingScreenSettings* LoadingScreenSettings = GetDefault<UFrontendLoadingScreenSettings>();

	TSubclassOf<UUserWidget> LoadedWidgetClass = LoadingScreenSettings->GetLoadingScreenWidgetClassChecked();

	UUserWidget* CreatedWidget = UUserWidget::CreateWidgetInstance(*GetGameInstance(), LoadedWidgetClass, NAME_None);

	check(CreatedWidget);

	CachedCreatedLoadingScreenWidget = CreatedWidget->TakeWidget();

	GetGameInstance()->GetGameViewportClient()->AddViewportWidgetContent(CachedCreatedLoadingScreenWidget.ToSharedRef(), 1000);

	NotifyLoadingScreenVisibilityChanged(true);
}

void UFrontendLoadingScreenSubsystem::TryRemoveLoadingScreen()
{
	if (!CachedCreatedLoadingScreenWidget)
	{
		return;
	}

	GetGameInstance()->GetGameViewportClient()->RemoveViewportWidgetContent(CachedCreatedLoadingScreenWidget.ToSharedRef());

	CachedCreatedLoadingScreenWidget.Reset();
}

void UFrontendLoadingScreenSubsystem::NotifyLoadingScreenVisibilityChanged(bool bIsVisible)
{
	for (const ULocalPlayer* ExistingLocalPlayer : GetGameInstance()->GetLocalPlayers())
	{
		if (!ExistingLocalPlayer)
		{
			continue;
		}

		if (APlayerController* PlayerController = ExistingLocalPlayer->GetPlayerController(GetGameInstance()->GetWorld()))
		{
			// 플레이어 컨트롤러가 인터페이스를 구현하고 있는지 확인합니다. 구현되어 있다면 인터페이스를 통해 로딩 상태를 전달합니다.
			if (PlayerController->Implements<UFrontendLoadingScreenInterface>())
			{
				if (bIsVisible)
				{
					IFrontendLoadingScreenInterface::Execute_OnLoadingScreenActivated(PlayerController);
				}
				else
				{
					IFrontendLoadingScreenInterface::Execute_OnLoadingScreenDeactivated(PlayerController);
				}
			}

			if (APawn* OwningPawn = PlayerController->GetPawn())
			{
				if (OwningPawn->Implements<UFrontendLoadingScreenInterface>())
				{
					if (bIsVisible)
					{
						IFrontendLoadingScreenInterface::Execute_OnLoadingScreenActivated(OwningPawn);
					}
					else
					{
						IFrontendLoadingScreenInterface::Execute_OnLoadingScreenDeactivated(OwningPawn);
					}
				}
			}
		}

		// 월드 내 다른 오브젝트들에게 로딩 상태를 알리는 코드를 여기에 작성합니다.
	}
}

bool UFrontendLoadingScreenSubsystem::IsPreLoadScreenActive() const
{
	if (const FPreLoadScreenManager* PreLoadScreenManager = FPreLoadScreenManager::Get())
	{
		return PreLoadScreenManager->HasValidActivePreLoadScreen();
	}

	return false;
}

bool UFrontendLoadingScreenSubsystem::ShouldShowLoadingScreen()
{
	const UFrontendLoadingScreenSettings* LoadingScreenSettings = GetDefault<UFrontendLoadingScreenSettings>();

	if (GIsEditor && !LoadingScreenSettings->bShouldShowLoadingScreenInEditor)
	{
		return false;
	}

	// 월드 내 오브젝트들이 로딩 화면을 필요로 하는지 확인
	if (CheckTheNeedToShowLoadingScreen())
	{
		GetGameInstance()->GetGameViewportClient()->bDisableWorldRendering = true;

		return true;
	}

	CurrentLoadingReason = TEXT("병원 내부를 정리하는 중입니다");

	// 로딩 화면을 표시할 필요가 없습니다. 이제 월드가 뷰포트에 렌더링될 수 있도록 허용합니다.
	GetGameInstance()->GetGameViewportClient()->bDisableWorldRendering = false;

	const float CurrentTime = FPlatformTime::Seconds();

	if (HoldLoadingScreenStartUpTime < 0.f)
	{
		HoldLoadingScreenStartUpTime = CurrentTime;
	}

	const float ElapsedTime = CurrentTime - HoldLoadingScreenStartUpTime;

	if (ElapsedTime < LoadingScreenSettings->HoldLoadingScreenExtraSeconds)
	{
		return true;
	}

	return false;
}

bool UFrontendLoadingScreenSubsystem::CheckTheNeedToShowLoadingScreen()
{
	if (bIsCurrentlyLoadingMap)
	{
		CurrentLoadingReason = TEXT("병원으로 들어가는 중");

		return true;
	}

	const UWorld* OwningWorld = GetGameInstance()->GetWorld();
	if (!OwningWorld)
	{
		CurrentLoadingReason = TEXT("내부를 준비하는 중");

		return true;
	}

	if (!OwningWorld->HasBegunPlay())
	{
		CurrentLoadingReason = TEXT("입장 준비 중");

		return true;
	}

	if (!OwningWorld->GetFirstLocalPlayerFromController())
	{
		CurrentLoadingReason = TEXT("방문 절차를 확인하는 중");

		return true;
	}

	// GameState, PlayerState, PlayerCharacter와 관련 ActorComponent들의 초기화가 완료되었는지 확인
	if (!OwningWorld->GetGameState())
	{
		CurrentLoadingReason = TEXT("기록을 불러오는 중");
		return true;
	}

	const APlayerController* PlayerController = OwningWorld->GetFirstPlayerController();
	if (!PlayerController || !PlayerController->PlayerState)
	{
		CurrentLoadingReason = TEXT("신원 정보를 확인하는 중");
		return true;
	}

	const APawn* Pawn = PlayerController->GetPawn();
	if (!Pawn)
	{
		CurrentLoadingReason = TEXT("복도로 안내 하는중");
		return true;
	}

	for (const UActorComponent* Component : Pawn->GetComponents())
	{
		if (Component && !Component->HasBegunPlay())
		{
			CurrentLoadingReason = TEXT("환경을 준비하는 중");
			return true;
		}
	}

	return false;
}