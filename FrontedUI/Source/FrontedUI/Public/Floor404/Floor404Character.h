// LeeHwaRang All Rights Reserved
// E키 이상현상 신고 상호작용을 처리하는 1인칭 캐릭터

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Floor404Character.generated.h"

class UInputAction;
class AFloor404AnomalyManager;
class AFloor404GameState;

UCLASS()
class FRONTEDUI_API AFloor404Character : public ACharacter
{
	GENERATED_BODY()

public:
	AFloor404Character();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Floor404|Input")
	TObjectPtr<UInputAction> InteractAction;

	/** E키 SphereTrace 최대 거리 (cm) */
	UPROPERTY(EditDefaultsOnly, Category = "Floor404|Interaction")
	float InteractDistance = 400.f;

	/** 이상현상 판정 구체 반경 (cm) */
	UPROPERTY(EditDefaultsOnly, Category = "Floor404|Interaction")
	float InteractSphereRadius = 15.f;

	/** true이면 SphereTrace 결과를 뷰포트에 시각화한다 */
	UPROPERTY(EditDefaultsOnly, Category = "Floor404|Interaction")
	bool bDebugInteractTrace = false;

	/** 정답 신고 성공 시 HUD 알림 표시 시간 (초) */
	UPROPERTY(EditDefaultsOnly, Category = "Floor404|UI")
	float ReportAcceptedNotificationDuration = 1.0f;

	UPROPERTY()
	TObjectPtr<AFloor404AnomalyManager> CachedAnomalyManager;

	UPROPERTY()
	TObjectPtr<AFloor404GameState> CachedGameState;

	/**
	 * @brief 카메라 기준 SphereTrace로 이상현상 신고를 시도한다.
	 *
	 * @note Hit 액터가 없으면 아무 처리도 하지 않는다.
	 *       활성화된 이상현상이 아니면 오답 처리한다.
	 */
	void Interact();

	/**
	 * @brief PlayerCameraManager 기준으로 Visibility 채널 SphereTrace를 수행한다.
	 *
	 * @param OutHit 결과 HitResult
	 * @return 충돌이 있으면 true
	 */
	bool PerformInteractionTrace(FHitResult& OutHit) const;
};