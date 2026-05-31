# Frontend UI 시스템 흐름 정리

Common UI 플러그인 기반으로 구성된 Frontend UI 시스템의 전체 구조와 흐름입니다.

---

## 클래스 구조 한눈에 보기

```mermaid
classDiagram
    direction TB

    class APlayerController
    class AFrontendPlayerController { 카메라 뷰타겟 설정 }
    APlayerController <|-- AFrontendPlayerController

    class UGameInstanceSubsystem
    class UFrontendUISubsystem { PrimaryLayout 참조 보관\n비동기 위젯 로드·푸시 }
    UGameInstanceSubsystem <|-- UFrontendUISubsystem

    class UCommonUserWidget
    class UWidget_PrimaryLayout { 4개 WidgetStack 루트 컨테이너 }
    UCommonUserWidget <|-- UWidget_PrimaryLayout

    class UCommonActivatableWidget
    class UWidget_ActivatableBase { 모든 화면 위젯의 베이스 }
    class UWidget_OptionsScreen { 탭 + 리스트 + 디테일 조합\nReset·Back 입력 처리 }
    UCommonActivatableWidget <|-- UWidget_ActivatableBase
    UWidget_ActivatableBase <|-- UWidget_OptionsScreen

    class UBlueprintFunctionLibrary
    class UFrontendFunctionLibrary { 태그 → 소프트 클래스·이미지 조회 }
    UBlueprintFunctionLibrary <|-- UFrontendFunctionLibrary

    class UDeveloperSettings
    class UFrontendDeveloperSettings { 태그 ↔ 위젯 클래스·이미지 매핑 }
    UDeveloperSettings <|-- UFrontendDeveloperSettings

    class UGameUserSettings
    class UFrontendGameUserSettings { 설정값 보관 및 INI 저장 }
    UGameUserSettings <|-- UFrontendGameUserSettings
```

```mermaid
classDiagram
    direction TB

    class UObject
    class UOptionsDataRegistry { 탭 컬렉션 생성 및 관리 }
    UObject <|-- UOptionsDataRegistry

    class UListDataObject_Base {
        <<Abstract>>
        DataID : FName
        DataDisplayName : FText
        DescriptionRichText : FText
        DisabledRichText : FText
        SoftDescriptionImage
        bShouldApplyChangeImmediately : bool
    }
    class UListDataObject_Collection { ChildListDataArray }
    class UListDataObject_Value {
        <<Abstract>>
        DataDynamicGetter
        DataDynamicSetter
        DefaultStringValue
    }
    class UListDataObject_String {
        AvailableOptionsStringArray
        AvailableOptionsTextArray
        CurrentStringValue : FString
        CurrentDisplayText : FText
    }
    UObject <|-- UListDataObject_Base
    UListDataObject_Base <|-- UListDataObject_Collection
    UListDataObject_Base <|-- UListDataObject_Value
    UListDataObject_Value <|-- UListDataObject_String

    class UCommonUserWidget
    class UWidget_ListEntry_Base {
        <<Abstract>>
        OnOwningListDataObjectSet()
        OnOwningListDataObjectModified()
        BP_GetWidgetToFocusForGamepad()
    }
    class UWidget_ListEntry_String {
        CommonButton_PreviousOption
        CommonRotator_AvailableOptions
        CommonButton_NextOption
    }
    UCommonUserWidget <|-- UWidget_ListEntry_Base
    UWidget_ListEntry_Base <|-- UWidget_ListEntry_String

    class UCommonListView
    class UFrontendCommonListView { DataListEntryMapping 기반\n항목 타입별 위젯 자동 선택 }
    UCommonListView <|-- UFrontendCommonListView

    class UDataAsset
    class UDataAsset_DataListEntryMapping { DataObject 클래스 →\nEntry 위젯 클래스 매핑 테이블 }
    UDataAsset <|-- UDataAsset_DataListEntryMapping

    class UUserWidget
    class UWidget_OptionsDetailsView { 선택 항목 이름·이미지·설명 표시 }
    UUserWidget <|-- UWidget_OptionsDetailsView
```

