// LeeHwaRang All Rights Reserved

#include "Floor404/Floor404WallDisableTrigger.h"

#include "Floor404/Floor404GameState.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"

AFloor404WallDisableTrigger::AFloor404WallDisableTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;
	BoxComponent->SetBoxExtent(FVector(100.f, 100.f, 100.f));
	BoxComponent->SetCollisionProfileName(TEXT("Trigger"));
}

void AFloor404WallDisableTrigger::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AFloor404WallDisableTrigger::OnBoxBeginOverlap);
}

void AFloor404WallDisableTrigger::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bTriggered)
	{
		return;
	}

	if (!Cast<ACharacter>(OtherActor))
	{
		return;
	}

	const AFloor404GameState* GameState = GetWorld()->GetGameState<AFloor404GameState>();
	if (!GameState || GameState->GetCurrentFloor() != TriggerFloor)
	{
		return;
	}

	if (!GameState->IsExitUnlocked())
	{
		return;
	}

	if (TargetWalls.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("AFloor404WallDisableTrigger: TargetWalls가 설정되지 않았습니다."));
		return;
	}

	bTriggered = true;
	for (AActor* Wall : TargetWalls)
	{
		if (Wall)
		{
			Wall->SetActorHiddenInGame(true);
			Wall->SetActorEnableCollision(false);
		}
	}
}
