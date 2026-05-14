// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonRotator.h"
#include "FrontendCommonRotator.generated.h"

/**
 * CommonRotator를 상속한 프론트엔드 전용 로테이터 위젯.
 * NativeTick을 비활성화하여 성능을 최적화합니다.
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class FRONTEDUI_API UFrontendCommonRotator : public UCommonRotator
{
	GENERATED_BODY()

public:
	void SetSelectedOptionByText(const FText& InTextOption);

};