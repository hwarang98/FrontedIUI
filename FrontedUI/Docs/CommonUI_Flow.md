# Frontend UI 시스템 흐름 정리

Common UI 플러그인 기반으로 구성된 Frontend UI 시스템의 전체 구조와 흐름입니다.

---

## 클래스 구조 한눈에 보기

```
APlayerController
└── AFrontendPlayerController          ← 카메라 설정

UGameInstanceSubsystem
└── UFrontendUISubsystem               ← UI 중앙 관리자 (GameInstance 수명)

UCommonUserWidget
└── UWidget_PrimaryLayout              ← 모든 위젯 스택의 루트 컨테이너

UCommonActivatableWidget
└── UWidget_ActivatableBase            ← 모든 화면 위젯의 베이스
    └── UWidget_OptionsScreen          ← 옵션 화면 (탭 + 리스트 + 디테일)

UBlueprintAsyncActionBase
└── UAsyncAction_PushSoftWidget        ← 블루프린트 비동기 푸시 노드

UBlueprintFunctionLibrary
└── UFrontendFunctionLibrary           ← 태그로 위젯 클래스 조회

UDeveloperSettings
└── UFrontendDeveloperSettings         ← 에디터에서 태그↔위젯 클래스 등록

UObject
└── UOptionsDataRegistry               ← 탭 컬렉션 생성/관리 (옵션 전용)

UObject (Abstract)
└── UListDataObject_Base               ← 리스트 항목 데이터 베이스
    ├── UListDataObject_Collection     ← 탭 (자식 항목 보유)
    └── UListDataObject_Value (Abstract)
        └── UListDataObject_String     ← 문자열 선택형 옵션 항목

UCommonUserWidget (Abstract)
└── UWidget_ListEntry_Base             ← 리스트 항목 위젯 베이스
    └── UWidget_ListEntry_String       ← 문자열 선택형 엔트리 위젯

UCommonListView
└── UFrontendCommonListView            ← DataListEntryMapping 기반 자동 위젯 선택

UCommonTabListWidgetBase
└── UFrontendTabListWidgetBase         ← 탭 목록 위젯

UDataAsset
└── UDataAsset_DataListEntryMapping    ← DataObject 클래스 → Entry 위젯 클래스 매핑 테이블

UUserWidget
└── UWidget_OptionsDetailsView         ← 선택된 항목의 상세 정보 표시 패널
```

---

## 게임플레이 태그 체계

```
Frontend
├── WidgetStack
│   ├── Frontend    ← 메인메뉴·PressAnyKey 등 프론트엔드 화면
│   ├── Modal       ← 팝업·다이얼로그
│   ├── GameMenu    ← 인게임 메뉴
│   └── GameHud     ← 인게임 HUD
├── Widget
│   ├── PressAnyKeyScreen   ← "아무 버튼이나 누르시오" 화면
│   ├── MainMenuScreen      ← 메인 메뉴 화면
│   ├── ConfirmScreen       ← 확인/취소 팝업
│   └── OptionsScreen       ← 옵션 화면
└── Image
    └── TestImage           ← 옵션 화면 설명 이미지 예시
```

`WidgetStack` 태그 → **어느 스택에 넣을지** 결정  
`Widget` 태그 → **어떤 위젯 클래스를 로드할지** 결정  
`Image` 태그 → **옵션 항목 설명 이미지**를 `UFrontendDeveloperSettings`에서 조회할 때 사용

---

## 에디터 설정 (FrontendDeveloperSettings)

`프로젝트 설정 > Frontend UI Settings`에서 태그와 위젯 클래스를 매핑합니다.

```
FrontendWidgetMap
├── Frontend.Widget.PressAnyKeyScreen  →  BP_Widget_PressAnyKey (소프트 레퍼런스)
├── Frontend.Widget.MainMenuScreen     →  BP_Widget_MainMenu    (소프트 레퍼런스)
└── Frontend.Widget.OptionsScreen      →  BP_Widget_OptionsScreen (소프트 레퍼런스)

OptionsScreenSoftImageMap
└── Frontend.Image.TestImage           →  T_SomeTexture2D (소프트 레퍼런스)
```

소프트 레퍼런스이기 때문에 실제 에셋은 **필요한 시점에 비동기 로드**됩니다.

---

## 전체 초기화 흐름

