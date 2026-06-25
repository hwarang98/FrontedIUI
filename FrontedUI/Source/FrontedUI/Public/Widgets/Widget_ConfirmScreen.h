// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "FrontendTypes/FrontendEnum.h"
#include "Widget_ConfirmScreen.generated.h"

class UDynamicEntryBox;
class UCommonTextBlock;

/** 확인 화면에 표시될 버튼 하나의 타입과 표시 텍스트를 담는 구조체 */
USTRUCT(BlueprintType)
struct FConfirmScreenButtonInfo
{
	GENERATED_BODY()

	/** 버튼의 역할 (Confirmed / Cancelled / Closed 등) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EConfirmScreenButtonType ConfirmScreenButtonType = EConfirmScreenButtonType::UnKnow;

	/** 버튼에 실제로 표시될 텍스트 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ButtonTextToDisplay;
};

/**
 * 확인 화면(ConfirmScreen)에 전달할 데이터 컨테이너.
 * 팩토리 함수로 원하는 버튼 구성을 미리 세팅한 뒤 위젯에 넘긴다.
 * Transient 프로퍼티만 사용하므로 저장되지 않는다.
 */
UCLASS()
class FRONTEDUI_API UConfirmScreenInfoObject : public UObject
{
	GENERATED_BODY()

public:
	/** OK 버튼 하나만 있는 단순 알림 화면 생성 */
	static UConfirmScreenInfoObject* CreateOKScreen(const FText& InScreenTitle, const FText& InScreenMessage);

	/** Yes / No 두 버튼이 있는 선택 화면 생성 */
	static UConfirmScreenInfoObject* CreateYesNoScreen(const FText& InScreenTitle, const FText& InScreenMessage);

	/** OK(확인) / Cancel(취소) 두 버튼이 있는 선택 화면 생성 */
	static UConfirmScreenInfoObject* CreateOkCancelScreen(const FText& InScreenTitle, const FText& InScreenMessage);

	/** 화면 상단에 표시될 제목 텍스트 */
	UPROPERTY(Transient)
	FText ScreenTitle;

	/** 화면 본문에 표시될 메시지 텍스트 */
	UPROPERTY(Transient)
	FText ScreenMessage;

	/** 화면에 배치할 버튼 목록 — 순서대로 DynamicEntryBox에 추가됨 */
	UPROPERTY(Transient)
	TArray<FConfirmScreenButtonInfo> AvailableScreenButtons;
};

/**
 * 제목, 메시지, 버튼 목록을 표시하는 범용 확인 팝업 위젯.
 * UConfirmScreenInfoObject를 받아 UI를 구성하며,
 * 블루프린트에서 BindWidget 변수를 반드시 연결해야 한다.
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class FRONTEDUI_API UWidget_ConfirmScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()

public:
	/**
	 * 확인 화면을 초기화한다.
	 * 제목·메시지 텍스트를 설정하고, AvailableScreenButtons 목록을 순회해 버튼을 동적으로 생성한다.
	 * 버튼 클릭 시 ClickedButtonCallback이 호출된 뒤 위젯이 자동으로 비활성화된다.
	 *
	 * @param InScreenInfoObject	표시할 제목·메시지·버튼 정보가 담긴 데이터 오브젝트 (null 불가)
	 * @param ClickedButtonCallback	클릭된 버튼의 타입을 인자로 받는 콜백
	 */
	void InitConfirmScreen(const UConfirmScreenInfoObject* InScreenInfoObject, TFunction<void(EConfirmScreenButtonType)> ClickedButtonCallback);

protected:
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

private:
	/** 화면 제목을 표시하는 텍스트 블록 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> CommonTextBlock_Title;

	/** 화면 본문 메시지를 표시하는 텍스트 블록 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonTextBlock> CommonTextBlock_Message;

	/** 버튼 항목들을 동적으로 생성·배치하는 엔트리 박스 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UDynamicEntryBox> DynamicEntryBox_Buttons;

};