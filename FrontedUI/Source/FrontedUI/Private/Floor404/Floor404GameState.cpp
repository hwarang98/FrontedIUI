// LeeHwaRang All Rights Reserved

#include "Floor404/Floor404GameState.h"
#include "FrontendDebugHelper.h"
#include "Kismet/GameplayStatics.h"

AFloor404GameState::AFloor404GameState()
{
	// 층별 기본 데이터 초기화 (Floor / RequiredReportCount / ProgressStep)
	FloorProgressDataList.Reserve(8);

	auto AddFloor = [this](int32 InFloor, int32 InRequired, int32 InStep) {
		FFloorProgressData& Data = FloorProgressDataList.AddDefaulted_GetRef();
		Data.Floor = InFloor;
		Data.RequiredReportCount = InRequired;
		Data.ProgressStep = InStep;
	};

	// 각 층별 이상현상 갯수
	AddFloor(1, 0, 0); // 1F: 신고 1개, Enemy Hidden
	AddFloor(2, 2, 0); // 2F: 신고 2개, Enemy Hidden
	AddFloor(3, 2, 0); // 3F: 신고 2개, Enemy Hidden
	AddFloor(4, 3, 0); // 4F: 신고 3개, Enemy Hidden
	AddFloor(5, 3, 1); // 5F: 신고 3개, Enemy Observe
	AddFloor(6, 3, 1); // 6F: 신고 3개, Enemy Observe
	AddFloor(7, 2, 2); // 7F: 신고 2개, Enemy Chase
	AddFloor(8, 2, 2); // 8F: 신고 2개, Enemy Chase

	CurrentFloor = 1;
	MaxFloor = 8;
	CurrentReportCount = 0;
	RequiredReportCount = GetRequiredReportCountForFloor(CurrentFloor);
	WrongReportCount = 0;
	ProgressStep = 0;
	bExitUnlocked = (RequiredReportCount == 0);
	bMonsterChaseStarted = false;
}

void AFloor404GameState::BeginPlay()
{
	Super::BeginPlay();
	GameStartTime = GetWorld()->GetTimeSeconds();
}

void AFloor404GameState::RegisterCorrectReport()
{
	CurrentReportCount++;
	TotalCorrectReports++;

	if (CurrentReportCount >= RequiredReportCount)
	{
		bExitUnlocked = true;

		if (ExitUnlockSound)
		{
			UGameplayStatics::PlaySound2D(this, ExitUnlockSound);
		}

		if (CurrentFloor >= MaxFloor && !bMonsterChaseStarted)
		{
			bMonsterChaseStarted = true;
			BroadcastProgress();
		}
	}
}

void AFloor404GameState::RegisterWrongReport()
{
	WrongReportCount++;
}

void AFloor404GameState::AdvanceFloor()
{
	if (!bExitUnlocked)
	{
		return;
	}

	if (CurrentFloor < MaxFloor)
	{
		CurrentFloor++;
	}

	ResetFloorProgress();
	UpdateRequiredReportCount();
	UpdateProgressStep();
	OnFloorAdvanced.Broadcast(CurrentFloor);
}

void AFloor404GameState::UpdateRequiredReportCount()
{
	RequiredReportCount = GetRequiredReportCountForFloor(CurrentFloor);
	if (RequiredReportCount == 0)
	{
		bExitUnlocked = true;
	}
}

void AFloor404GameState::UpdateProgressStep()
{
	for (const FFloorProgressData& Data : FloorProgressDataList)
	{
		if (Data.Floor == CurrentFloor)
		{
			if (Data.ProgressStep != ProgressStep)
			{
				ProgressStep = Data.ProgressStep;
				BroadcastProgress();
			}
			return;
		}
	}

	if (ProgressStep != 0)
	{
		ProgressStep = 0;
		BroadcastProgress();
	}
}

void AFloor404GameState::BroadcastProgress()
{
	OnProgressChanged.Broadcast(ProgressStep, bMonsterChaseStarted);
}

void AFloor404GameState::ResetFloorProgress()
{
	CurrentReportCount = 0;
	bExitUnlocked = false;
	bMonsterChaseStarted = false;
}

int32 AFloor404GameState::GetRequiredReportCountForFloor(int32 Floor) const
{
	for (const FFloorProgressData& Data : FloorProgressDataList)
	{
		if (Data.Floor == Floor)
		{
			return Data.RequiredReportCount;
		}
	}

	return 1;
}

bool AFloor404GameState::IsExitUnlocked() const
{
	return bExitUnlocked;
}

bool AFloor404GameState::HasMonsterChaseStarted() const
{
	return bMonsterChaseStarted;
}

bool AFloor404GameState::IsFinalFloor() const
{
	return CurrentFloor >= MaxFloor;
}

int32 AFloor404GameState::GetCurrentFloor() const
{
	return CurrentFloor;
}

int32 AFloor404GameState::GetMaxFloor() const
{
	return MaxFloor;
}

int32 AFloor404GameState::GetCurrentReportCount() const
{
	return CurrentReportCount;
}

int32 AFloor404GameState::GetRequiredReportCount() const
{
	return RequiredReportCount;
}

int32 AFloor404GameState::GetWrongReportCount() const
{
	return WrongReportCount;
}

int32 AFloor404GameState::GetProgressStep() const
{
	return ProgressStep;
}

void AFloor404GameState::HandleGameClear()
{
	if (bGameCleared)
	{
		return;
	}

	bGameCleared = true;
	OnGameClear.Broadcast();
}

void AFloor404GameState::HandleGameOver()
{
	if (bGameOver)
	{
		return;
	}

	bGameOver = true;
	OnGameOver.Broadcast();
}

float AFloor404GameState::GetElapsedSeconds() const
{
	return GetWorld()->GetTimeSeconds() - GameStartTime;
}

int32 AFloor404GameState::GetTotalCorrectReports() const
{
	return TotalCorrectReports;
}