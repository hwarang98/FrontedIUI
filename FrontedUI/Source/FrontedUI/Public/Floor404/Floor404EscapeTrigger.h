// LeeHwaRang All Rights Reserved
// 8층 탈출 조건 달성 후 플레이어가 진입하면 게임 클리어를 처리하는 트리거 액터

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Floor404EscapeTrigger.generated.h"

class UBoxComponent;

UCLASS()
class FRONTEDUI_API AFloor404EscapeTrigger : public AActor
{
	GENERATED_BODY()

public:
	AFloor404EscapeTrigger();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Floor404|Escape", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> BoxComponent;

	/** true면 IsFinalFloor 조건을 무시하고 어느 층에서도 탈출 가능. */
	UPROPERTY(EditAnywhere, Category = "Floor404|Escape")
	bool bTestMode = false;

	bool bTriggered = false;

	UFUNCTION()
	void OnBoxBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
		);
};