```
게임 시작
│
├─ GameInstance 생성
│   └─ UFrontendUISubsystem 자동 생성 (ShouldCreateSubsystem 통과 시)
│       ※ 데디케이티드 서버 → 생성 안 함
│       ※ 파생 클래스 존재 → 기본 클래스 생성 안 함
│
├─ 레벨 로드 → AFrontendPlayerController::OnPossess()
│   └─ 씬에서 태그 "Default"인 CameraActor를 찾아 뷰타겟으로 설정
│
└─ [블루프린트] PrimaryLayout 위젯 생성 & 표시
    ├─ Widget_PrimaryLayout::RegisterWidgetStack() 호출 (스택 4개 등록)
    │   ├─ Frontend.WidgetStack.Frontend  → CommonActivatableWidgetStack
    │   ├─ Frontend.WidgetStack.Modal     → CommonActivatableWidgetStack
    │   ├─ Frontend.WidgetStack.GameMenu  → CommonActivatableWidgetStack
    │   └─ Frontend.WidgetStack.GameHud   → CommonActivatableWidgetStack
    │
    └─ UFrontendUISubsystem::RegisterCreatedPrimaryLayout() 호출
        └─ 서브시스템이 PrimaryLayout 참조를 캐시
```

---

## 위젯 푸시 흐름 (PressAnyKey → MainMenu)

### 단계 1 — 위젯 클래스 조회

```
[블루프린트]
UFrontendFunctionLibrary::GetFrontendSoftWidgetClassByTag(Frontend.Widget.PressAnyKeyScreen)
    └─ UFrontendDeveloperSettings::FrontendWidgetMap 에서 소프트 클래스 반환
```

### 단계 2 — 비동기 푸시 노드 실행

```
[블루프린트] "Push Soft Widget To Widget Stack" 노드 실행
    │
    ├─ UAsyncAction_PushSoftWidget::PushSoftWidget() [팩토리]
    │   ├─ WorldContextObject로 World 획득
    │   ├─ 파라미터 캐시 (World, PlayerController, SoftClass, StackTag, bFocus)
    │   └─ RegisterWithGameInstance() → GC로부터 노드 수명 보호
    │
    └─ UAsyncAction_PushSoftWidget::Activate() [UE가 자동 호출]
        └─ UFrontendUISubsystem::PushSoftWidgetToStackAsync()
```

### 단계 3 — 비동기 로드 & 스택 푸시

```
UAssetManager::StreamableManager::RequestAsyncLoad()
    │
    └─ [로드 완료 콜백]
        │
        ├─ Widget_PrimaryLayout::FindWidgetStackByTag(Frontend.WidgetStack.Frontend)
        │   └─ 해당 CommonActivatableWidgetStack 반환
        │
        ├─ Stack::AddWidget<UWidget_ActivatableBase>()
        │   └─ [OnCreatedBeforePush 콜백]
        │       ├─ PushedWidget->SetOwningPlayer(PlayerController)
        │       └─ OnWidgetCreatedBeforePush 델리게이트 브로드캐스트
        │
        └─ [AfterPush 콜백]
            ├─ AfterPush 델리게이트 브로드캐스트
            ├─ bFocus == true → GetDesiredFocusTarget()->SetFocus()
            └─ SetReadyToDestroy() → 노드 GC 허용
```

---

## 옵션 화면 생성 흐름 (전체 Flow)