---

## 게임플레이 태그 체계

```mermaid
mindmap
  root((Frontend))
    WidgetStack
      Frontend
      Modal
      GameMenu
      GameHud
    Widget
      PressAnyKeyScreen
      MainMenuScreen
      ConfirmScreen
      OptionsScreen
    Image
      TestImage
```

- `WidgetStack` 태그 → **어느 스택에 넣을지** 결정
- `Widget` 태그 → **어떤 위젯 클래스를 로드할지** 결정
- `Image` 태그 → **옵션 항목 설명 이미지**를 `UFrontendDeveloperSettings`에서 조회할 때 사용

---

## 에디터 설정 (FrontendDeveloperSettings)

`프로젝트 설정 > Frontend UI Settings`에서 태그와 위젯 클래스를 매핑합니다.

```
FrontendWidgetMap
├── Frontend.Widget.PressAnyKeyScreen  →  BP_Widget_PressAnyKey   (소프트 레퍼런스)
├── Frontend.Widget.MainMenuScreen     →  BP_Widget_MainMenu       (소프트 레퍼런스)
└── Frontend.Widget.OptionsScreen      →  BP_Widget_OptionsScreen  (소프트 레퍼런스)

OptionsScreenSoftImageMap
└── Frontend.Image.TestImage           →  T_SomeTexture2D          (소프트 레퍼런스)
```

소프트 레퍼런스이기 때문에 실제 에셋은 **필요한 시점에 비동기 로드**됩니다.

---

## 전체 초기화 흐름

```mermaid
flowchart TD
    A([게임 시작]) --> B[GameInstance 생성]
    B --> C{ShouldCreateSubsystem?}
    C -- 데디케이티드 서버 --> X1([생성 안 함])
    C -- 파생 클래스 존재 --> X2([기본 클래스 생성 안 함])
    C -- 통과 --> D[UFrontendUISubsystem 자동 생성]

    D --> E[레벨 로드]
    E --> F[AFrontendPlayerController::OnPossess]
    F --> G[태그 'Default' CameraActor 찾아 뷰타겟 설정]

    G --> H["[BP] PrimaryLayout 위젯 생성 & 표시"]
    H --> I[Widget_PrimaryLayout::RegisterWidgetStack]
    I --> I1[Frontend.WidgetStack.Frontend 등록]
    I --> I2[Frontend.WidgetStack.Modal 등록]
    I --> I3[Frontend.WidgetStack.GameMenu 등록]
    I --> I4[Frontend.WidgetStack.GameHud 등록]

    I1 & I2 & I3 & I4 --> J[UFrontendUISubsystem::RegisterCreatedPrimaryLayout]
    J --> K([서브시스템이 PrimaryLayout 참조 캐시])
```

---

## 위젯 푸시 흐름

```mermaid
sequenceDiagram
    actor BP as Blueprint
    participant FL as UFrontendFunctionLibrary
    participant DS as UFrontendDeveloperSettings
    participant AA as UAsyncAction_PushSoftWidget
    participant SS as UFrontendUISubsystem
    participant AM as UAssetManager
    participant PL as UWidget_PrimaryLayout
    participant ST as CommonActivatableWidgetStack

    BP->>FL: GetFrontendSoftWidgetClassByTag(Widget.PressAnyKeyScreen)
    FL->>DS: FrontendWidgetMap 조회
    DS-->>FL: SoftClass 반환
    FL-->>BP: SoftClass 반환

    BP->>AA: PushSoftWidget() [팩토리]
    Note over AA: 파라미터 캐시 후<br/>RegisterWithGameInstance()

    AA->>SS: PushSoftWidgetToStackAsync()
    SS->>AM: RequestAsyncLoad(SoftClass)

    AM-->>SS: 로드 완료 콜백
    SS->>PL: FindWidgetStackByTag(WidgetStack.Frontend)
    PL-->>SS: CommonActivatableWidgetStack 반환

    SS->>ST: AddWidget<UWidget_ActivatableBase>()
    ST-->>SS: OnCreatedBeforePush 콜백
    SS->>SS: SetOwningPlayer(PlayerController)
    SS->>BP: OnWidgetCreatedBeforePush 델리게이트

    ST-->>SS: AfterPush 콜백
    SS->>BP: AfterPush 델리게이트
    Note over SS: bFocus==true 이면<br/>GetDesiredFocusTarget()->SetFocus()
    SS->>AA: SetReadyToDestroy()
```

