// LeeHwaRang All Rights Reserved

#include "Floor404/Floor404Character.h"
#include "Floor404/Floor404AnomalyManager.h"
#include "Floor404/Floor404GameState.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputComponent.h"
#include "FrontendDebugHelper.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/FrontendUISubsystem.h"

AFloor404Character::AFloor404Character()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AFloor404Character::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundManagers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFloor404AnomalyManager::StaticClass(), FoundManagers);
	if (FoundManagers.Num() > 0)
	{
		CachedAnomalyManager = Cast<AFloor404AnomalyManager>(FoundManagers[0]);
	}

	CachedGameState = GetWorld()->GetGameState<AFloor404GameState>();
}

void AFloor404Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AFloor404Character::Interact);
	}
}

void AFloor404Character::Interact()
{
	if (!CachedAnomalyManager || !CachedGameState)
	{
		UE_LOG(LogTemp, Warning, TEXT("AFloor404Character::Interact: AnomalyManager 또는 GameState가 null입니다."));
		return;
	}

	FHitResult HitResult;
	if (!PerformInteractionTrace(HitResult))
	{
		return;
	}

	AActor* HitActor = HitResult.GetActor();
	if (!HitActor)
	{
		return;
	}

	if (CachedAnomalyManager->TryReportAnomaly(HitActor))
	{
		CachedGameState->RegisterCorrectReport();

		if (UFrontendUISubsystem* UISubsystem = UFrontendUISubsystem::Get(this))
		{
			UISubsystem->ShowHudNotification(NSLOCTEXT("Floor404", "ReportAccepted", "보고가 접수되었습니다"), ReportAcceptedNotificationDuration);
		}
	}
	else
	{
		CachedGameState->RegisterWrongReport();
	}
}

bool AFloor404Character::PerformInteractionTrace(FHitResult& OutHit) const
{
	const APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController)
	{
		return false;
	}

	FVector CameraLocation;
	FRotator CameraRotation;
	PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	const FVector End = CameraLocation + CameraRotation.Vector() * InteractDistance;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	const bool bHit = GetWorld()->SweepSingleByChannel(OutHit, CameraLocation, End, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(InteractSphereRadius), Params);

	if (bDebugInteractTrace)
	{
		const FColor HitColor = bHit ? FColor::Green : FColor::Red;
		const FVector HitPoint = bHit ? OutHit.ImpactPoint : End;
		DrawDebugLine(GetWorld(), CameraLocation, HitPoint, HitColor, false, 2.f);
		if (bHit)
		{
			DrawDebugSphere(GetWorld(), HitPoint, 8.f, 8, FColor::Green, false, 2.f);
		}
	}

	return bHit;
}