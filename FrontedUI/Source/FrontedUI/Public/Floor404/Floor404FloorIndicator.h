// LeeHwaRang All Rights Reserved
// TextRenderComponent로 현재 층(1F~8F)을 표시하는 액터

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Floor404FloorIndicator.generated.h"

class UTextRenderComponent;

UCLASS()
class FRONTEDUI_API AFloor404FloorIndicator : public AActor
{
	GENERATED_BODY()

public:
	AFloor404FloorIndicator();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Floor404|FloorIndicator", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTextRenderComponent> FloorTextRender;

	/** OnFloorAdvanced 델리게이트에 바인딩되어 층 텍스트를 갱신한다. */
	UFUNCTION()
	void OnFloorAdvanced(int32 NewFloor);

	/** 층 번호를 받아 "1F" 형식의 FText를 반환한다. */
	FText MakeFloorText(int32 Floor) const;
};