---

## 옵션 화면 생성 흐름

```mermaid
sequenceDiagram
    actor BP as Blueprint
    participant OS as UWidget_OptionsScreen
    participant DR as UOptionsDataRegistry
    participant TL as UFrontendTabListWidgetBase
    participant LV as UFrontendCommonListView
    participant DV as UWidget_OptionsDetailsView

    BP->>OS: Push OptionsScreen

    OS->>OS: NativeOnInitialized()
    Note over OS: Reset/Back 액션 바인딩<br/>TabListWidget.OnTabSelected 연결<br/>ListView.OnHovered/OnSelected 연결

    OS->>OS: NativeOnActivated()
    OS->>DR: GetOrCreateDataRegistry()

    alt DataRegistry 없음 (최초 접근)
        DR->>DR: InitOptionsDataRegistry(LocalPlayer)
        DR->>DR: InitGameplayCollectionTab()
        DR->>DR: InitAudioCollectionTab()
        DR->>DR: InitVideoCollectionTab()
        DR->>DR: InitControlCollectionTab()
    end

    DR-->>OS: RegisteredOptionsTabCollections 반환

    loop 각 TabCollection
        OS->>TL: RequestRegisterTab(TabID, DisplayName)
        TL->>TL: RegisterTab() → 탭 버튼 생성
        TL->>TL: SetButtonText(DisplayName)
    end

    TL-->>OS: OnTabSelected(TabID) 콜백

    OS->>DV: ClearDetailsViewInfo()
    OS->>DR: GetListSourceItemsBySelectedTabID(TabID)
    DR-->>OS: TArray<UListDataObject_Base*>

    OS->>LV: SetListItems(FoundItems)
    OS->>LV: RequestRefresh()
    OS->>LV: NavigateToIndex(0) + SetSelectedIndex(0)

    LV->>LV: OnGenerateEntryWidgetInternal(Item)
    Note over LV: DataListEntryMapping으로<br/>DataObject 타입에 맞는<br/>Entry 위젯 클래스 조회

    LV->>LV: NativeOnListItemObjectSet(DataObject)
    Note over LV: SetVisibility(Visible)<br/>OnOwningListDataObjectSet() 호출<br/>DisplayName 텍스트 설정<br/>OnListDataModified 델리게이트 연결
```

---

## 옵션 데이터 시스템 구조

### Data Object 계층

```mermaid
classDiagram
    direction TB

    class UListDataObject_Base {
        <<Abstract>>
        +DataID : FName
        +DataDisplayName : FText
        +DescriptionRichText : FText
        +DisabledRichText : FText
        +SoftDescriptionImage : TSoftObjectPtr
        +ParentData : UListDataObject_Base*
        -bShouldApplyChangeImmediately : bool
        +InitDataObject()
        +GetAllChildListData() TArray
        +HasDefaultValue() bool
        +CanResetBackToDefaultValue() bool
        +TryResetBackToDefaultValue() bool
        #OnDataObjectInitialized()
        #NotifyListDataModified()
    }

    class UListDataObject_Collection {
        -ChildListDataArray : TArray
        +AddChildListData(UListDataObject_Base*)
        +GetAllChildListData() TArray
        +HasAnyChildListData() bool
    }

    class UListDataObject_Value {
        <<Abstract>>
        #DataDynamicGetter : TSharedPtr
        #DataDynamicSetter : TSharedPtr
        -DefaultStringValue : TOptional~FString~
        +SetDataDynamicGetter()
        +SetDataDynamicSetter()
        +SetDefaultValueFromString()
    }

    class UListDataObject_String {
        +AvailableOptionsStringArray : TArray~FString~
        +AvailableOptionsTextArray : TArray~FText~
        #CurrentStringValue : FString
        #CurrentDisplayText : FText
        +AddDynamicOption()
        +AdvanceToNextOption()
        +BackToPreviousOption()
        +OnRotatorInitiatedValueChange()
        #OnDataObjectInitialized()
        +CanResetBackToDefaultValue() bool
        +TryResetBackToDefaultValue() bool
    }

    UListDataObject_Base <|-- UListDataObject_Collection
    UListDataObject_Base <|-- UListDataObject_Value
    UListDataObject_Value <|-- UListDataObject_String
```