```
[블루프린트] OptionsScreen 위젯 Push
    │
    ├─ UWidget_OptionsScreen::NativeOnInitialized()
    │   ├─ ResetAction 바인딩 등록 (ResetAction이 설정된 경우)
    │   ├─ Back 액션 바인딩 등록 (CommonUI 기본 Back 액션)
    │   ├─ TabListWidget_OptionsTabs::OnTabSelected → OnOptionsTabSelected 연결
    │   ├─ CommonListView_OptionsList::OnItemIsHoveredChanged → OnListViewItemHovered 연결
    │   └─ CommonListView_OptionsList::OnItemSelectionChanged → OnListViewItemSelected 연결
    │
    └─ UWidget_OptionsScreen::NativeOnActivated()
        │
        ├─ GetOrCreateDataRegistry() 호출
        │   └─ CreatedOwningDataRegistry가 없으면:
        │       ├─ NewObject<UOptionsDataRegistry>() 생성
        │       └─ UOptionsDataRegistry::InitOptionsDataRegistry(LocalPlayer)
        │           ├─ InitGameplayCollectionTab()   → "GameplayTabCollection" 등록
        │           ├─ InitAudioCollectionTab()      → "AudioTabCollection" 등록
        │           ├─ InitVideoCollectionTab()      → "VideoTabCollection" 등록
        │           └─ InitControlCollectionTab()    → "ControlTabCollection" 등록
        │
        └─ 각 TabCollection에 대해 TabListWidget_OptionsTabs::RequestRegisterTab() 호출
            └─ UFrontendTabListWidgetBase::RegisterTab() → 탭 버튼 생성 및 텍스트 설정

    [탭 선택 시] OnOptionsTabSelected(TabID)
        │
        ├─ DetailsView_ListEntryInfo->ClearDetailsViewInfo()
        ├─ DataRegistry::GetListSourceItemsBySelectedTabID(TabID) → 해당 탭의 자식 항목 배열 획득
        ├─ CommonListView_OptionsList::SetListItems() → 리스트 뷰 데이터 갱신
        ├─ CommonListView_OptionsList::RequestRefresh()
        ├─ NavigateToIndex(0) + SetSelectedIndex(0) (선택 항목 없을 때)
        └─ 각 항목에 OnListDataModified 델리게이트 연결, ResettableDataArray 갱신

    [리스트 항목 위젯 생성 시] UFrontendCommonListView::OnGenerateEntryWidgetInternal()
        └─ DataListEntryMapping::FindEntryWidgetClassByDataObject() 호출
            └─ 데이터 오브젝트 클래스 계층 탐색 → 적합한 Entry 위젯 클래스 선택

    [항목 위젯에 데이터 바인딩] UWidget_ListEntry_Base::NativeOnListItemObjectSet()
        ├─ SetVisibility(Visible)
        └─ OnOwningListDataObjectSet(ListDataObject) 호출
            ├─ CommonText_SettingDisplayName 텍스트 설정
            └─ OnListDataModified 델리게이트 연결

    [옵션 화면 비활성화] NativeOnDeactivated()
        └─ UFrontendGameUserSettings::Get()->ApplySettings(true)  ← 설정 디스크 저장
```

---

## 옵션 데이터 시스템 구조

### Data Object 계층

```
UListDataObject_Base (Abstract)
│   ├─ DataID           : FName       ← 탭/항목 고유 식별자
│   ├─ DataDisplayName  : FText       ← UI 표시 이름
│   ├─ DescriptionRichText : FText    ← 설명 패널에 표시할 Rich Text
│   ├─ DisabledRichText : FText       ← 비활성화 이유 텍스트
│   ├─ SoftDescriptionImage           ← 설명 이미지 (소프트 레퍼런스)
│   ├─ ParentData                     ← 부모 항목 참조
│   └─ bShouldApplyChangeImmediately  ← true 시 값 변경마다 ApplySettings() 즉시 호출
│
├── UListDataObject_Collection        ← 탭 컬렉션 (자식 배열 보유)
│   └─ ChildListDataArray : TArray<UListDataObject_Base*>
│
└── UListDataObject_Value (Abstract)  ← 단일 값 선택형 항목
    ├─ DataDynamicGetter  ← FOptionsDataInteractionHelper (UFrontendGameUserSettings 함수 래퍼)
    ├─ DataDynamicSetter  ← FOptionsDataInteractionHelper
    └─ DefaultStringValue : TOptional<FString>
        │
        └── UListDataObject_String    ← 문자열 목록 중 하나 선택
            ├─ AvailableOptionsStringArray : TArray<FString>
            ├─ AvailableOptionsTextArray   : TArray<FText>
            ├─ CurrentStringValue : FString
            └─ CurrentDisplayText : FText
```

### Collection과 Entry의 관계

```
UListDataObject_Collection (탭)
    └─ ChildListDataArray
        ├─ UListDataObject_String (옵션 항목 1)
        ├─ UListDataObject_String (옵션 항목 2)
        └─ ...

탭 선택 시:
    Collection::GetAllChildListData() → ListView에 SetListItems()
    ListView가 각 항목에 대해 EntryWidget 생성 (DataListEntryMapping 참조)
```

