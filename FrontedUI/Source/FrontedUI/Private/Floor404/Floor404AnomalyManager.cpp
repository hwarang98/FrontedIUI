// LeeHwaRang All Rights Reserved

#include "Floor404/Floor404AnomalyManager.h"
#include "Floor404/Floor404GameState.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

AFloor404AnomalyManager::AFloor404AnomalyManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AFloor404AnomalyManager::BeginPlay()
{
	Super::BeginPlay();

	InitializeAnomalies();

	if (const AFloor404GameState* GameState = GetWorld()->GetGameState<AFloor404GameState>())
	{
		ActivateAnomaliesForFloor(GameState->GetCurrentFloor());
	}
}

void AFloor404AnomalyManager::InitializeAnomalies()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Anomaly"), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (!Actor)
		{
			continue;
		}

		int32 FloorNum = 0;
		bool bFoundFloor = false;

		for (const FName& Tag : Actor->Tags)
		{
			FString TagStr = Tag.ToString();
			if (TagStr.StartsWith(TEXT("Floor_")))
			{
				FloorNum = FCString::Atoi(*TagStr.Mid(6));
				bFoundFloor = true;
				break;
			}
		}

		if (!bFoundFloor)
		{
			UE_LOG(LogTemp, Warning, TEXT("AAnomalyManager: 액터 '%s'에 'Anomaly' 태그는 있지만 'Floor_N' 태그가 없습니다."), *Actor->GetName());
			continue;
		}

		FAnomalyData& Data = AllAnomalies.AddDefaulted_GetRef();
		Data.AnomalyID = Actor->GetFName();
		Data.Floor = FloorNum;
		Data.TargetActor = Actor;
		Data.bReported = false;
		Data.bActive = false;
	}

	if (bDebugShowAnomalies)
	{
		DrawDebugAnomalies();
	}
}

void AFloor404AnomalyManager::ActivateAnomaliesForFloor(int32 Floor)
{
	for (FAnomalyData& Data : AllAnomalies)
	{
		if (Data.Floor == Floor)
		{
			Data.bActive = true;
			Data.bReported = false;
			if (Data.TargetActor)
			{
				Data.TargetActor->SetActorHiddenInGame(false);
				Data.TargetActor->SetActorEnableCollision(true);
			}
		}
		else
		{
			Data.bActive = false;
			if (Data.TargetActor && !Data.bReported)
			{
				Data.TargetActor->SetActorHiddenInGame(true);
				Data.TargetActor->SetActorEnableCollision(false);
			}
		}
	}
}

bool AFloor404AnomalyManager::TryReportAnomaly(AActor* TargetActor)
{
	for (FAnomalyData& Data : AllAnomalies)
	{
		if (Data.TargetActor == TargetActor && Data.bActive && !Data.bReported)
		{
			Data.bReported = true;
			return true;
		}
	}
	return false;
}

void AFloor404AnomalyManager::DrawDebugAnomalies() const
{
	static const FColor FloorColors[] = {
		FColor::Green,  // Floor 1
		FColor::Green,  // Floor 2
		FColor::Yellow, // Floor 3
		FColor::Yellow, // Floor 4
		FColor::Orange, // Floor 5
		FColor::Orange, // Floor 6
		FColor::Red,    // Floor 7
		FColor::Red,    // Floor 8
	};

	for (const FAnomalyData& Data : AllAnomalies)
	{
		if (!Data.TargetActor)
		{
			continue;
		}

		const int32 ColorIndex = FMath::Clamp(Data.Floor - 1, 0, 7);
		const FColor Color = FloorColors[ColorIndex];
		const FVector Location = Data.TargetActor->GetActorLocation();

		DrawDebugSphere(GetWorld(), Location, 60.f, 12, Color, true);
		DrawDebugString(GetWorld(), Location + FVector(0.f, 0.f, 80.f),
			FString::Printf(TEXT("[F%d] %s"), Data.Floor, *Data.TargetActor->GetName()),
			nullptr, Color, -1.f, true, 1.2f);
	}
}