### Collection과 Entry 관계

```mermaid
graph LR
    subgraph Registry["UOptionsDataRegistry"]
        GC["GameplayTabCollection\n(UListDataObject_Collection)"]
        AC["AudioTabCollection"]
        VC["VideoTabCollection"]
        CC["ControlTabCollection"]
    end

    GC --> E1["GameDifficulty\n(UListDataObject_String)"]
    GC --> E2["TestItem\n(UListDataObject_String)"]
    AC --> E3["(옵션 항목 추가 예정)"]

    subgraph Mapping["UDataAsset_DataListEntryMapping"]
        M1["UListDataObject_String\n→ WBP_ListEntry_String"]
    end

    E1 & E2 -->|클래스 계층 탐색| Mapping
    Mapping -->|위젯 클래스 반환| W1["UWidget_ListEntry_String\n인스턴스 생성"]
```

### 값 저장/로드 흐름

```mermaid
flowchart LR
    subgraph Init["초기화 시 (OnDataObjectInitialized)"]
        direction TB
        S1["AvailableOptions[0]\n(기본 초기값)"]
        S2["DefaultStringValue\n(덮어쓰기)"]
        S3["DataDynamicGetter\n→ INI 저장값\n(최종 적용)"]
        S1 --> S2 --> S3
    end

    subgraph Change["값 변경 시"]
        direction TB
        C1["AdvanceToNextOption()\nBackToPreviousOption()"]
        C2["DataDynamicSetter\n→ GameUserSettings Setter"]
        C3{"bShouldApply\nImmediately?"}
        C4["ApplySettings(true)\n즉시 INI 저장"]
        C5["NativeOnDeactivated 시\nApplySettings(true)"]
        C1 --> C2 --> C3
        C3 -- true --> C4
        C3 -- false --> C5
    end
```

### FOptionsDataInteractionHelper 구조

```
MAKE_OPTIONS_DATA_CONTROL(GetCurrentGameDifficulty)
    └─ MakeShared<FOptionsDataInteractionHelper>(
           GET_FUNCTION_NAME_STRING_CHECKED(UFrontendGameUserSettings, GetCurrentGameDifficulty))
    └─ FCachedPropertyPath로 함수 경로 캐시
    └─ GetValueAsString() / SetValueFromString()
           → UFrontendGameUserSettings 인스턴스의 Getter/Setter 리플렉션 호출
```

`UFrontendGameUserSettings`의 Getter/Setter는 반드시 `UFUNCTION()`으로 마킹되어야 합니다.  
`FCachedPropertyPath`가 리플렉션 시스템을 통해 함수를 호출하기 때문입니다.

---

## 옵션 탭(Tab) 추가 방법

### 작업 절차

```mermaid
flowchart TD
    A["① OptionsDataRegistry.h\n헤더에 InitXxxCollectionTab() 선언"] --> B
    B["② OptionsDataRegistry.cpp\nInitOptionsDataRegistry()에 호출 추가\n(호출 순서 = 탭 표시 순서)"] --> C
    C["③ InitXxxCollectionTab() 구현\nNewObject<UListDataObject_Collection>()\nSetDataID / SetDataDisplayName\nRegisterTabCollections.Add()"] --> D
    D["④ WBP_CAW_OptionsScreen 열기\nTabListWidget_OptionsTabs에\n새 탭 버튼 배치\n(ID는 DataID와 동일하게)"] --> E
    E([완료])
```

