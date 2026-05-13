// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widget_OptionsScreen.generated.h"

class UOptionsDataRegistry;
class UFrontendCommonListView;
class UFrontendTabListWidgetBase;

/**
 * 옵션 화면 위젯.
 * 초기화 시 Reset/Back 입력 액션을 바인딩하고,
 * 활성화 시 DataRegistry로부터 탭 컬렉션을 받아 TabListWidget에 탭을 등록합니다.
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class FRONTEDUI_API UWidget_OptionsScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()

protected:
	// UUserWidget Interface
	virtual void NativeOnInitialized() override; // Reset/Back 입력 액션 바인딩 등록
	virtual void NativeOnActivated() override;   // DataRegistry 탭 컬렉션을 TabListWidget에 등록

	// 탭 선택 시 호출되어 해당 탭의 옵션 항목 목록을 리스트 뷰에 갱신합니다.
	UFUNCTION()
	void OnOptionsTabSelected(FName TabID);

private:
	/** CreatedOwningDataRegistry가 없으면 생성하여 반환합니다. 직접 멤버에 접근하는 대신 이 함수를 사용하세요. */
	UOptionsDataRegistry* GetOrCreateDataRegistry();

	void OnResetBoundActionTriggered(); // Reset 액션 입력 시 호출
	void OnBackBoundActionTriggered();  // Back 액션 입력 시 호출 (위젯 비활성화)

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFrontendTabListWidgetBase> TabListWidget_OptionsTabs; // 옵션 탭 목록을 표시하는 위젯 (BP 바인딩 필수)

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFrontendCommonListView> CommonListView_OptionsList; // 선택된 탭의 옵션 항목을 표시하는 리스트 뷰 (BP 바인딩 필수)

	// 옵션 탭 데이터 생성을 담당하는 레지스트리. 반드시 GetOrCreateDataRegistry()를 통해 접근하세요.
	UPROPERTY(Transient)
	TObjectPtr<UOptionsDataRegistry> CreatedOwningDataRegistry;

	UPROPERTY(EditDefaultsOnly, Category = "Frontend Options Screen", meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle ResetAction; // 설정 초기화 입력 액션 (DataTable 행 참조)

	FUIActionBindingHandle ResetActionHandle; // ResetAction 바인딩 핸들 (바인딩 해제 시 사용)
};