### FOptionsDataInteractionHelper 구조

```cpp
FOptionsDataInteractionHelper(GET_FUNCTION_NAME_STRING_CHECKED(UFrontendGameUserSettings, GetCurrentGameDifficulty))
    └─ FCachedPropertyPath로 함수 경로 캐시
    └─ GetValueAsString() / SetValueFromString() → UFrontendGameUserSettings의 Getter/Setter 호출
```

`UFrontendGameUserSettings`의 Getter/Setter는 반드시 `UFUNCTION()`으로 마킹되어야 합니다.  
`FCachedPropertyPath`가 리플렉션 시스템을 통해 함수를 호출하기 때문입니다.

---

## 옵션 탭(Tab) 추가 방법

### 작업 절차

**1단계 — `OptionsDataRegistry.cpp`에 Init 함수 추가**

```cpp
// OptionsDataRegistry.h
void InitAccessibilityCollectionTab(); // 헤더에 선언 추가

// OptionsDataRegistry.cpp
void UOptionsDataRegistry::InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer)
{
    InitGameplayCollectionTab();
    InitAudioCollectionTab();
    InitVideoCollectionTab();
    InitControlCollectionTab();
    InitAccessibilityCollectionTab(); // ← 여기에 추가 (표시 순서 = 호출 순서)
}

void UOptionsDataRegistry::InitAccessibilityCollectionTab()
{
    UListDataObject_Collection* AccessibilityTabCollection = NewObject<UListDataObject_Collection>();
    AccessibilityTabCollection->SetDataID(FName("AccessibilityTabCollection")); // ← 고유 ID (중복 불가)
    AccessibilityTabCollection->SetDataDisplayName(FText::FromString(TEXT("접근성")));

    // 하위 옵션 항목들을 여기서 AddChildListData()로 추가...

    RegisteredOptionsTabCollections.Add(AccessibilityTabCollection);
}
```

**2단계 — 블루프린트 `WBP_CAW_OptionsScreen`에서 탭 버튼 추가**

`TabListWidget_OptionsTabs`(`UFrontendTabListWidgetBase`)에 새 탭 버튼을 배치합니다.  
`NativeOnActivated()`에서 `GetTabButtonBaseByID(TabID) != nullptr` 조건을 통과해야 탭이 등록됩니다.  
버튼이 없으면 **탭이 화면에 표시되지 않습니다** (오류 없이 무시됨).

### 누락 시 발생하는 문제

| 누락 항목 | 증상 |
|---|---|
| `RegisteredOptionsTabCollections.Add()` 미호출 | 탭이 데이터 레지스트리에 없어 `GetListSourceItemsBySelectedTabID()` checkf 실패 → 크래시 |
| BP에서 탭 버튼 미배치 | 탭이 화면에 표시되지 않음 (오류 없음, 조용히 무시) |
| DataID 중복 | `GetListSourceItemsBySelectedTabID()`가 첫 번째 매칭 탭만 반환 |

---

## 탭 하위 옵션(Entry) 추가 방법

### 작업 절차

**1단계 — DataObject 생성 및 구성**

```cpp
// OptionsDataRegistry.cpp 내 해당 탭 Init 함수에서 작성

UListDataObject_String* MyNewOption = NewObject<UListDataObject_String>();
MyNewOption->SetDataID(FName("MyNewOption"));                         // 고유 ID
MyNewOption->SetDataDisplayName(FText::FromString(TEXT("새 옵션"))); // 탭 리스트에 표시될 이름
MyNewOption->SetDescriptionRichText(FText::FromString(TEXT("옵션 설명")));  // 상세 패널 설명

// 선택 가능한 값 목록 추가 (StringValue = 내부 저장값, DisplayText = UI 표시값)
MyNewOption->AddDynamicOption(TEXT("Value1"), FText::FromString(TEXT("옵션 1")));
MyNewOption->AddDynamicOption(TEXT("Value2"), FText::FromString(TEXT("옵션 2")));

// 기본값 설정 (Reset 기능 사용 시 필수)
MyNewOption->SetDefaultValueFromString(TEXT("Value1"));

// GameUserSettings와 연결 (값 저장/로드)
MyNewOption->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetMyNewOptionValue));
MyNewOption->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetMyNewOptionValue));

// 값 변경 시 즉시 적용 여부
MyNewOption->SetShouldApplySettingsImmediately(true);

// 탭 컬렉션에 추가 (이 시점에 InitDataObject() 자동 호출됨)
SomeTabCollection->AddChildListData(MyNewOption);
```

