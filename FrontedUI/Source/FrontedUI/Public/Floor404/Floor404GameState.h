// LeeHwaRang All Rights Reserved
// 층 진행 / 이상현상 신고 수 / 출구 해금 / 몬스터 추격 플래그를 관리하는 GameState

#pragma once

#include "CoreMinimal.h"
#include "Floor404GameStateBase.h"
#include "Floor404GameState.generated.h"

/** ProgressStep과 몬스터 추격 시작 여부가 변경될 때 Broadcast된다. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFloor404ProgressChanged, int32, NewProgressStep, bool, bNewShouldMonsterChase);

/** 층 이동이 완료되어 새 층으로 진입할 때 Broadcast된다. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFloor404FloorAdvanced, int32, NewFloor);

/** 게임 클리어 시 Broadcast된다. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFloor404GameClear);

/** 게임 오버 시 Broadcast된다. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFloor404GameOver);

/** 층별 진행 데이터. FloorProgressDataList에 8개 항목으로 저장된다. */
USTRUCT(BlueprintType)
struct FFloorProgressData
{
	GENERATED_BODY()

	/** 층 번호 (1~8). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Floor404|FloorData")
	int32 Floor = 1;

	/** 해당 층에서 출구 해금에 필요한 이상현상 신고 수. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Floor404|FloorData")
	int32 RequiredReportCount = 1;

	/** 해당 층의 몬스터 행동 단계. 0=Hidden / 1=Observe / 2=Chase */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Floor404|FloorData")
	int32 ProgressStep = 0;
};

/**
 * Floor 404의 게임 진행 상태를 관리한다.
 * 층수, 신고 수, 출구 해금, 몬스터 추격 여부를 보유하며
 * 외부에서 직접 변수를 수정하지 않도록 Setter 함수 중심으로 설계한다.
 */
UCLASS()
class FRONTEDUI_API AFloor404GameState : public AFloor404GameStateBase
{
	GENERATED_BODY()

public:
	AFloor404GameState();

	/** GameState의 ProgressStep 또는 bMonsterChaseStarted가 변경될 때 Broadcast된다. */
	UPROPERTY(BlueprintAssignable, Category = "Floor404|GameState")
	FOnFloor404ProgressChanged OnProgressChanged;

	/** 층 이동이 완료되어 새 층으로 진입할 때 Broadcast된다. BP_Enemy의 초기화 바인딩에 사용한다. */
	UPROPERTY(BlueprintAssignable, Category = "Floor404|GameState")
	FOnFloor404FloorAdvanced OnFloorAdvanced;

	/** 게임 클리어 시 Broadcast된다. BP_Enemy와 Clear UI가 바인딩해 사용한다. */
	UPROPERTY(BlueprintAssignable, Category = "Floor404|GameState")
	FOnFloor404GameClear OnGameClear;

	/** 게임 오버 시 Broadcast된다. BP_Enemy가 바인딩해 추격 중지에 사용한다. */
	UPROPERTY(BlueprintAssignable, Category = "Floor404|GameState")
	FOnFloor404GameOver OnGameOver;

	/**
	 * @brief 정답 신고를 처리한다.
	 *
	 * @note CurrentReportCount를 증가시키고 RequiredReportCount에 도달하면
	 *       bExitUnlocked를 true로 설정한다.
	 *       CurrentFloor가 MaxFloor(8층)이면 bMonsterChaseStarted도 true로 설정한다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Floor404|GameState")
	void RegisterCorrectReport();

	/** WrongReportCount를 1 증가시킨다. 전체 게임 누적으로 관리된다. */
	UFUNCTION(BlueprintCallable, Category = "Floor404|GameState")
	void RegisterWrongReport();

	/**
	 * @brief 다음 층으로 이동한다.
	 *
	 * @note bExitUnlocked가 true일 때만 동작한다.
	 *       층 이동 후 ResetFloorProgress() → UpdateRequiredReportCount() → UpdateProgressStep() 순서로 호출한다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Floor404|GameState")
	void AdvanceFloor();

	/** CurrentFloor에 해당하는 FloorProgressData에서 RequiredReportCount를 읽어 갱신한다. */
	UFUNCTION(BlueprintCallable, Category = "Floor404|GameState")
	void UpdateRequiredReportCount();

	/** CurrentFloor에 해당하는 FloorProgressData에서 ProgressStep을 읽어 갱신한다. */
	UFUNCTION(BlueprintCallable, Category = "Floor404|GameState")
	void UpdateProgressStep();

	/**
	 * @brief 층 이동 시 해당 층의 진행 상태를 초기화한다.
	 *
	 * @note CurrentReportCount / bExitUnlocked / bMonsterChaseStarted를 리셋한다.
	 *       WrongReportCount는 전체 게임 누적 값이므로 리셋하지 않는다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Floor404|GameState")
	void ResetFloorProgress();

	/**
	 * @brief 지정한 층에서 필요한 신고 수를 반환한다.
	 *
	 * @param Floor 조회할 층 번호
	 * @return 해당 층의 RequiredReportCount. 층을 찾지 못하면 1을 반환한다.
	 */
	UFUNCTION(BlueprintPure, Category = "Floor404|GameState")
	int32 GetRequiredReportCountForFloor(int32 Floor) const;

	/** bExitUnlocked 값을 반환한다. */
	UFUNCTION(BlueprintPure, Category = "Floor404|GameState")
	bool IsExitUnlocked() const;

