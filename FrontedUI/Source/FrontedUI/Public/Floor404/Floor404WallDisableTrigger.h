// LeeHwaRang All Rights Reserved
// 플레이어가 진입하고 탈출 조건이 달성된 경우 지정된 벽 액터를 비활성화하는 트리거 액터

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Floor404WallDisableTrigger.generated.h"

class UBoxComponent;

UCLASS()
class FRONTEDUI_API AFloor404WallDisableTrigger : public AActor
{
	GENERATED_BODY()

public:
	AFloor404WallDisableTrigger();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Floor404|Wall", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> BoxComponent;

	/** Details Panel에서 연결할 비활성화 대상 벽 액터 목록. */
	UPROPERTY(EditInstanceOnly, Category = "Floor404|Wall")
	TArray<TObjectPtr<AActor>> TargetWalls;

	/** 이 트리거가 동작할 층 번호. */
	UPROPERTY(EditInstanceOnly, Category = "Floor404|Wall")
	int32 TriggerFloor = 8;

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
