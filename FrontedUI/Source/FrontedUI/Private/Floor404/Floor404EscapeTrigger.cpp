// LeeHwaRang All Rights Reserved

#include "Floor404/Floor404EscapeTrigger.h"

#include "Floor404/Floor404Character.h"
#include "Floor404/Floor404GameMode.h"
#include "Floor404/Floor404GameState.h"
#include "Components/BoxComponent.h"

AFloor404EscapeTrigger::AFloor404EscapeTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;
	BoxComponent->SetBoxExtent(FVector(100.f, 100.f, 100.f));
	BoxComponent->SetCollisionProfileName(TEXT("Trigger"));
}

void AFloor404EscapeTrigger::BeginPlay()
{
	Super::BeginPlay();
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AFloor404EscapeTrigger::OnBoxBeginOverlap);
}

void AFloor404EscapeTrigger::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bTriggered)
	{
		return;
	}

	if (!Cast<AFloor404Character>(OtherActor))
	{
		return;
	}

	const AFloor404GameState* GameState = GetWorld()->GetGameState<AFloor404GameState>();
	if (!GameState || !GameState->IsExitUnlocked() || (!bTestMode && !GameState->IsFinalFloor()))
	{
		return;
	}

	bTriggered = true;

	if (AFloor404GameMode* GameMode = Cast<AFloor404GameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->HandleGameClear();
	}
}