	/** bMonsterChaseStarted 값을 반환한다. */
	UFUNCTION(BlueprintPure, Category = "Floor404|GameState")
	bool HasMonsterChaseStarted() const;

	/** CurrentFloor가 MaxFloor 이상이면 true를 반환한다. */
	UFUNCTION(BlueprintPure, Category = "Floor404|GameState")
	bool IsFinalFloor() const;

	UFUNCTION(BlueprintPure, Category = "Floor404|GameState")
	int32 GetCurrentFloor() const;

	UFUNCTION(BlueprintPure, Category = "Floor404|GameState")
	int32 GetMaxFloor() const;

	UFUNCTION(BlueprintPure, Category = "Floor404|GameState")
	int32 GetCurrentReportCount() const;

	UFUNCTION(BlueprintPure, Category = "Floor404|GameState")
	int32 GetRequiredReportCount() const;

	UFUNCTION(BlueprintPure, Category = "Floor404|GameState")
	int32 GetWrongReportCount() const;

	UFUNCTION(BlueprintPure, Category = "Floor404|GameState")
	int32 GetProgressStep() const;

	/** 게임 클리어 처리를 수행한다. bGameCleared를 세트하고 OnGameClear를 Broadcast한다. */
	UFUNCTION(BlueprintCallable, Category = "Floor404|GameState")
	void HandleGameClear();

	/** 게임 오버 처리를 수행한다. bGameOver를 세트하고 OnGameOver를 Broadcast한다. */
	UFUNCTION(BlueprintCallable, Category = "Floor404|GameState")
	void HandleGameOver();

	/** 게임 시작 이후 경과 시간을 초 단위로 반환한다. */
	UFUNCTION(BlueprintPure, Category = "Floor404|GameState")
	float GetElapsedSeconds() const;

	/** 전체 게임에서 정답으로 처리된 총 이상현상 신고 수를 반환한다. */
	UFUNCTION(BlueprintPure, Category = "Floor404|GameState")
	int32 GetTotalCorrectReports() const;

protected:
	virtual void BeginPlay() override;

	/**
	 * 층별 진행 데이터 목록.
	 * Blueprint Class Defaults에서 수정 가능하며 기본값은 생성자에서 초기화된다.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Floor404|FloorData")
	TArray<FFloorProgressData> FloorProgressDataList;

	/** 현재 층수. 1층에서 시작하며 MaxFloor를 초과하지 않는다. */
	UPROPERTY(BlueprintReadOnly, Category = "Floor404|GameState")
	int32 CurrentFloor = 1;

	/** 최대 층수. */
	UPROPERTY(BlueprintReadOnly, Category = "Floor404|GameState")
	int32 MaxFloor = 8;

	/** 현재 층에서 완료된 정답 신고 수. */
	UPROPERTY(BlueprintReadOnly, Category = "Floor404|GameState")
	int32 CurrentReportCount = 0;

	/** 현재 층에서 출구 해금에 필요한 신고 수. FloorProgressDataList에서 읽어온 캐시 값이다. */
	UPROPERTY(BlueprintReadOnly, Category = "Floor404|GameState")
	int32 RequiredReportCount = 1;

	/** 전체 게임 누적 오답 신고 수. 층 이동 시 리셋하지 않는다. */
	UPROPERTY(BlueprintReadOnly, Category = "Floor404|GameState")
	int32 WrongReportCount = 0;

	/** 몬스터 행동 단계. 0=Hidden / 1=Observe / 2=Chase */
	UPROPERTY(BlueprintReadOnly, Category = "Floor404|GameState")
	int32 ProgressStep = 0;

	/** 필요한 신고 수를 채워 출구가 활성화된 상태. */
	UPROPERTY(BlueprintReadOnly, Category = "Floor404|GameState")
	bool bExitUnlocked = false;

	/** 8층(최종층)에서 필요한 신고 수를 채운 후 몬스터 추격이 시작된 상태. */
	UPROPERTY(BlueprintReadOnly, Category = "Floor404|GameState")
	bool bMonsterChaseStarted = false;

	/** 게임 시작 시각. GetElapsedSeconds() 계산에 사용한다. */
	UPROPERTY(BlueprintReadOnly, Category = "Floor404|GameState")
	float GameStartTime = 0.f;

	/** 전체 게임 누적 정답 신고 수. 층 이동 시 리셋하지 않는다. */
	UPROPERTY(BlueprintReadOnly, Category = "Floor404|GameState")
	int32 TotalCorrectReports = 0;

	/** 게임 클리어 여부. HandleGameClear 중복 실행 방지에 사용한다. */
	UPROPERTY(BlueprintReadOnly, Category = "Floor404|GameState")
	bool bGameCleared = false;

	/** 게임 오버 여부. HandleGameOver 중복 실행 방지에 사용한다. */
	UPROPERTY(BlueprintReadOnly, Category = "Floor404|GameState")
	bool bGameOver = false;

	/** 출구가 해금될 때 재생할 사운드. */
	UPROPERTY(EditDefaultsOnly, Category = "Floor404|Sound")
	TObjectPtr<USoundBase> ExitUnlockSound;

private:
	/** 현재 ProgressStep과 bMonsterChaseStarted 값으로 OnProgressChanged를 Broadcast한다. */
	void BroadcastProgress();
};