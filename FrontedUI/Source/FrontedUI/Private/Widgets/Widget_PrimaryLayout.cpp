// LeeHwaRang All Rights Reserved


#include "Widgets/Widget_PrimaryLayout.h"
#include "FrontendDebugHelper.h"
#include "Subsystems/FrontendUISubsystem.h"

UCommonActivatableWidgetContainerBase* UWidget_PrimaryLayout::FindWidgetStackByTag(const FGameplayTag& InTag) const
{
	if (!RegisterWidgetStackMap.Contains(InTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("FindWidgetStackByTag: 태그에 해당하는 스택이 없습니다 : %s"), *InTag.ToString());
		return nullptr;
	}
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

			// 스택 등록 전에 async 콜백이 먼저 실행되어 큐에 쌓인 요청이 있으면 여기서 소진
			if (UFrontendUISubsystem* UISubsystem = UFrontendUISubsystem::Get(this))
			{
				UISubsystem->FlushPendingPushesForStack(InStackTag, InStack);
			}
		}
	}
}
