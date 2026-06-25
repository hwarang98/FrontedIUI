// LeeHwaRang All Rights Reserved


#include "Widgets/Options/OptionsDataRegistry.h"
#include "FrontendSettings/FrontendGameUserSettings.h"
#include "Widgets/Options/OptionsDataInteractionHelper.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"
#include "FrontendFunctionLibrary.h"
#include "FrontendGameplayTags.h"
#include "Widgets/Options/DataObjects/ListDataObject_Scalar.h"
#include "Widgets/Options/DataObjects/ListDataObject_StringBool.h"
#include "Widgets/Options/DataObjects/ListDataObject_StringEnum.h"
#include "Widgets/Options/DataObjects/ListDataObject_StringInteger.h"
#include "Widgets/Options/DataObjects/ListDataObject_StringResolution.h"
#include "Internationalization/StringTableRegistry.h"
#include "EnhancedInputSubsystems.h"
#include "FrontendDebugHelper.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Widgets/Options/DataObjects/ListDataObject_KeyRemap.h"
#include "Widgets/Options/DataObjects/ListDataObject_Action.h"
#include "Subsystems/FrontendUISubsystem.h"
#include "FrontendTypes/FrontendEnum.h"
#include "Kismet/KismetSystemLibrary.h"


#define MAKE_OPTIONS_DATA_CONTROL(SetterOrGetterFunctionName) \
	MakeShared<FOptionsDataInteractionHelper>(GET_FUNCTION_NAME_STRING_CHECKED(UFrontendGameUserSettings, SetterOrGetterFunctionName))

#define GET_DESCRIPTION(InKey) LOCTABLE("/Game/UI/StringTables/ST_OptionsScreenDescription.ST_OptionsScreenDescription", InKey)

void UOptionsDataRegistry::InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer)
{
	// 각 카테고리 탭을 순서대로 생성합니다. 탭 표시 순서는 이 호출 순서를 따릅니다.
	// InitGameplayCollectionTab();
	InitAudioCollectionTab();
	InitVideoCollectionTab();
	InitControlCollectionTab(InOwningLocalPlayer);
	InitSystemCollectionTab(InOwningLocalPlayer);
}

TArray<UListDataObject_Base*> UOptionsDataRegistry::GetListSourceItemsBySelectedTabID(const FName& InSelectedTabID)
{
	UListDataObject_Collection* const* FoundTabCollectionPtr = RegisteredOptionsTabCollections.FindByPredicate(
		[InSelectedTabID](const UListDataObject_Collection* AvailableTabCollection)-> bool {
			return AvailableTabCollection->GetDataID() == InSelectedTabID;
		});

	checkf(FoundTabCollectionPtr, TEXT("%s 에 해당하는 유효한 Tab ID를 찾을 수 없습니다."), *InSelectedTabID.ToString());

	const UListDataObject_Collection* FoundTabCollection = *FoundTabCollectionPtr;

	TArray<UListDataObject_Base*> AllChildListItems;

	for (UListDataObject_Base* ChildListData : FoundTabCollection->GetAllChildListData())
	{
		if (!ChildListData)
		{
			continue;
		}

		AllChildListItems.Add(ChildListData);

		if (ChildListData->HasAnyChildListData())
		{
			FindChildListDataRecursively(ChildListData, AllChildListItems);
		}
	}

	return AllChildListItems;

}

