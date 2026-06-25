// LeeHwaRang All Rights Reserved

#include "Floor404/Floor404FloorIndicator.h"

#include "Floor404/Floor404GameState.h"
#include "Components/TextRenderComponent.h"

AFloor404FloorIndicator::AFloor404FloorIndicator()
{
	PrimaryActorTick.bCanEverTick = false;

	FloorTextRender = CreateDefaultSubobject<UTextRenderComponent>(TEXT("FloorTextRender"));
	RootComponent = FloorTextRender;
	FloorTextRender->SetHorizontalAlignment(EHTA_Center);
	FloorTextRender->SetVerticalAlignment(EVRTA_TextCenter);
	FloorTextRender->SetText(FText::FromString(TEXT("1F")));
}

void AFloor404FloorIndicator::BeginPlay()
{
	Super::BeginPlay();

	AFloor404GameState* GameState = GetWorld()->GetGameState<AFloor404GameState>();
	if (!GameState)
	{
		return;
	}

	FloorTextRender->SetText(MakeFloorText(GameState->GetCurrentFloor()));
	GameState->OnFloorAdvanced.AddDynamic(this, &ThisClass::OnFloorAdvanced);
}

void AFloor404FloorIndicator::OnFloorAdvanced(int32 NewFloor)
{
	FloorTextRender->SetText(MakeFloorText(NewFloor));
}

FText AFloor404FloorIndicator::MakeFloorText(int32 Floor) const
{
	return FText::FromString(FString::Printf(TEXT("%dF"), Floor));
}