**코드 예시:**

```cpp
// OptionsDataRegistry.h — 선언 추가
void InitAccessibilityCollectionTab();

// OptionsDataRegistry.cpp — 호출 추가
void UOptionsDataRegistry::InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer)
{
    InitGameplayCollectionTab();
    InitAudioCollectionTab();
    InitVideoCollectionTab();
    InitControlCollectionTab();
    InitAccessibilityCollectionTab(); // ← 표시 순서 = 호출 순서
}

// 구현
void UOptionsDataRegistry::InitAccessibilityCollectionTab()
{
    UListDataObject_Collection* Tab = NewObject<UListDataObject_Collection>();
    Tab->SetDataID(FName("AccessibilityTabCollection")); // ← 고유 ID (중복 불가)
    Tab->SetDataDisplayName(FText::FromString(TEXT("접근성")));

    // 하위 옵션 항목을 AddChildListData()로 추가...

    RegisteredOptionsTabCollections.Add(Tab);
}
```

### 누락 시 발생하는 문제

```mermaid
flowchart LR
    subgraph A["RegisteredTabCollections.Add() 미호출"]
        A1["탭 데이터 없음\n→ GetListSourceItemsBySelectedTabID()\ncheckf 실패 → 크래시"]
    end
    subgraph B["BP 탭 버튼 미배치"]
        B1["GetTabButtonBaseByID() == nullptr\n→ RequestRegisterTab() 미호출\n→ 탭 화면에 미표시\n(오류 없음, 조용히 무시)"]
    end
    subgraph C["DataID 중복"]
        C1["GetListSourceItemsBySelectedTabID()\n첫 번째 매칭만 반환\n→ 두 번째 탭 항목 표시 안 됨"]
    end
```

---

## 탭 하위 옵션(Entry) 추가 방법

### 작업 절차

```mermaid
flowchart TD
    A["① NewObject<UListDataObject_String>() 생성\nSetDataID / SetDataDisplayName\nSetDescriptionRichText"] --> B
    B["② AddDynamicOption() 반복 호출\n(StringValue, DisplayText 쌍 추가)"] --> C
    C["③ SetDefaultValueFromString()\nReset 기능 사용 시 필수"] --> D
    D["④ SetDataDynamicGetter/Setter()\nMAKE_OPTIONS_DATA_CONTROL 매크로 사용"] --> E
    E["⑤ SetShouldApplySettingsImmediately()\n즉시 적용 여부 결정"] --> F
    F["⑥ TabCollection->AddChildListData()\n← 이 시점에 InitDataObject() 자동 호출"] --> G
    G["⑦ FrontendGameUserSettings에\nUFUNCTION() Getter/Setter 추가\nUPROPERTY(Config) 멤버 추가"] --> H
    H["⑧ DataListEntryMapping DA에서\nDataObject 클래스 → Entry 위젯 매핑 확인"] --> I
    I([완료])
```

**코드 예시:**

```cpp
// ① ~ ⑥ OptionsDataRegistry.cpp
UListDataObject_String* MyOption = NewObject<UListDataObject_String>();
MyOption->SetDataID(FName("MyOption"));
MyOption->SetDataDisplayName(FText::FromString(TEXT("새 옵션")));
MyOption->SetDescriptionRichText(FText::FromString(TEXT("옵션 설명 Rich Text")));

MyOption->AddDynamicOption(TEXT("Value1"), FText::FromString(TEXT("옵션 1")));
MyOption->AddDynamicOption(TEXT("Value2"), FText::FromString(TEXT("옵션 2")));

MyOption->SetDefaultValueFromString(TEXT("Value1"));
MyOption->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetMyOptionValue));
MyOption->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetMyOptionValue));
MyOption->SetShouldApplySettingsImmediately(true);

SomeTabCollection->AddChildListData(MyOption); // ← 마지막에 호출
```

