// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "ListDataObject_Base.h"
#include "ListDataObject_Action.generated.h"

/** 클릭 시 등록된 액션을 실행하는 단순 버튼형 옵션 데이터 오브젝트 */
UCLASS()
class FRONTEDUI_API UListDataObject_Action : public UListDataObject_Base
{
	GENERATED_BODY()

public:
	/** 항목 클릭 시 호출될 액션을 등록합니다 */
	void SetAction(TFunction<void()> InAction);

	/** 등록된 액션을 실행합니다 */
	void ExecuteAction() const;

private:
	TFunction<void()> BoundAction;
};
