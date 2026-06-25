// LeeHwaRang All Rights Reserved

#include "Widgets/Options/Widget_KeyRemapScreen.h"
#include "CommonInputSubsystem.h"
#include "CommonRichTextBlock.h"
#include "FrontendDebugHelper.h"
#include "ICommonInputModule.h"
#include "CommonUITypes.h"
#include "Framework/Application/IInputProcessor.h"

class FKeyRemapScreenInputPreProcessor : public IInputProcessor
{
public:
	FKeyRemapScreenInputPreProcessor(ECommonInputType InInputTypeToListenTo, ULocalPlayer* InOwningLocalPlayer)
		: CachedInputTypeToListenTo(InInputTypeToListenTo)
		, CachedWeakOwningLocalPlayer(InOwningLocalPlayer) {}

	DECLARE_DELEGATE_OneParam(FOnInputPreProcessorKeyPressedDelegate, const FKey& /*PressedKey*/);
	FOnInputPreProcessorKeyPressedDelegate OnInputPreProcessorKeyPressed;

	DECLARE_DELEGATE_OneParam(FOnInputPreProcessorKeySelectCanceledDelegate, const FString& /*CanceledReason*/)
	FOnInputPreProcessorKeySelectCanceledDelegate OnInputPreProcessorKeySelectCanceled;

protected:
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override {}

	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override
	{
		ProcessKeyPressed(InKeyEvent.GetKey());

		return true;
	}

	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		ProcessKeyPressed(MouseEvent.GetEffectingButton());

		return true;
	}

	void ProcessKeyPressed(const FKey& InPressedKey)
	{
		// esc 눌렀을떄
		if (InPressedKey == EKeys::Escape)
		{
			OnInputPreProcessorKeySelectCanceled.ExecuteIfBound(TEXT("키 재맵핑이 취소되었습니다"));

			return;
		}

		UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(CachedWeakOwningLocalPlayer.Get());

		check(CommonInputSubsystem);

		ECommonInputType CurrentInputType = CommonInputSubsystem->GetCurrentInputType();

		switch (CachedInputTypeToListenTo)
		{
			case ECommonInputType::MouseAndKeyboard:

				if (InPressedKey.IsGamepadKey() || CurrentInputType == ECommonInputType::Gamepad)
				{
					OnInputPreProcessorKeySelectCanceled.ExecuteIfBound(TEXT("키보드 입력을 위해 게임패드 키를 누른 것이 감지되었습니다. 키 재맵핑이 취소되었습니다."));

					return;
				}

				break;

			case ECommonInputType::Gamepad:

				if (CurrentInputType == ECommonInputType::Gamepad && InPressedKey == EKeys::LeftMouseButton)
				{
					FCommonInputActionDataBase* InputActionData = ICommonInputModule::GetSettings().GetDefaultClickAction().GetRow<FCommonInputActionDataBase>(TEXT(""));

					check(InputActionData);

					OnInputPreProcessorKeyPressed.ExecuteIfBound(InputActionData->GetDefaultGamepadInputTypeInfo().GetKey());

					return;
				}

				if (!InPressedKey.IsGamepadKey())
				{
					OnInputPreProcessorKeySelectCanceled.ExecuteIfBound(TEXT("게임패드 입력을 위해 게임패드 이외의 키를 누른 것이 감지되었습니다. 키 재맵핑이 취소되었습니다."));

					return;
				}

				break;

			default:
				break;
		}

		OnInputPreProcessorKeyPressed.ExecuteIfBound(InPressedKey);
	}

private:
	ECommonInputType CachedInputTypeToListenTo;

	TWeakObjectPtr<ULocalPlayer> CachedWeakOwningLocalPlayer;
};

void UWidget_KeyRemapScreen::SetDesiredInputTypeToFilter(ECommonInputType InDesiredInputType)
{
	CachedDesiredInputType = InDesiredInputType;
}

void UWidget_KeyRemapScreen::NativeOnActivated()
{
	Super::NativeOnActivated();

	CachedInputPreprocessor = MakeShared<FKeyRemapScreenInputPreProcessor>(CachedDesiredInputType, GetOwningLocalPlayer());
	CachedInputPreprocessor->OnInputPreProcessorKeyPressed.BindUObject(this, &ThisClass::OnValidKeyPressedDetected);
	CachedInputPreprocessor->OnInputPreProcessorKeySelectCanceled.BindUObject(this, &ThisClass::OnKeySelectCanceled);

	FSlateApplication::Get().RegisterInputPreProcessor(CachedInputPreprocessor, -1);

	FString InputDeviceName;

	switch (CachedDesiredInputType)
	{
		case ECommonInputType::MouseAndKeyboard:
			InputDeviceName = TEXT("키보드 및 마우스");
			break;

		case ECommonInputType::Gamepad:
			InputDeviceName = TEXT("게임패드");
			break;

		default:
			break;
	}

	const FString DisplayRichMessage = FString::Printf(
		TEXT("<KeyRemapHighlight>%s</> <KeyRemapDefault>의 아무 키나 누르세요.</>"), *InputDeviceName
		);

	CommonRichText_RemapMessage->SetText(FText::FromString(DisplayRichMessage));
}

void UWidget_KeyRemapScreen::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	if (CachedInputPreprocessor)
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(CachedInputPreprocessor);

		CachedInputPreprocessor.Reset();
	}
}

void UWidget_KeyRemapScreen::OnValidKeyPressedDetected(const FKey& PressedKey)
{
	// PressedKey를 값으로 캡처해 다음 틱에서 람다가 실행될 때도 키 데이터가 유효하도록 합니다.
	RequestDeactivateWidget(
		[this,PressedKey]() {
			OnKeyRemapScreenKeyPressed.ExecuteIfBound(PressedKey);
		}
		);
}

void UWidget_KeyRemapScreen::OnKeySelectCanceled(const FString& CanceledReason)
{
	// CanceledReason을 값으로 캡처해 다음 틱에서 람다가 실행될 때도 문자열이 유효하도록 합니다.
	RequestDeactivateWidget(
		[this,CanceledReason]() {
			OnKeyRemapScreenKeySelectCanceled.ExecuteIfBound(CanceledReason);
		}
		);
}

void UWidget_KeyRemapScreen::RequestDeactivateWidget(TFunction<void()> PreDeactivateCallback)
{
	// 키 입력 이벤트가 아직 처리 중인 상태에서 즉시 위젯을 닫으면 입력이 누락될 수 있으므로 한 틱 지연합니다.
	FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateLambda(
			[PreDeactivateCallback,this](float DeltaTime)-> bool {
				PreDeactivateCallback();

				DeactivateWidget();

				return false; // false를 반환해 이 틱커 델리게이트를 즉시 제거합니다 (일회성 실행).
			}
			)
		);
}