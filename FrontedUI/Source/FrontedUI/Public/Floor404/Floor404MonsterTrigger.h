// LeeHwaRang All Rights Reserved
// 플레이어가 진입하면 지정된 Enemy Actor에게 활성화 신호를 전달하는 트리거 액터

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Floor404MonsterTrigger.generated.h"

class UBoxComponent;

UCLASS()
class FRONTEDUI_API AFloor404MonsterTrigger : public AActor
{
	GENERATED_BODY()

public:
	AFloor404MonsterTrigger();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Floor404|Monster", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> BoxComponent;

	/** Details Panel에서 연결할 대상 Enemy Actor. IFloor404MonsterActivatable을 구현해야 한다. */
	UPROPERTY(EditInstanceOnly, Category = "Floor404|Monster")
	TObjectPtr<AActor> TargetEnemy;

	/** 이 트리거가 활성화될 층 번호. */
	UPROPERTY(EditInstanceOnly, Category = "Floor404|Monster")
	int32 TriggerFloor = 1;

	/** 몬스터가 활성화될 때 재생할 사운드. */
	UPROPERTY(EditDefaultsOnly, Category = "Floor404|Monster")
	TObjectPtr<USoundBase> SpawnSound;

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