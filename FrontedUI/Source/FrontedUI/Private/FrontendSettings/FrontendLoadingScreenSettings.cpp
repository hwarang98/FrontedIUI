// LeeHwaRang All Rights Reserved


#include "FrontendSettings/FrontendLoadingScreenSettings.h"
#include "Blueprint/UserWidget.h"

TSubclassOf<UUserWidget> UFrontendLoadingScreenSettings::GetLoadingScreenWidgetClassChecked() const
{
	checkf(!SoftLoadingScreenWidgetClass.IsNull(), TEXT("프로젝트 설정에서 유효한 위젯 블루프린트를 로딩 화면으로 할당하는 것을 잊었습니다."));

	TSubclassOf<UUserWidget> LoadedLoadingScreenWidget = SoftLoadingScreenWidgetClass.LoadSynchronous();

	return LoadedLoadingScreenWidget;
}