void UOptionsDataRegistry::FindChildListDataRecursively(UListDataObject_Base* InParentData, TArray<UListDataObject_Base*>& OutFoundChildListData) const
{
	if (!InParentData || !InParentData->HasAnyChildListData())
	{
		return;
	}

	for (UListDataObject_Base* SubChildListData : InParentData->GetAllChildListData())
	{
		if (!SubChildListData)
		{
			continue;
		}

		OutFoundChildListData.Add(SubChildListData);

		if (SubChildListData->HasAnyChildListData())
		{
			FindChildListDataRecursively(SubChildListData, OutFoundChildListData);
		}
	}
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

	#pragma region Volume Category
	UListDataObject_Collection* VolumeCategoryCollection = NewObject<UListDataObject_Collection>();
	VolumeCategoryCollection->SetDataID(FName("VolumeCategoryCollection"));
	VolumeCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("볼륨")));

	AudioTabCollection->AddChildListData(VolumeCategoryCollection);

	#pragma region Overall Volume
	UListDataObject_Scalar* OverallVolume = NewObject<UListDataObject_Scalar>();
	OverallVolume->SetDataID(FName("OverallVolume"));
	OverallVolume->SetDataDisplayName(FText::FromString(TEXT("전체 볼륨")));
	OverallVolume->SetDisabledRichText(FText::FromString(TEXT("전체 볼륨에 대한 설명입니다")));
	OverallVolume->SetDisplayValueRange(TRange<float>(0.f, 1.f));
	OverallVolume->SetOutputValueRange(TRange<float>(0.f, 2.f));
	OverallVolume->SetSliderStepSize(0.01f);
	OverallVolume->SetDefaultValueFromString(LexToString(1.f));
	OverallVolume->SetDisplayNumericType(ECommonNumericType::Percentage);
	OverallVolume->SetNumberFormattingOptions(UListDataObject_Scalar::NoDecimals()); // 소수점 없으면 50%, 한자리 소수점이면 50.5%
	OverallVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetOverallVolume));
	OverallVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetOverallVolume));
	OverallVolume->SetShouldApplySettingsImmediately(true);

	VolumeCategoryCollection->AddChildListData(OverallVolume);
	#pragma endregion

	#pragma region Music Volume
	UListDataObject_Scalar* MusicVolume = NewObject<UListDataObject_Scalar>();
	MusicVolume->SetDataID(FName("MusicVolume"));
	MusicVolume->SetDataDisplayName(FText::FromString(TEXT("음악 볼륨")));
	MusicVolume->SetDescriptionRichText(FText::FromString(TEXT("음악 볼륨에 대한 설명입니다.")));
	MusicVolume->SetDisplayValueRange(TRange<float>(0.f, 1.f));
	MusicVolume->SetOutputValueRange(TRange<float>(0.f, 2.f));
	MusicVolume->SetSliderStepSize(0.01f);
	MusicVolume->SetDefaultValueFromString(LexToString(1.f));
	MusicVolume->SetDisplayNumericType(ECommonNumericType::Percentage);
	MusicVolume->SetNumberFormattingOptions(UListDataObject_Scalar::NoDecimals()); //No Decimal: 50%, One Decimal: 50.5%
	MusicVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetMusicVolume));
	MusicVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetMusicVolume));
	MusicVolume->SetShouldApplySettingsImmediately(true);

	VolumeCategoryCollection->AddChildListData(MusicVolume);
	#pragma endregion

	#pragma region SoundFX Volume
	UListDataObject_Scalar* SoundFXVolume = NewObject<UListDataObject_Scalar>();
	SoundFXVolume->SetDataID(FName("SoundFXVolume"));
	SoundFXVolume->SetDataDisplayName(FText::FromString(TEXT("이팩트 사운드 볼륨")));
	SoundFXVolume->SetDescriptionRichText(FText::FromString(TEXT("이팩트 사운드 볼륨에 대한 설명입니다.")));
	SoundFXVolume->SetDisplayValueRange(TRange<float>(0.f, 1.f));
	SoundFXVolume->SetOutputValueRange(TRange<float>(0.f, 2.f));
	SoundFXVolume->SetSliderStepSize(0.01f);
	SoundFXVolume->SetDefaultValueFromString(LexToString(1.f));
	SoundFXVolume->SetDisplayNumericType(ECommonNumericType::Percentage);
	SoundFXVolume->SetNumberFormattingOptions(UListDataObject_Scalar::NoDecimals()); //No Decimal: 50%, One Decimal: 50.5%
	SoundFXVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetSoundFXVolume));
	SoundFXVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetSoundFXVolume));
	SoundFXVolume->SetShouldApplySettingsImmediately(true);

	VolumeCategoryCollection->AddChildListData(SoundFXVolume);
	#pragma endregion
	#pragma endregion

	#pragma region Sound Category
	UListDataObject_Collection* SoundCategoryCollection = NewObject<UListDataObject_Collection>();
	SoundCategoryCollection->SetDataID(FName("SoundCategoryCollection"));
	SoundCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("사운드")));

	AudioTabCollection->AddChildListData(SoundCategoryCollection);

	#pragma region Allow Background Audio
	UListDataObject_StringBool* AllowBackgroundAudio = NewObject<UListDataObject_StringBool>();
	AllowBackgroundAudio->SetDataID(FName("AllowBackgroundAudio"));
	AllowBackgroundAudio->SetDataDisplayName(FText::FromString(TEXT("백그라운드 소리 허용")));
	AllowBackgroundAudio->OverrideTrueDisplayText(FText::FromString(TEXT("활성화")));
	AllowBackgroundAudio->OverrideFalseDisplayText(FText::FromString(TEXT("비활성화")));
	AllowBackgroundAudio->SetFalseAsDefaultValue();
	AllowBackgroundAudio->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetAllowBackGroundAudio));
	AllowBackgroundAudio->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetAllowBackGroundAudio));
	AllowBackgroundAudio->SetShouldApplySettingsImmediately(true);

	SoundCategoryCollection->AddChildListData(AllowBackgroundAudio);
	#pragma endregion

	#pragma region Use HDR Audio
	UListDataObject_StringBool* UseHDRAudioMode = NewObject<UListDataObject_StringBool>();
	UseHDRAudioMode->SetDataID(FName("UseHDRAudioMode"));
	UseHDRAudioMode->SetDataDisplayName(FText::FromString(TEXT("HDR 오디오 모드")));
	UseHDRAudioMode->OverrideTrueDisplayText(FText::FromString(TEXT("활성화")));
	UseHDRAudioMode->OverrideFalseDisplayText(FText::FromString(TEXT("비활성화")));
	UseHDRAudioMode->SetFalseAsDefaultValue();
	UseHDRAudioMode->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetUseHDRAudioMode));
	UseHDRAudioMode->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetUseHDRAudioMode));
	UseHDRAudioMode->SetShouldApplySettingsImmediately(true);

	SoundCategoryCollection->AddChildListData(UseHDRAudioMode);
	#pragma endregion

	#pragma endregion

	RegisteredOptionsTabCollections.Add(AudioTabCollection);
}

