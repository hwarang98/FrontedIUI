// LeeHwaRang All Rights Reserved

#include "Floor404/Floor404MonsterTrigger.h"

#include "Floor404/Floor404GameState.h"
#include "Floor404/Floor404MonsterActivatable.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

AFloor404MonsterTrigger::AFloor404MonsterTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	RootComponent = BoxComponent;
	BoxComponent->SetBoxExtent(FVector(100.f, 100.f, 100.f));
	BoxComponent->SetCollisionProfileName(TEXT("Trigger"));
}

void AFloor404MonsterTrigger::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AFloor404MonsterTrigger::OnBoxBeginOverlap);
}

void AFloor404MonsterTrigger::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

	if (!TargetEnemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("AFloor404MonsterTrigger: TargetEnemy가 설정되지 않았습니다."));
		return;
	}

	if (!TargetEnemy->Implements<UFloor404MonsterActivatable>())
	{
		UE_LOG(LogTemp, Warning, TEXT("AFloor404MonsterTrigger: TargetEnemy가 IFloor404MonsterActivatable을 구현하지 않았습니다."));
		return;
	}

	bTriggered = true;
	IFloor404MonsterActivatable::Execute_Activate(TargetEnemy);

	if (SpawnSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, SpawnSound, GetActorLocation());
	}
}