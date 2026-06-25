// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FrontendLoadingScreenSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FRONTEDUI_API UFrontendLoadingScreenSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadingReasonUpdatedDelegate, const FString&, CurrentLoadingReason);

	UPROPERTY(BlueprintAssignable)
	FOnLoadingReasonUpdatedDelegate OnLoadingReasonUpdated;

	#pragma region USubsytem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	#pragma endregion

	#pragma region FTickableGameObject Interface
	virtual UWorld* GetTickableGameObjectWorld() const override;
	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	#pragma endregion

private:
	void OnMapPreLoaded(const FWorldContext& WorldContext, const FString& MapName);
	void OnMapPostLoaded(UWorld* LoadedWorld);
	void TryUpdateLoadingScreen();
	void TryDisplayLoadingScreenIfNone();
	void TryRemoveLoadingScreen();
	void NotifyLoadingScreenVisibilityChanged(bool bIsVisible);

	bool IsPreLoadScreenActive() const;
	bool ShouldShowLoadingScreen();
	bool CheckTheNeedToShowLoadingScreen();
	bool bIsCurrentlyLoadingMap = false;

	float HoldLoadingScreenStartUpTime = -1.f;

	FString CurrentLoadingReason;

	TSharedPtr<SWidget> CachedCreatedLoadingScreenWidget;
};