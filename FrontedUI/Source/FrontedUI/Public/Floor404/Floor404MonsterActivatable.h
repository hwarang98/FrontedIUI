// LeeHwaRang All Rights Reserved
// 몬스터 트리거에 의해 활성화될 수 있는 Actor의 인터페이스

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Floor404MonsterActivatable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UFloor404MonsterActivatable : public UInterface
{
	GENERATED_BODY()
};

class FRONTEDUI_API IFloor404MonsterActivatable
{
	GENERATED_BODY()

public:
	/** 트리거 발동 시 호출된다. */
	UFUNCTION(BlueprintNativeEvent, Category = "Floor404|Monster")
	void Activate();
};