void UOptionsDataRegistry::InitVideoCollectionTab()
{
	UListDataObject_Collection* VideoTabCollection = NewObject<UListDataObject_Collection>();
	VideoTabCollection->SetDataID(FName("VideoTabCollection"));
	VideoTabCollection->SetDataDisplayName(FText::FromString(TEXT("그래픽")));

	UListDataObject_StringEnum* CreatedWindowMode = nullptr;

	#pragma region Display Category
	UListDataObject_Collection* DisplayCategoryCollection = NewObject<UListDataObject_Collection>();
	DisplayCategoryCollection->SetDataID(FName("DisplayCategoryCollection"));
	DisplayCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("디스플레이")));

	VideoTabCollection->AddChildListData(DisplayCategoryCollection);

	FOptionsDataEditConditionDescriptor PackageBuildOnlyCondition;
	PackageBuildOnlyCondition.SetEditConditionFunction([]()-> bool {
		const bool bIsInEditor = GIsEditor || GIsPlayInEditorWorld;

		return !bIsInEditor;
	});

	PackageBuildOnlyCondition.SetDisabledRichReason(TEXT("\n\n<Disabled>이 설정은 패키지 빌드에서만 조정할 수 있습니다.</>"));

	#pragma region Window Mode
	UListDataObject_StringEnum* WindowMode = NewObject<UListDataObject_StringEnum>();
	WindowMode->SetDataID(FName("WindowMode"));
	WindowMode->SetDataDisplayName(FText::FromString(TEXT("창 모드")));
	WindowMode->SetDescriptionRichText(GET_DESCRIPTION("WindowModeDescKey"));
	WindowMode->AddEnumOption(EWindowMode::Fullscreen, FText::FromString(TEXT("전체 화면")));
	WindowMode->AddEnumOption(EWindowMode::WindowedFullscreen, FText::FromString(TEXT("테두리없는 창 모드")));
	WindowMode->AddEnumOption(EWindowMode::Windowed, FText::FromString(TEXT("창 모드")));
	WindowMode->SetDefaultValueFromEnumOption(EWindowMode::WindowedFullscreen);
	WindowMode->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetFullscreenMode));
	WindowMode->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetFullscreenMode));
	WindowMode->SetShouldApplySettingsImmediately(true);

	CreatedWindowMode = WindowMode;

	WindowMode->AddEditCondition(PackageBuildOnlyCondition);

	DisplayCategoryCollection->AddChildListData(WindowMode);
	#pragma endregion

	#pragma region Screen Resolution
	UListDataObject_StringResolution* ScreenResolution = NewObject<UListDataObject_StringResolution>();
	ScreenResolution->SetDataID(FName("ScreenResolution"));
	ScreenResolution->SetDataDisplayName(FText::FromString(TEXT("화면 해상도")));
	ScreenResolution->SetDescriptionRichText(GET_DESCRIPTION("ScreenResolutionsDescKey"));
	ScreenResolution->InitResolutionValues();
	ScreenResolution->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetScreenResolution));
	ScreenResolution->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetScreenResolution));
	ScreenResolution->SetShouldApplySettingsImmediately(true);

	ScreenResolution->AddEditCondition(PackageBuildOnlyCondition);

	FOptionsDataEditConditionDescriptor WindowModeEditCondition;
	WindowModeEditCondition.SetEditConditionFunction([CreatedWindowMode]()-> bool {
			const bool bIsBorderlessWindow = CreatedWindowMode->GetCurrentValueAsEnum<EWindowMode::Type>() == EWindowMode::WindowedFullscreen;
			return !bIsBorderlessWindow;
		}
		);
	WindowModeEditCondition.SetDisabledRichReason(TEXT("\n\n<Disabled>'창 모드'를 경계 없는 창으로 설정하면 화면 해상도를 조정할 수 없습니다. 이 값은 최대 허용 해상도와 일치해야 합니다.</>"));
	WindowModeEditCondition.SetDisabledForcedStringValue(ScreenResolution->GetMaximumAllowedResolution());

	DisplayCategoryCollection->AddChildListData(ScreenResolution);

	ScreenResolution->AddEditDependencyData(CreatedWindowMode);

	ScreenResolution->AddEditCondition(WindowModeEditCondition);
	#pragma endregion Resolution
	#pragma endregion

	#pragma region Graphics Category
	UListDataObject_Collection* GraphicsCategoryCollection = NewObject<UListDataObject_Collection>();
	GraphicsCategoryCollection->SetDataID(FName("GraphicsCategoryCollection"));
	GraphicsCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("그래픽")));

	VideoTabCollection->AddChildListData(GraphicsCategoryCollection);

	#pragma region Display Gamma
	UListDataObject_Scalar* DisplayGamma = NewObject<UListDataObject_Scalar>();
	DisplayGamma->SetDataID(FName("DisplayGamma"));
	DisplayGamma->SetDataDisplayName(FText::FromString(TEXT("밝기")));
	DisplayGamma->SetDescriptionRichText(GET_DESCRIPTION("DisplayGammaDescKey"));
	DisplayGamma->SetDisplayValueRange(TRange<float>(0.f, 1.f));
	DisplayGamma->SetOutputValueRange(TRange<float>(1.7f, 2.7f));
	DisplayGamma->SetSliderStepSize(0.01f);
	DisplayGamma->SetDisplayNumericType(ECommonNumericType::Percentage);
	DisplayGamma->SetNumberFormattingOptions(UListDataObject_Scalar::NoDecimals());
	DisplayGamma->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetCurrentDisplayGamma));
	DisplayGamma->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetCurrentDisplayGamma));
	DisplayGamma->SetDefaultValueFromString(LexToString(2.2f));

	GraphicsCategoryCollection->AddChildListData(DisplayGamma);

	#pragma endregion

	UListDataObject_StringInteger* CreatedOverallQuality = nullptr;

	#pragma region Overall Quality
	UListDataObject_StringInteger* OverallQuality = NewObject<UListDataObject_StringInteger>();
	OverallQuality->SetDataID(FName("OverallQuality"));
	OverallQuality->SetDataDisplayName(FText::FromString(TEXT("전체 품질")));
	OverallQuality->SetDescriptionRichText(GET_DESCRIPTION("OverallQualityDescKey"));
	OverallQuality->AddIntegerOption(0, FText::FromString(TEXT("낮음")));
	OverallQuality->AddIntegerOption(1, FText::FromString(TEXT("기본")));
	OverallQuality->AddIntegerOption(2, FText::FromString(TEXT("높음")));
	OverallQuality->AddIntegerOption(3, FText::FromString(TEXT("매우높음")));
	OverallQuality->AddIntegerOption(4, FText::FromString(TEXT("시네마틱")));
	OverallQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetOverallScalabilityLevel));
	OverallQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetOverallScalabilityLevel));
	OverallQuality->SetShouldApplySettingsImmediately(true);

	GraphicsCategoryCollection->AddChildListData(OverallQuality);

	CreatedOverallQuality = OverallQuality;
	#pragma endregion

	#pragma region Resolution Scale
	UListDataObject_Scalar* ResolutionScale = NewObject<UListDataObject_Scalar>();
	ResolutionScale->SetDataID(FName("ResolutionScale"));
	ResolutionScale->SetDataDisplayName(FText::FromString(TEXT("해상도")));
	ResolutionScale->SetDescriptionRichText(FText::FromString(TEXT("해상도에 대한 설명입니다.")));
	ResolutionScale->SetDisplayValueRange(TRange<float>(0.f, 1.f));
	ResolutionScale->SetOutputValueRange(TRange<float>(0.f, 1.f));
	ResolutionScale->SetSliderStepSize(0.01f);
	ResolutionScale->SetDisplayNumericType(ECommonNumericType::Percentage);
	ResolutionScale->SetNumberFormattingOptions(UListDataObject_Scalar::NoDecimals());
	ResolutionScale->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetResolutionScaleNormalized));
	ResolutionScale->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetResolutionScaleNormalized));
	ResolutionScale->SetShouldApplySettingsImmediately(true);

	ResolutionScale->AddEditDependencyData(CreatedOverallQuality);

	GraphicsCategoryCollection->AddChildListData(ResolutionScale);
	#pragma endregion

	#pragma region Global Illumination Quality
	UListDataObject_StringInteger* GlobalIlluminationQuality = NewObject<UListDataObject_StringInteger>();
	GlobalIlluminationQuality->SetDataID(FName("GlobalIlluminationQuality"));
	GlobalIlluminationQuality->SetDataDisplayName(FText::FromString(TEXT("글로벌 일루미네이션")));
	GlobalIlluminationQuality->SetDescriptionRichText(GET_DESCRIPTION("GlobalIlluminationQualityDescKey"));
	GlobalIlluminationQuality->AddIntegerOption(0, FText::FromString(TEXT("낮음")));
	GlobalIlluminationQuality->AddIntegerOption(1, FText::FromString(TEXT("기본")));
	GlobalIlluminationQuality->AddIntegerOption(2, FText::FromString(TEXT("높음")));
	GlobalIlluminationQuality->AddIntegerOption(3, FText::FromString(TEXT("매우 높음")));
	GlobalIlluminationQuality->AddIntegerOption(4, FText::FromString(TEXT("시네마틱")));
	GlobalIlluminationQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetGlobalIlluminationQuality));
	GlobalIlluminationQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetGlobalIlluminationQuality));
	GlobalIlluminationQuality->SetShouldApplySettingsImmediately(true);

	GlobalIlluminationQuality->AddEditDependencyData(CreatedOverallQuality);

	CreatedOverallQuality->AddEditDependencyData(GlobalIlluminationQuality);

	GraphicsCategoryCollection->AddChildListData(GlobalIlluminationQuality);
	#pragma endregion

	#pragma region Shadow Quality
	UListDataObject_StringInteger* ShadowQuality = NewObject<UListDataObject_StringInteger>();
	ShadowQuality->SetDataID(FName("ShadowQuality"));
	ShadowQuality->SetDataDisplayName(FText::FromString(TEXT("그림자 품질")));
	ShadowQuality->SetDescriptionRichText(GET_DESCRIPTION("ShadowQualityDescKey"));
	ShadowQuality->AddIntegerOption(0, FText::FromString(TEXT("낮음")));
	ShadowQuality->AddIntegerOption(1, FText::FromString(TEXT("기본")));
	ShadowQuality->AddIntegerOption(2, FText::FromString(TEXT("높음")));
	ShadowQuality->AddIntegerOption(3, FText::FromString(TEXT("매우 높음")));
	ShadowQuality->AddIntegerOption(4, FText::FromString(TEXT("시네마틱")));
	ShadowQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetShadowQuality));
	ShadowQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetShadowQuality));
	ShadowQuality->SetShouldApplySettingsImmediately(true);

	ShadowQuality->AddEditDependencyData(CreatedOverallQuality);

	CreatedOverallQuality->AddEditDependencyData(ShadowQuality);

	GraphicsCategoryCollection->AddChildListData(ShadowQuality);
	#pragma endregion

	#pragma region AntiAliasing Quality
	UListDataObject_StringInteger* ViewDistanceQuality = NewObject<UListDataObject_StringInteger>();
	ViewDistanceQuality->SetDataID(FName("ViewDistanceQuality"));
	ViewDistanceQuality->SetDataDisplayName(FText::FromString(TEXT("시야 거리")));
	ViewDistanceQuality->SetDescriptionRichText(GET_DESCRIPTION("ViewDistanceDescKey"));
	ViewDistanceQuality->AddIntegerOption(0, FText::FromString(TEXT("낮음")));
	ViewDistanceQuality->AddIntegerOption(1, FText::FromString(TEXT("기본")));
	ViewDistanceQuality->AddIntegerOption(2, FText::FromString(TEXT("높음")));
	ViewDistanceQuality->AddIntegerOption(3, FText::FromString(TEXT("매우 높음")));
	ViewDistanceQuality->AddIntegerOption(4, FText::FromString(TEXT("시네마틱")));
	ViewDistanceQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetViewDistanceQuality));
	ViewDistanceQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetViewDistanceQuality));
	ViewDistanceQuality->SetShouldApplySettingsImmediately(true);

	ViewDistanceQuality->AddEditDependencyData(CreatedOverallQuality);

	CreatedOverallQuality->AddEditDependencyData(ViewDistanceQuality);

	GraphicsCategoryCollection->AddChildListData(ViewDistanceQuality);
	#pragma endregion

	#pragma region Texture Quality
	UListDataObject_StringInteger* TextureQuality = NewObject<UListDataObject_StringInteger>();
	TextureQuality->SetDataID(FName("TextureQuality"));
	TextureQuality->SetDataDisplayName(FText::FromString(TEXT("텍스처 품질")));
	TextureQuality->SetDescriptionRichText(GET_DESCRIPTION("TextureQualityDescKey"));
	TextureQuality->AddIntegerOption(0, FText::FromString(TEXT("낮음")));
	TextureQuality->AddIntegerOption(1, FText::FromString(TEXT("기본")));
	TextureQuality->AddIntegerOption(2, FText::FromString(TEXT("높음")));
	TextureQuality->AddIntegerOption(3, FText::FromString(TEXT("매우 높음")));
	TextureQuality->AddIntegerOption(4, FText::FromString(TEXT("시네마틱")));
	TextureQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetTextureQuality));
	TextureQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetTextureQuality));
	TextureQuality->SetShouldApplySettingsImmediately(true);

	TextureQuality->AddEditDependencyData(CreatedOverallQuality);

	CreatedOverallQuality->AddEditDependencyData(TextureQuality);

	GraphicsCategoryCollection->AddChildListData(TextureQuality);
	#pragma endregion

	#pragma region Visual Effects Quality
	UListDataObject_StringInteger* VisualEffectQuality = NewObject<UListDataObject_StringInteger>();
	VisualEffectQuality->SetDataID(FName("VisualEffectQuality"));
	VisualEffectQuality->SetDataDisplayName(FText::FromString(TEXT("이팩트 품질")));
	VisualEffectQuality->SetDescriptionRichText(GET_DESCRIPTION("VisualEffectQualityDescKey"));
	VisualEffectQuality->AddIntegerOption(0, FText::FromString(TEXT("낮음")));
	VisualEffectQuality->AddIntegerOption(1, FText::FromString(TEXT("기본")));
	VisualEffectQuality->AddIntegerOption(2, FText::FromString(TEXT("높음")));
	VisualEffectQuality->AddIntegerOption(3, FText::FromString(TEXT("매우 높음")));
	VisualEffectQuality->AddIntegerOption(4, FText::FromString(TEXT("시네마틱")));
	VisualEffectQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetVisualEffectQuality));
	VisualEffectQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetVisualEffectQuality));
	VisualEffectQuality->SetShouldApplySettingsImmediately(true);

	VisualEffectQuality->AddEditDependencyData(CreatedOverallQuality);

	CreatedOverallQuality->AddEditDependencyData(VisualEffectQuality);

	GraphicsCategoryCollection->AddChildListData(VisualEffectQuality);
	#pragma endregion

	#pragma region Reflection Quality
	UListDataObject_StringInteger* ReflectionQuality = NewObject<UListDataObject_StringInteger>();
	ReflectionQuality->SetDataID(FName("ReflectionQuality"));
	ReflectionQuality->SetDataDisplayName(FText::FromString(TEXT("반사 품질")));
	ReflectionQuality->SetDescriptionRichText(GET_DESCRIPTION("ReflectionQualityDescKey"));
	ReflectionQuality->AddIntegerOption(0, FText::FromString(TEXT("낮음")));
	ReflectionQuality->AddIntegerOption(1, FText::FromString(TEXT("기본")));
	ReflectionQuality->AddIntegerOption(2, FText::FromString(TEXT("높음")));
	ReflectionQuality->AddIntegerOption(3, FText::FromString(TEXT("매우 높음")));
	ReflectionQuality->AddIntegerOption(4, FText::FromString(TEXT("시네마틱")));
	ReflectionQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetReflectionQuality));
	ReflectionQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetReflectionQuality));
	ReflectionQuality->SetShouldApplySettingsImmediately(true);

	ReflectionQuality->AddEditDependencyData(CreatedOverallQuality);

	CreatedOverallQuality->AddEditDependencyData(ReflectionQuality);

	GraphicsCategoryCollection->AddChildListData(ReflectionQuality);
	#pragma endregion

	#pragma region PostProcess Quality
	UListDataObject_StringInteger* PostProcessingQuality = NewObject<UListDataObject_StringInteger>();
	PostProcessingQuality->SetDataID(FName("PostProcessingQuality"));
	PostProcessingQuality->SetDataDisplayName(FText::FromString(TEXT("후처리 품질")));
	PostProcessingQuality->SetDescriptionRichText(GET_DESCRIPTION("PostProcessingQualityDescKey"));
	PostProcessingQuality->AddIntegerOption(0, FText::FromString(TEXT("낮음")));
	PostProcessingQuality->AddIntegerOption(1, FText::FromString(TEXT("기본")));
	PostProcessingQuality->AddIntegerOption(2, FText::FromString(TEXT("높음")));
	PostProcessingQuality->AddIntegerOption(3, FText::FromString(TEXT("매우 높음")));
	PostProcessingQuality->AddIntegerOption(4, FText::FromString(TEXT("시네마틱")));
	PostProcessingQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetPostProcessingQuality));
	PostProcessingQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetPostProcessingQuality));
	PostProcessingQuality->SetShouldApplySettingsImmediately(true);

	PostProcessingQuality->AddEditDependencyData(CreatedOverallQuality);

	CreatedOverallQuality->AddEditDependencyData(PostProcessingQuality);

	GraphicsCategoryCollection->AddChildListData(PostProcessingQuality);
	#pragma endregion

	#pragma region Advanced Graphics Category
	#pragma endregion
	UListDataObject_Collection* AdvancedGraphicsCategoryCollection = NewObject<UListDataObject_Collection>();
	AdvancedGraphicsCategoryCollection->SetDataID(FName("AdvancedGraphicsCategoryCollection"));
	AdvancedGraphicsCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("고급 그래픽")));

	VideoTabCollection->AddChildListData(AdvancedGraphicsCategoryCollection);

	#pragma region Vertical Sync
	UListDataObject_StringBool* VerticalSync = NewObject<UListDataObject_StringBool>();
	VerticalSync->SetDataID(FName("VerticalSync"));
	VerticalSync->SetDataDisplayName(FText::FromString(TEXT("수직 동기화")));
	VerticalSync->SetDescriptionRichText(GET_DESCRIPTION("VerticalSyncDescKey"));
	VerticalSync->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(IsVSyncEnabled));
	VerticalSync->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetVSyncEnabled));
	VerticalSync->SetFalseAsDefaultValue();
	VerticalSync->SetShouldApplySettingsImmediately(true);

	FOptionsDataEditConditionDescriptor FullscreenOnlyCondition;
	FullscreenOnlyCondition.SetEditConditionFunction(
		[CreatedWindowMode]()-> bool {
			return CreatedWindowMode->GetCurrentValueAsEnum<EWindowMode::Type>() == EWindowMode::Fullscreen;
		}
		);
	FullscreenOnlyCondition.SetDisabledRichReason(TEXT("\n\n<Disabled>'화면 모드'를 '전체 화면'으로 설정해야 사용할 수 있습니다.</>"));
	FullscreenOnlyCondition.SetDisabledForcedStringValue(TEXT("false"));

	VerticalSync->AddEditCondition(FullscreenOnlyCondition);

	AdvancedGraphicsCategoryCollection->AddChildListData(VerticalSync);
	#pragma endregion

	#pragma region Frame Rate Limit
	UListDataObject_String* FrameRateLimit = NewObject<UListDataObject_String>();
	FrameRateLimit->SetDataID(FName("FrameRateLimit"));
	FrameRateLimit->SetDataDisplayName(FText::FromString(TEXT("프레임 제한")));
	FrameRateLimit->SetDescriptionRichText(GET_DESCRIPTION("FrameRateLimitDescKey"));
	FrameRateLimit->AddDynamicOption(LexToString(30.f), FText::FromString(TEXT("30 FPS")));
	FrameRateLimit->AddDynamicOption(LexToString(60.f), FText::FromString(TEXT("60 FPS")));
	FrameRateLimit->AddDynamicOption(LexToString(90.f), FText::FromString(TEXT("90 FPS")));
	FrameRateLimit->AddDynamicOption(LexToString(120.f), FText::FromString(TEXT("120 FPS")));
	FrameRateLimit->AddDynamicOption(LexToString(165.f), FText::FromString(TEXT("165 FPS")));
	FrameRateLimit->AddDynamicOption(LexToString(240.f), FText::FromString(TEXT("240 FPS")));
	FrameRateLimit->AddDynamicOption(LexToString(0.f), FText::FromString(TEXT("제한 없음")));
	FrameRateLimit->SetDefaultValueFromString(LexToString(0.f));
	FrameRateLimit->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetFrameRateLimit));
	FrameRateLimit->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetFrameRateLimit));
	FrameRateLimit->SetShouldApplySettingsImmediately(true);

	AdvancedGraphicsCategoryCollection->AddChildListData(FrameRateLimit);
	#pragma endregion

	#pragma endregion

	RegisteredOptionsTabCollections.Add(VideoTabCollection);
}

