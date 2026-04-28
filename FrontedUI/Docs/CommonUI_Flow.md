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

UBlueprintAsyncActionBase
└── UAsyncAction_PushSoftWidget        ← 블루프린트 비동기 푸시 노드

UBlueprintFunctionLibrary
└── UFrontendFunctionLibrary           ← 태그로 위젯 클래스 조회

UDeveloperSettings
└── UFrontendDeveloperSettings         ← 에디터에서 태그↔위젯 클래스 등록
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
└── Widget
    ├── PressAnyKeyScreen   ← "아무 버튼이나 누르시오" 화면
    └── MainMenuScreen      ← 메인 메뉴 화면
```

`WidgetStack` 태그 → **어느 스택에 넣을지** 결정  
`Widget` 태그 → **어떤 위젯 클래스를 로드할지** 결정

---

## 에디터 설정 (FrontendDeveloperSettings)

`프로젝트 설정 > Frontend UI Settings`에서 태그와 위젯 클래스를 매핑합니다.

```
FrontendWidgetMap
├── Frontend.Widget.PressAnyKeyScreen  →  BP_Widget_PressAnyKey (소프트 레퍼런스)
└── Frontend.Widget.MainMenuScreen     →  BP_Widget_MainMenu    (소프트 레퍼런스)
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

## 요약 — 핵심 역할 분담

| 클래스 | 역할 |
|---|---|
| `AFrontendPlayerController` | 카메라 뷰타겟 설정 |
| `UFrontendUISubsystem` | PrimaryLayout 참조 보관, 비동기 위젯 로드·푸시 |
| `UWidget_PrimaryLayout` | 4개 WidgetStack의 루트 컨테이너, 태그로 스택 관리 |
| `UWidget_ActivatableBase` | 개별 화면 위젯의 베이스 (PressAnyKey, MainMenu 등이 상속) |
| `UAsyncAction_PushSoftWidget` | 블루프린트에서 비동기 푸시를 노드 하나로 사용 가능하게 래핑 |
| `UFrontendFunctionLibrary` | 태그 → 소프트 클래스 조회 헬퍼 |
| `UFrontendDeveloperSettings` | 에디터에서 태그↔위젯 클래스 매핑 테이블 |

---

## 현재까지 구현된 화면 전환

```
게임 시작
  └→ PressAnyKeyScreen 푸시 (Frontend.WidgetStack.Frontend)
        └→ 아무 버튼 입력
              └→ MainMenuScreen 푸시 (Frontend.WidgetStack.Frontend)
```

`CommonActivatableWidgetStack`은 스택 구조이므로 MainMenuScreen이 푸시되면  
PressAnyKeyScreen 위에 쌓이고, MainMenuScreen이 닫히면 자동으로 PressAnyKeyScreen으로 돌아옵니다.