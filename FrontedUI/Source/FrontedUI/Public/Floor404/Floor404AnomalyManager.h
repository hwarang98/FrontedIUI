// LeeHwaRang All Rights Reserved
// Actor Tag 기반 이상현상 등록, 활성화, 신고 판정을 담당하는 매니저

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Floor404AnomalyManager.generated.h"

/** 월드에 배치된 이상현상 액터 하나의 상태 데이터 */
USTRUCT(BlueprintType)
struct FAnomalyData
{
	GENERATED_BODY()

	/** 이상현상 고유 ID (액터 이름 기반) */
	UPROPERTY()
	FName AnomalyID;

	/** 해당 이상현상이 속한 층 번호 (Floor_N 태그에서 파싱) */
	UPROPERTY()
	int32 Floor = 0;

	/** 이상현상 대상 액터 */
	UPROPERTY()
	TObjectPtr<AActor> TargetActor = nullptr;

	/** 플레이어가 이미 신고했으면 true */
	UPROPERTY()
	bool bReported = false;

	/** 현재 층에서 활성화된 이상현상이면 true */
	UPROPERTY()
	bool bActive = false;
};

/**
 * BeginPlay 시 "Anomaly" 태그 액터를 수집하고 층별로 활성화/비활성화한다.
 * 레벨에 1개 배치되어야 한다.
 */
UCLASS()
class FRONTEDUI_API AFloor404AnomalyManager : public AActor
{
	GENERATED_BODY()

public:
	AFloor404AnomalyManager();

	/**
	 * @brief 해당 층의 이상현상을 활성화하고 나머지를 비활성화한다.
	 *
	 * @param Floor 활성화할 층 번호
	 */
	UFUNCTION(BlueprintCallable, Category = "Floor404|Anomaly")
	void ActivateAnomaliesForFloor(int32 Floor);

	/**
	 * @brief TargetActor가 현재 활성화된 이상현상이면 신고 처리 후 true를 반환한다.
	 *
	 * @param TargetActor 플레이어가 LineTrace로 Hit한 액터
	 * @return 정답(활성 이상현상 신고)이면 true, 오답이면 false
	 */
	UFUNCTION(BlueprintCallable, Category = "Floor404|Anomaly")
	bool TryReportAnomaly(AActor* TargetActor);

protected:
	virtual void BeginPlay() override;

	/** PIE 실행 시 이상현상 위치를 층별 색상으로 Debug Sphere + 이름 텍스트로 표시할지 여부. */
	UPROPERTY(EditDefaultsOnly, Category = "Floor404|Debug")
	bool bDebugShowAnomalies = true;

private:
	/** "Anomaly" 태그 액터를 수집하고 Floor_N 태그를 파싱해 AllAnomalies를 구성한다. */
	void InitializeAnomalies();

	/** AllAnomalies 항목마다 층별 색상 Debug Sphere와 이름 텍스트를 그린다. */
	void DrawDebugAnomalies() const;

	UPROPERTY()
	TArray<FAnomalyData> AllAnomalies;
};