```cpp
// ⑦ FrontendGameUserSettings.h
UFUNCTION()  // ← 필수 (FCachedPropertyPath 리플렉션)
FString GetMyOptionValue() const { return MyOptionValue; }

UFUNCTION()
void SetMyOptionValue(const FString& InValue) { MyOptionValue = InValue; }

UPROPERTY(Config)  // ← INI 저장 필수
FString MyOptionValue;
```

### Entry Widget 매핑 탐색 방식

```mermaid
flowchart LR
    A["UListDataObject_MyCustomType\n(매핑 없음)"] --> B["UListDataObject_Value\n(매핑 없음)"] --> C["UListDataObject_Base\n(매핑 있음 → WBP_ListEntry_Base)"]
    style A fill:#f9f,stroke:#333
    style C fill:#9f9,stroke:#333
```

정확한 타입에 매핑이 없으면 **부모 클래스로 순차 탐색**하여 폴백 위젯을 사용합니다.  
클래스 계층 전체에 매핑이 없으면 위젯 생성 실패입니다.

---

## CommonUI 포커스 및 네비게이션

### 게임패드 포커스 이동 흐름

```mermaid
sequenceDiagram
    participant UE as UE 포커스 시스템
    participant LB as UWidget_ListEntry_Base
    participant BP as 블루프린트 (BP_ListEntry_String)
    participant Rot as UFrontendCommonRotator

    UE->>LB: NativeOnFocusReceived()
    LB->>LB: GetCurrentInputType() == Gamepad?

    alt 게임패드 입력
        LB->>BP: BP_GetWidgetToFocusForGamepad()
        BP-->>LB: 포커스 대상 위젯 반환 (예: Rotator)
        LB->>Rot: SetUserFocus()
    else 키보드/마우스
        LB->>UE: Super::NativeOnFocusReceived() (기본 처리)
    end
```

### 탭 전환 시 포커스 처리

```mermaid
flowchart TD
    A["탭 버튼 선택"] --> B["OnOptionsTabSelected(TabID)"]
    B --> C["ClearDetailsViewInfo()"]
    C --> D["SetListItems() + RequestRefresh()"]
    D --> E{"GetSelectedItem() == nullptr?"}
    E -- Yes --> F["NavigateToIndex(0)\nSetSelectedIndex(0)"]
    E -- No --> G["기존 선택 유지"]
    F --> H["NativeGetDesiredFocusTarget()\n→ Index 0 Entry 위젯 반환"]
    G --> H
```

### 포커스 관련 버그가 발생하기 쉬운 부분

| 상황 | 증상 | 원인 |
|---|---|---|
| 새 Entry 위젯 추가 후 게임패드 조작 불가 | 포커스가 Entry 내부 위젯으로 전달 안 됨 | `BP_GetWidgetToFocusForGamepad()` 블루프린트 미구현 |
| 로테이터 클릭해도 리스트 선택 안 됨 | ListView 선택 상태 미변경 | `OnClicked()`에서 `SelectThisEntryWidget()` 미호출 |
| Entry 위젯 재사용 시 표시 안 됨 | 아이템이 보이지 않음 | `NativeOnListItemObjectSet()`의 `SetVisibility(Visible)` 누락 |
| 탭 전환 후 디테일 패널 이전 정보 잔류 | 이전 항목 설명이 남음 | `OnOptionsTabSelected()` 시작 시 `ClearDetailsViewInfo()` 미호출 |

---

## 개발 시 유의사항

### "Invalid Option"이 표시되는 원인