**2단계 — `FrontendGameUserSettings`에 Getter/Setter 추가**

```cpp
// FrontendGameUserSettings.h
UFUNCTION()  // ← 반드시 UFUNCTION 필요 (FCachedPropertyPath 리플렉션 사용)
FString GetMyNewOptionValue() const { return MyNewOptionValue; }

UFUNCTION()
void SetMyNewOptionValue(const FString& InValue) { MyNewOptionValue = InValue; }

UPROPERTY(Config)  // ← Config로 마킹해야 INI에 저장됨
FString MyNewOptionValue;
```

**3단계 — Entry Widget 매핑 확인**

`UListDataObject_String` 타입의 항목은 `UDataAsset_DataListEntryMapping`에서  
`UListDataObject_String → WBP_ListEntry_String` 매핑이 이미 등록되어 있으면 자동으로 적용됩니다.

새로운 DataObject 타입을 만드는 경우:
1. `UListDataObject_Value`를 상속하는 새 클래스 생성
2. `UWidget_ListEntry_Base`를 상속하는 새 Entry 위젯 블루프린트 생성
3. `DA_DataListEntryMapping` 데이터 에셋에서 클래스 → 위젯 매핑 추가

### 값 저장 및 로드 방식

```
[초기화 시 로드 순서] UListDataObject_String::OnDataObjectInitialized()
    1. AvailableOptionsStringArray[0] 으로 초기화 (첫 번째 옵션)
    2. DefaultStringValue가 있으면 해당 값으로 덮어쓰기
    3. DataDynamicGetter가 있으면 GameUserSettings에서 현재 저장된 값으로 덮어쓰기
       → INI 파일에 저장된 값이 최종적으로 적용됨

[값 변경 시 저장] AdvanceToNextOption() / BackToPreviousOption()
    → DataDynamicSetter::SetValueFromString() 호출
    → UFrontendGameUserSettings의 Setter 함수 실행 (메모리에만 저장)
    → bShouldApplyChangeImmediately == true 이면 ApplySettings(true) 즉시 호출 (INI 저장)

[화면 닫을 때] UWidget_OptionsScreen::NativeOnDeactivated()
    → UFrontendGameUserSettings::Get()->ApplySettings(true) 호출 (INI 저장)
```

---

## CommonUI 포커스 및 네비게이션

### 게임패드 포커스 이동 방식

`UWidget_OptionsScreen::NativeGetDesiredFocusTarget()`:
- 리스트 뷰에 선택된 항목이 있으면 해당 **Entry 위젯**으로 포커스 이동
- 없으면 `Super`(CommonUI 기본 포커스 타겟) 반환

`UWidget_ListEntry_Base::NativeOnFocusReceived()`:
- 게임패드 입력 타입 감지 시 `BP_GetWidgetToFocusForGamepad()` 호출
- 블루프린트에서 포커스를 받을 실제 자식 위젯(예: 로테이터)을 반환해야 함
- 반환값이 없으면 `Super` 기본 처리

```cpp
// Widget_ListEntry_Base.cpp
if (CommonInputSubsystem->GetCurrentInputType() == ECommonInputType::Gamepad)
{
    if (const UWidget* WidgetToFocus = BP_GetWidgetToFocusForGamepad())
    {
        return FReply::Handled().SetUserFocus(SlateWidgetToFocus.ToSharedRef());
    }
}
```

### 탭 전환 시 포커스 처리

탭 전환 → `OnOptionsTabSelected()` 호출 → `SetListItems()` + `NavigateToIndex(0)` + `SetSelectedIndex(0)`  
탭 변경 후 첫 번째 항목으로 자동 이동하며, `NativeGetDesiredFocusTarget()`이 해당 위젯을 반환합니다.

### 포커스 관련 버그가 발생하기 쉬운 부분