void UOptionsDataRegistry::InitControlCollectionTab(ULocalPlayer* InOwningLocalPlayer)
{
	UListDataObject_Collection* ControlTabCollection = NewObject<UListDataObject_Collection>();
	ControlTabCollection->SetDataID(FName("ControlTabCollection"));
	ControlTabCollection->SetDataDisplayName(FText::FromString(TEXT("조작")));

	UEnhancedInputLocalPlayerSubsystem* EISubSystem = InOwningLocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	check(EISubSystem);

	UEnhancedInputUserSettings* EIUserSettings = EISubSystem->GetUserSettings();

	check(EIUserSettings);

	#pragma region Keyboard Mouse Category
	UListDataObject_Collection* KeyboardMouseCategoryCollection = NewObject<UListDataObject_Collection>();
	KeyboardMouseCategoryCollection->SetDataID(FName("KeyboardMouseCategoryCollection"));
	KeyboardMouseCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("키보드 & 마우스")));

	ControlTabCollection->AddChildListData(KeyboardMouseCategoryCollection);

	#pragma region Keyboard Mouse Input

	#pragma endregion
	FPlayerMappableKeyQueryOptions KeyboardMouseOnly;
	KeyboardMouseOnly.KeyToMatch = EKeys::S;
	KeyboardMouseOnly.bMatchBasicKeyTypes = true;

	// FPlayerMappableKeyQueryOptions GamepadOnly;
	// GamepadOnly.KeyToMatch = EKeys::Gamepad_FaceButton_Bottom;
	// GamepadOnly.bMatchBasicKeyTypes = true;

	for (const TPair<FString, TObjectPtr<UEnhancedPlayerMappableKeyProfile>>& ProfilePair : EIUserSettings->GetAllAvailableKeyProfiles())
	{
		UEnhancedPlayerMappableKeyProfile* MappableKeyProfile = ProfilePair.Value;

		check(MappableKeyProfile);

		for (const TPair<FName, FKeyMappingRow>& MappingRowPair : MappableKeyProfile->GetPlayerMappingRows())
		{
			for (const FPlayerKeyMapping& KeyMapping : MappingRowPair.Value.Mappings)
			{
				if (MappableKeyProfile->DoesMappingPassQueryOptions(KeyMapping, KeyboardMouseOnly))
				{
					UListDataObject_KeyRemap* KeyRemapDataObject = NewObject<UListDataObject_KeyRemap>();
					KeyRemapDataObject->SetDataID(KeyMapping.GetMappingName());
					KeyRemapDataObject->SetDataDisplayName(KeyMapping.GetDisplayName());
					KeyRemapDataObject->InitKeyRemapData(EIUserSettings, MappableKeyProfile, ECommonInputType::MouseAndKeyboard, KeyMapping);

					KeyboardMouseCategoryCollection->AddChildListData(KeyRemapDataObject);
				}

			}
		}
	}

	#pragma endregion

	#pragma region Gamepad Category
	UListDataObject_Collection* GamepadCategoryCollection = NewObject<UListDataObject_Collection>();
	GamepadCategoryCollection->SetDataID(FName("GamepadCategoryCollection"));
	GamepadCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("게임패드")));

	ControlTabCollection->AddChildListData(GamepadCategoryCollection);

	#pragma region Gamepad Input
	FPlayerMappableKeyQueryOptions GamepadOnly;
	GamepadOnly.KeyToMatch = EKeys::Gamepad_FaceButton_Bottom;
	GamepadOnly.bMatchBasicKeyTypes = true;

	for (const TPair<FString, TObjectPtr<UEnhancedPlayerMappableKeyProfile>>& ProfilePair : EIUserSettings->GetAllAvailableKeyProfiles())
	{
		UEnhancedPlayerMappableKeyProfile* MappableKeyProfile = ProfilePair.Value;

		check(MappableKeyProfile);

		for (const TPair<FName, FKeyMappingRow>& MappingRowPair : MappableKeyProfile->GetPlayerMappingRows())
		{
			for (const FPlayerKeyMapping& KeyMapping : MappingRowPair.Value.Mappings)
			{
				if (MappableKeyProfile->DoesMappingPassQueryOptions(KeyMapping, GamepadOnly))
				{
					UListDataObject_KeyRemap* KeyRemapDataObject = NewObject<UListDataObject_KeyRemap>();
					KeyRemapDataObject->SetDataID(KeyMapping.GetMappingName());
					KeyRemapDataObject->SetDataDisplayName(KeyMapping.GetDisplayName());
					KeyRemapDataObject->InitKeyRemapData(EIUserSettings, MappableKeyProfile, ECommonInputType::Gamepad, KeyMapping);

					GamepadCategoryCollection->AddChildListData(KeyRemapDataObject);
				}
			}
		}
	}
	#pragma endregion

	#pragma endregion

	RegisteredOptionsTabCollections.Add(ControlTabCollection);
}

