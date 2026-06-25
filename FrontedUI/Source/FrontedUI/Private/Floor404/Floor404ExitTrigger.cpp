// LeeHwaRang All Rights Reserved

#include "Floor404/Floor404ExitTrigger.h"

#include "FrontendDebugHelper.h"
#include "Floor404/Floor404AnomalyManager.h"
#include "Floor404/Floor404Character.h"
#include "Floor404/Floor404GameState.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/FrontendUISubsystem.h"

AFloor404ExitTrigger::AFloor404ExitTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;
	BoxComponent->SetBoxExtent(FVector(100.f, 100.f, 100.f));
	BoxComponent->SetCollisionProfileName(TEXT("Trigger"));
}

void AFloor404ExitTrigger::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundManagers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFloor404AnomalyManager::StaticClass(), FoundManagers);
	if (FoundManagers.Num() > 0)
	{
		AnomalyManager = Cast<AFloor404AnomalyManager>(FoundManagers[0]);
	}

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AFloor404ExitTrigger::OnBoxBeginOverlap);
}

void AFloor404ExitTrigger::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Cast<AFloor404Character>(OtherActor))
	{
		return;
	}

	const AFloor404GameState* GameState = GetWorld()->GetGameState<AFloor404GameState>();
	if (!GameState)
	{
		return;
	}

	if (bTeleportOnly)
	{
		TeleportPlayerToSpawnPoint();
		return;
	}

	if (!GameState->IsExitUnlocked())
	{
		if (UFrontendUISubsystem* UISubsystem = UFrontendUISubsystem::Get(this))
		{
			UISubsystem->ShowHudNotification(NSLOCTEXT("Floor404", "RemainingAnomaly", "아직 이 층에 남아있는 이상현상이 있습니다"), RemainingAnomalyNotificationDuration);
		}
		return;
	}

	AdvanceFloor();
}

void AFloor404ExitTrigger::AdvanceFloor()
{
	AFloor404GameState* GameState = GetWorld()->GetGameState<AFloor404GameState>();
	if (!GameState)
	{
		return;
	}

	GameState->AdvanceFloor();

	const int32 NewFloor = GameState->GetCurrentFloor();

	if (AnomalyManager)
	{
		AnomalyManager->ActivateAnomaliesForFloor(NewFloor);
	}

	TeleportPlayerToSpawnPoint();
}

void AFloor404ExitTrigger::TeleportPlayerToSpawnPoint()
{
	if (!SpawnPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("AExitTrigger: SpawnPoint가 설정되지 않았습니다."));
		return;
	}

	if (ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		const FRotator SpawnRotation = SpawnPoint->GetActorRotation();

		PlayerCharacter->SetActorLocationAndRotation(SpawnPoint->GetActorLocation(), SpawnRotation);

		if (APlayerController* PlayerController = Cast<APlayerController>(PlayerCharacter->GetController()))
		{
			PlayerController->SetControlRotation(SpawnRotation);
		}
	}
}