| 상황 | 문제 | 원인 |
|---|---|---|
| 새 Entry 위젯 추가 후 게임패드 조작 불가 | 포커스가 Entry 내부 위젯으로 전달되지 않음 | `BP_GetWidgetToFocusForGamepad()`를 블루프린트에서 구현하지 않음 |
| 탭 전환 후 이전 탭 항목이 선택된 상태 유지 | 잘못된 항목에 포커스 | `DetailsView_ListEntryInfo->ClearDetailsViewInfo()` 이후 타이밍 문제 |
| 로테이터 클릭해도 항목 선택 안 됨 | ListView 선택 상태 미변경 | `CommonRotator_AvailableOptions->OnClicked()`에서 `SelectThisEntryWidget()` 누락 |
| Entry 위젯 반환 후 Visibility가 Hidden | 아이템 재사용 시 표시 안 됨 | `NativeOnListItemObjectSet()`에서 `SetVisibility(Visible)` 미호출 |

---

## 개발 시 유의사항

### "Invalid Option"이 표시되는 원인

`UListDataObject_String::OnDataObjectInitialized()`에서 `TrySetDisplayTextFromStringValue()`가 실패하면  
`CurrentDisplayText = FText::FromString(TEXT("Invalid Option"))`이 설정됩니다.

**원인 체크리스트:**
1. `DataDynamicGetter`가 반환한 값이 `AvailableOptionsStringArray`에 없음
2. `DefaultStringValue`로 지정한 문자열이 `AddDynamicOption()`에 추가한 StringValue와 불일치 (대소문자 포함)
3. `AddDynamicOption()` 호출 전에 `AddChildListData()`를 호출하여 `InitDataObject()`가 먼저 실행됨
   - **주의**: `AddChildListData()`는 내부에서 `InitDataObject()`를 자동 호출합니다.  
     반드시 `AddDynamicOption()` 등 구성을 모두 완료한 후 `AddChildListData()`를 호출해야 합니다.

```cpp
// 잘못된 순서 (Invalid Option 발생)
GameplayTabCollection->AddChildListData(GameDifficulty); // InitDataObject() 여기서 호출됨
GameDifficulty->AddDynamicOption(TEXT("Easy"), ...);     // 너무 늦음

// 올바른 순서
GameDifficulty->AddDynamicOption(TEXT("Easy"), ...);     // 먼저 옵션 추가
GameDifficulty->SetDefaultValueFromString(TEXT("Easy")); // 기본값 설정
GameDifficulty->SetDataDynamicGetter(...);               // Getter 연결
GameplayTabCollection->AddChildListData(GameDifficulty); // 마지막에 추가 (InitDataObject 호출)
```

### Data Asset 누락 시 발생하는 문제

**`DataListEntryMapping` 미설정 (`UFrontendCommonListView` 속성)**

- 에디터 컴파일 시 `ValidateCompiledDefaults()`가 에러 로그 출력
- 런타임에 `OnGenerateEntryWidgetInternal()` 실패 → 항목 위젯이 생성되지 않음

**`TabButtonEntryWidgetClass` 미설정 (`UFrontendTabListWidgetBase` 속성)**

- 에디터 컴파일 시 에러 메시지: `"변수 TabButtonEntryWidgetClass에 유효한 항목이 지정되어 있지 않습니다."`
- 런타임에 탭 버튼이 생성되지 않음 → 탭 선택 불가

**`OptionsScreenSoftImageMap` 미등록 이미지 태그 사용**

- `UFrontendFunctionLibrary::GetOptionsSoftImageByTag()`가 빈 TSoftObjectPtr 반환
- `SetSoftDescriptionImage()`에 빈 포인터가 설정되어 이미지가 Collapsed 상태로 표시됨

### Entry WidgetClass 매핑 관련 주의사항

`UDataAsset_DataListEntryMapping::FindEntryWidgetClassByDataObject()`는 클래스 계층을 **위로** 탐색합니다.

```
UListDataObject_String → 매핑 없음 → UListDataObject_Value → 매핑 없음
    → UListDataObject_Base → 매핑 있음 → 해당 위젯 클래스 반환
```

- 자식 클래스에 매핑이 없어도 부모 클래스 매핑으로 폴백됩니다.
- 클래스 계층 전체에 매핑이 없으면 빈 `TSubclassOf`가 반환되어 위젯 생성 실패.
- 구체적인 자식 타입에 별도 위젯이 필요하면 해당 자식 타입을 명시적으로 매핑에 추가해야 합니다.

### Getter/Setter `UFUNCTION()` 누락