```mermaid
flowchart TD
    A["UListDataObject_String::OnDataObjectInitialized()"] --> B["TrySetDisplayTextFromStringValue(CurrentStringValue)"]
    B --> C{"AvailableOptionsStringArray에\n해당 값이 있는가?"}
    C -- Yes --> D["CurrentDisplayText 정상 설정"]
    C -- No --> E["CurrentDisplayText =\nFText::FromString('Invalid Option')"]

    subgraph 주요 원인
        X1["AddDynamicOption() 전에\nAddChildListData() 호출\n← InitDataObject()가 먼저 실행됨"]
        X2["DefaultStringValue 오타\n또는 대소문자 불일치"]
        X3["DataDynamicGetter 반환값이\nAvailableOptions에 없음"]
    end
    E -.-> 주요 원인
```

**올바른 호출 순서:**

```cpp
// ❌ 잘못된 순서
SomeTab->AddChildListData(MyOption);  // InitDataObject() 여기서 호출 → 옵션 배열 비어있음
MyOption->AddDynamicOption(...);       // 너무 늦음 → "Invalid Option"

// ✅ 올바른 순서
MyOption->AddDynamicOption(TEXT("Easy"), ...);
MyOption->SetDefaultValueFromString(TEXT("Easy"));
MyOption->SetDataDynamicGetter(...);
SomeTab->AddChildListData(MyOption);  // 마지막에 호출 → InitDataObject() 정상 실행
```

### Data Asset 누락 시 발생하는 문제

| 누락 항목 | 에디터 증상 | 런타임 증상 |
|---|---|---|
| `UFrontendCommonListView::DataListEntryMapping` 미설정 | 컴파일 시 에러 로그 | 항목 위젯 생성 안 됨 |
| `UFrontendTabListWidgetBase::TabButtonEntryWidgetClass` 미설정 | 컴파일 에러: `"유효한 항목이 지정되어 있지 않습니다"` | 탭 버튼 생성 안 됨 → 탭 선택 불가 |
| `OptionsScreenSoftImageMap` 미등록 이미지 태그 | 없음 | 설명 이미지 Collapsed 표시 |
| `FrontendWidgetMap` 미등록 위젯 태그 | 없음 | `GetFrontendSoftWidgetClassByTag()` 빈 포인터 반환 |

### `UFUNCTION()` 누락 시 동작

```mermaid
flowchart LR
    A["FOptionsDataInteractionHelper::GetValueAsString()"] --> B["FCachedPropertyPath로\n함수 탐색"]
    B --> C{"UFUNCTION() 마킹?"}
    C -- Yes --> D["UFrontendGameUserSettings\nGetter 호출 성공"]
    C -- No --> E["함수 못 찾음\n→ 빈 FString 반환\n(오류 없음, 조용히 실패)"]
    E --> F["OnDataObjectInitialized()에서\nGetter 값 무시\n→ DefaultValue 또는 첫 번째 옵션 표시"]
```

### `bShouldApplySettingsImmediately` 관련 주의

| 설정값 | 동작 | 적합한 옵션 타입 |
|---|---|---|
| `true` | 값 변경마다 `ApplySettings(true)` 호출 (즉시 INI 저장) | 난이도, 언어 등 즉시 반영 필요 항목 |
| `false` (기본) | 화면 닫을 때 `NativeOnDeactivated()`에서 일괄 저장 | 그래픽 품질, 해상도 등 빈번한 저장이 비효율적인 항목 |

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

```mermaid
flowchart TD
    A([게임 시작]) --> B["PressAnyKeyScreen 푸시\n(WidgetStack.Frontend)"]
    B --> C{아무 버튼 입력}
    C --> D["MainMenuScreen 푸시\n(WidgetStack.Frontend)"]
    D --> E{Options 버튼 클릭}
    E --> F["OptionsScreen 푸시\n(WidgetStack.Frontend)"]
    F --> G{Back 입력}
    G --> H["OptionsScreen 닫힘\nApplySettings() 자동 호출"]
    H --> D

    style A fill:#4a9,color:#fff
    style H fill:#94a,color:#fff
```

`CommonActivatableWidgetStack`은 스택 구조이므로 MainMenuScreen이 푸시되면  
PressAnyKeyScreen 위에 쌓이고, MainMenuScreen이 닫히면 자동으로 PressAnyKeyScreen으로 돌아옵니다.
