// LeeHwaRang All Rights Reserved


#include "Widgets/Options/OptionsDataRegistry.h"
#include "FrontendSettings/FrontendGameUserSettings.h"
#include "Widgets/Options/OptionsDataInteractionHelper.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"
#include "FrontendFunctionLibrary.h"
#include "FrontendGameplayTags.h"

#define MAKE_OPTIONS_DATA_CONTROL(SetterOrGetterFunctionName) \
	MakeShared<FOptionsDataInteractionHelper>(GET_FUNCTION_NAME_STRING_CHECKED(UFrontendGameUserSettings, SetterOrGetterFunctionName))

void UOptionsDataRegistry::InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer)
{
	// 각 카테고리 탭을 순서대로 생성합니다. 탭 표시 순서는 이 호출 순서를 따릅니다.
	InitGameplayCollectionTab();
	InitAudioCollectionTab();
	InitVideoCollectionTab();
	InitControlCollectionTab();
}

TArray<UListDataObject_Base*> UOptionsDataRegistry::GetListSourceItemsBySelectedTabID(const FName& InSelectedTabID)
{
	UListDataObject_Collection* const* FoundTabCollectionPtr = RegisteredOptionsTabCollections.FindByPredicate(
		[InSelectedTabID](const UListDataObject_Collection* AvailableTabCollection)-> bool {
			return AvailableTabCollection->GetDataID() == InSelectedTabID;
		});

	checkf(FoundTabCollectionPtr, TEXT("%s 에 해당하는 유효한 Tab ID를 찾을 수 없습니다."), *InSelectedTabID.ToString());

	const UListDataObject_Collection* FoundTabCollection = *FoundTabCollectionPtr;

	return FoundTabCollection->GetAllChildListData();
}

void UOptionsDataRegistry::InitGameplayCollectionTab()
{
	UListDataObject_Collection* GameplayTabCollection = NewObject<UListDataObject_Collection>();
	GameplayTabCollection->SetDataID(FName("GameplayTabCollection"));
	GameplayTabCollection->SetDataDisplayName(FText::FromString(TEXT("게임 플레이")));

	// 다음은 데이터 인터랙터 헬퍼를 생성하는 전체 코드입니다.
	// TSharedPtr<FOptionsDataInteractionHelper> ConstructedHelper =
	// 	MakeShared<FOptionsDataInteractionHelper>(GET_FUNCTION_NAME_STRING_CHECKED(UFrontendGameUserSettings, GetCurrentGameDifficulty));

	{
		UListDataObject_String* GameDifficulty = NewObject<UListDataObject_String>();
		GameDifficulty->SetDataID(FName("GameDifficulty"));
		GameDifficulty->SetDataDisplayName(FText::FromString(TEXT("난이도")));
		GameDifficulty->SetDescriptionRichText(FText::FromString(TEXT("게임의 난이도를 조정합니다.\n\n<Bold>쉬움:</> 스토리 경험에 집중할 수 있습니다. 가장 편안한 전투 난이도를 제공합니다.\n\n<Bold>보통:</> 약간 더 어려운 전투 경험을 제공합니다.\n\n<Bold>어려움:</> 훨씬 더 도전적인 전투 경험을 제공합니다.\n\n<Bold>매우 어려움:</> 가장 높은 수준의 도전적인 전투 경험을 제공합니다. 첫 플레이에는 권장되지 않습니다.")));
		GameDifficulty->AddDynamicOption(TEXT("Easy"), FText::FromString(TEXT("쉬움")));
		GameDifficulty->AddDynamicOption(TEXT("Normal"), FText::FromString(TEXT("보통")));
		GameDifficulty->AddDynamicOption(TEXT("Hard"), FText::FromString(TEXT("어려움")));
		GameDifficulty->AddDynamicOption(TEXT("Very Hard"), FText::FromString(TEXT("매우 어려움")));
		GameDifficulty->SetDefaultValueFromString(TEXT("Normal"));
		GameDifficulty->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetCurrentGameDifficulty));
		GameDifficulty->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetCurrentGameDifficulty));
		GameDifficulty->SetShouldApplySettingsImmediately(true);

		GameplayTabCollection->AddChildListData(GameDifficulty);
	}

	{
		UListDataObject_String* TestItem = NewObject<UListDataObject_String>();
		TestItem->SetDataID(FName("TestItem"));
		TestItem->SetDataDisplayName(FText::FromString(TEXT("Test Image Item")));
		TestItem->SetSoftDescriptionImage(UFrontendFunctionLibrary::GetOptionsSoftImageByTag(FrontendGameplayTags::Frontend_Image_TestImage));
		TestItem->SetDescriptionRichText(FText::FromString(TEXT("표시할 이미지는 프로젝트 설정에서 지정할 수 있습니다. 개발자가 할당한 이미지는 무엇이든 될 수 있습니다")));

		GameplayTabCollection->AddChildListData(TestItem);
	}

	RegisteredOptionsTabCollections.Add(GameplayTabCollection);
}

void UOptionsDataRegistry::InitAudioCollectionTab()
{
	UListDataObject_Collection* AudioTabCollection = NewObject<UListDataObject_Collection>();
	AudioTabCollection->SetDataID(FName("AudioTabCollection"));
	AudioTabCollection->SetDataDisplayName(FText::FromString(TEXT("오디오")));

	RegisteredOptionsTabCollections.Add(AudioTabCollection);
}

void UOptionsDataRegistry::InitVideoCollectionTab()
{
	UListDataObject_Collection* VideoTabCollection = NewObject<UListDataObject_Collection>();
	VideoTabCollection->SetDataID(FName("VideoTabCollection"));
	VideoTabCollection->SetDataDisplayName(FText::FromString(TEXT("그래픽")));

	RegisteredOptionsTabCollections.Add(VideoTabCollection);
}

void UOptionsDataRegistry::InitControlCollectionTab()
{
	UListDataObject_Collection* ControlTabCollection = NewObject<UListDataObject_Collection>();
	ControlTabCollection->SetDataID(FName("ControlTabCollection"));
	ControlTabCollection->SetDataDisplayName(FText::FromString(TEXT("조작")));

	RegisteredOptionsTabCollections.Add(ControlTabCollection);
}