void UOptionsDataRegistry::InitSystemCollectionTab(ULocalPlayer* InOwningLocalPlayer)
{
	UListDataObject_Collection* SystemTabCollection = NewObject<UListDataObject_Collection>();
	SystemTabCollection->SetDataID(FName("SystemTabCollection"));
	SystemTabCollection->SetDataDisplayName(FText::FromString(TEXT("시스템")));

	TWeakObjectPtr<ULocalPlayer> WeakLocalPlayer = InOwningLocalPlayer;

	{
		UListDataObject_Action* ReturnToTitle = NewObject<UListDataObject_Action>();
		ReturnToTitle->SetDataID(FName("ReturnToTitle"));
		ReturnToTitle->SetDataDisplayName(FText::FromString(TEXT("타이틀로 돌아가기")));
		ReturnToTitle->SetDescriptionRichText(FText::FromString(TEXT("메인 메뉴로 돌아갑니다.\n\n현재 게임 진행 내용은 유지되지 않습니다.")));
		ReturnToTitle->SetAction([WeakLocalPlayer]() {
			ULocalPlayer* LocalPlayer = WeakLocalPlayer.Get();
			if (!LocalPlayer)
			{
				return;
			}

			UFrontendUISubsystem* UISubsystem = UFrontendUISubsystem::Get(LocalPlayer);
			if (!UISubsystem)
			{
				return;
			}

			UISubsystem->PushConfirmScreenToModalStackAsync(
				EConfirmScreenType::YesNo,
				FText::FromString(TEXT("타이틀로 돌아가기")),
				FText::FromString(TEXT("타이틀 화면으로 돌아가시겠습니까?")),
				[WeakLocalPlayer](EConfirmScreenButtonType ClickedButtonType) {
					if (ClickedButtonType != EConfirmScreenButtonType::Confirmed)
					{
						return;
					}

					UFrontendUISubsystem* ConfirmedUISubsystem = UFrontendUISubsystem::Get(WeakLocalPlayer.Get());
					if (ConfirmedUISubsystem)
					{
						ConfirmedUISubsystem->ReturnToFrontend();
					}
				}
				);
		});

		SystemTabCollection->AddChildListData(ReturnToTitle);
	}

	{
		UListDataObject_Action* QuitGame = NewObject<UListDataObject_Action>();
		QuitGame->SetDataID(FName("QuitGame"));
		QuitGame->SetDataDisplayName(FText::FromString(TEXT("게임 종료")));
		QuitGame->SetDescriptionRichText(FText::FromString(TEXT("게임을 종료합니다.\n\n현재 진행 상황은 저장되지 않으며 모두 사라집니다.")));
		QuitGame->SetAction([WeakLocalPlayer]() {
			ULocalPlayer* LocalPlayer = WeakLocalPlayer.Get();
			if (!LocalPlayer)
			{
				return;
			}

			UFrontendUISubsystem* UISubsystem = UFrontendUISubsystem::Get(LocalPlayer);
			if (!UISubsystem)
			{
				return;
			}

			UISubsystem->PushConfirmScreenToModalStackAsync(
				EConfirmScreenType::YesNo,
				FText::FromString(TEXT("게임 종료")),
				FText::FromString(TEXT("게임을 종료하시겠습니까?")),
				[WeakLocalPlayer](EConfirmScreenButtonType ClickedButtonType) {
					if (ClickedButtonType != EConfirmScreenButtonType::Confirmed)
					{
						return;
					}

					ULocalPlayer* LocalPlayer = WeakLocalPlayer.Get();
					if (!LocalPlayer)
					{
						return;
					}

					APlayerController* PC = LocalPlayer->GetPlayerController(LocalPlayer->GetWorld());
					UKismetSystemLibrary::QuitGame(LocalPlayer->GetWorld(), PC, EQuitPreference::Quit, false);
				}
				);
		});

		SystemTabCollection->AddChildListData(QuitGame);
	}

	RegisteredOptionsTabCollections.Add(SystemTabCollection);
}