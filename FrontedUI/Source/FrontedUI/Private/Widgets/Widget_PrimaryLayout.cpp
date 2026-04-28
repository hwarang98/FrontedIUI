// LeeHwaRang All Rights Reserved


#include "Widgets/Widget_PrimaryLayout.h"
#include "FrontendDebugHelper.h"

UCommonActivatableWidgetContainerBase* UWidget_PrimaryLayout::FindWidgetStackByTag(const FGameplayTag& InTag) const
{
	// 태그에 해당하는 스택이 없으면 에러 메시지와 함께 크래시 — 호출 전 반드시 등록 필요
	checkf(RegisterWidgetStackMap.Contains(InTag), TEXT("태그로 위젯 스택을 찾을 수 없습니다 : %s"), *InTag.ToString());
	return RegisterWidgetStackMap.FindRef(InTag);
}

void UWidget_PrimaryLayout::RegisterWidgetStack(UPARAM(meta = (Categories = "Frontend.WidgetStack")) FGameplayTag InStackTag, UCommonActivatableWidgetContainerBase* InStack)
{
	// IsDesignTime()이 false일 때만 동작 — 에디터 프리뷰가 아닌 실제 런타임임을 보장
	if (!IsDesignTime())
	{
		// 동일 태그의 중복 등록을 방지하여 기존 스택이 덮어쓰이는 것을 막음
		if (!RegisterWidgetStackMap.Contains(InStackTag))
		{
			RegisterWidgetStackMap.Add(InStackTag, InStack);
		}
	}
}
