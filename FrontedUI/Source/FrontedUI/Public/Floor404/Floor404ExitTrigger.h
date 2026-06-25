// LeeHwaRang All Rights Reserved
// 플레이어가 출구 영역에 진입하면 층 전환을 처리하는 트리거 액터

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Floor404ExitTrigger.generated.h"

class UBoxComponent;
class AFloor404AnomalyManager;

UCLASS()
class FRONTEDUI_API AFloor404ExitTrigger : public AActor
{
	GENERATED_BODY()

public:
	AFloor404ExitTrigger();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Floor404|Exit", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY()
	TObjectPtr<AFloor404AnomalyManager> AnomalyManager;

	/**
	 * @brief 플레이어 캐릭터가 박스에 진입하면 출구 해금 여부를 확인하고 층을 전환한다.
	 */
	UFUNCTION()
	void OnBoxBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
		);

	/** GameState->AdvanceFloor() 호출 후 이상현상 활성화 및 플레이어 텔레포트를 수행한다. */
	void AdvanceFloor();

	UPROPERTY(EditAnywhere, Category = "Floor404|Exit")
	TObjectPtr<AActor> SpawnPoint;

	/** true면 층 전환 없이 SpawnPoint로 텔레포트만 수행한다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floor404|Exit", meta = (AllowPrivateAccess = "true"))
	bool bTeleportOnly = false;

	/** 이상현상이 남아 출구가 막혔을 때 HUD 알림 표시 시간 (초) */
	UPROPERTY(EditAnywhere, Category = "Floor404|UI")
	float RemainingAnomalyNotificationDuration = 1.5f;

	void TeleportPlayerToSpawnPoint();
};