`FOptionsDataInteractionHelper`는 `FCachedPropertyPath`를 통해 함수를 리플렉션으로 호출합니다.  
`UFUNCTION()` 없이 일반 C++ 함수로 선언하면 `FCachedPropertyPath`가 함수를 찾지 못해 **값을 읽거나 쓸 수 없습니다**.  
이 경우 Getter는 빈 문자열을 반환하고 Setter는 아무 동작도 하지 않습니다.

### `bShouldApplyChangeImmediately` 관련 주의

`SetShouldApplySettingsImmediately(true)`로 설정된 항목은 값이 바뀔 때마다 `ApplySettings(true)`를 호출합니다.  
성능 민감한 설정(그래픽 품질, 해상도 등)에 이 플래그를 사용하면 빈번한 디스크 I/O와 그래픽 리셋이 발생할 수 있습니다.  
즉시 적용이 불필요한 항목은 `false`(기본값)로 두고, 화면 닫힐 때 `NativeOnDeactivated()`의 `ApplySettings()`에 맡기세요.

---

## 요약 — 핵심 역할 분담

| 클래스 | 역할 |
|---|---|
| `AFrontendPlayerController` | 카메라 뷰타겟 설정 |
| `UFrontendUISubsystem` | PrimaryLayout 참조 보관, 비동기 위젯 로드·푸시 |
| `UWidget_PrimaryLayout` | 4개 WidgetStack의 루트 컨테이너, 태그로 스택 관리 |
| `UWidget_ActivatableBase` | 개별 화면 위젯의 베이스 (PressAnyKey, MainMenu 등이 상속) |
| `UWidget_OptionsScreen` | 탭 리스트 + 옵션 리스트 + 디테일 패널 조합, Reset/Back 입력 처리 |
| `UOptionsDataRegistry` | 탭 컬렉션 생성 및 관리 (지연 초기화, 최초 접근 시 생성) |
| `UListDataObject_Collection` | 탭 데이터 (자식 항목 배열 보유) |
| `UListDataObject_String` | 문자열 선택형 옵션 항목 데이터 |
| `UFrontendCommonListView` | DataListEntryMapping으로 항목 타입별 위젯 자동 선택 |
| `UFrontendTabListWidgetBase` | 탭 버튼 목록 위젯 (`RequestRegisterTab()`으로 탭 등록) |
| `UWidget_ListEntry_Base` | 리스트 항목 위젯 베이스 (DataObject 연결, 포커스 처리) |
| `UWidget_ListEntry_String` | 이전/다음 버튼 + 로테이터로 문자열 값 순환 선택 |
| `UWidget_OptionsDetailsView` | 선택/호버 항목의 이름·이미지·설명 표시 패널 |
| `UDataAsset_DataListEntryMapping` | DataObject 클래스 → Entry 위젯 클래스 매핑 테이블 |
| `FOptionsDataInteractionHelper` | GameUserSettings Getter/Setter를 리플렉션으로 래핑 |
| `UFrontendGameUserSettings` | 설정값 런타임 보관 및 INI 저장 (`UGameUserSettings` 상속) |
| `UAsyncAction_PushSoftWidget` | 블루프린트에서 비동기 푸시를 노드 하나로 사용 가능하게 래핑 |
| `UFrontendFunctionLibrary` | 태그 → 소프트 클래스/이미지 조회 헬퍼 |
| `UFrontendDeveloperSettings` | 에디터에서 태그↔위젯 클래스·이미지 매핑 테이블 |

---

## 현재까지 구현된 화면 전환

```
게임 시작
  └→ PressAnyKeyScreen 푸시 (Frontend.WidgetStack.Frontend)
        └→ 아무 버튼 입력
              └→ MainMenuScreen 푸시 (Frontend.WidgetStack.Frontend)
                    └→ Options 버튼 클릭
                          └→ OptionsScreen 푸시 (Frontend.WidgetStack.Frontend)
                                └→ Back 입력
                                      └→ OptionsScreen 닫힘 (ApplySettings 자동 호출)
```

`CommonActivatableWidgetStack`은 스택 구조이므로 MainMenuScreen이 푸시되면  
PressAnyKeyScreen 위에 쌓이고, MainMenuScreen이 닫히면 자동으로 PressAnyKeyScreen으로 돌아옵니다.
