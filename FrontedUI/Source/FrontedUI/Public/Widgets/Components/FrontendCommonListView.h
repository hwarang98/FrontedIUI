// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonListView.h"
#include "FrontendCommonListView.generated.h"

class UDataAsset_DataListEntryMapping;
/**
 * DataListEntryMapping 데이터 에셋을 기반으로 데이터 오브젝트 타입에 맞는 위젯을 자동 선택하는 리스트 뷰.
 * 항목 생성 시 데이터 오브젝트의 클래스 계층을 탐색해 등록된 엔트리 위젯 클래스를 선택합니다.
 */
UCLASS()
class FRONTEDUI_API UFrontendCommonListView : public UCommonListView
{
	GENERATED_BODY()

protected:
	/**
	 * @brief DataListEntryMapping에서 데이터 오브젝트 타입에 맞는 위젯 클래스를 찾아 항목 위젯을 생성합니다.
	 * 에디터 디자인 타임에는 기본 동작을 유지하고, 런타임에는 클래스 계층을 탐색해 최적 위젯을 선택합니다.
	 *
	 * @param Item 생성할 데이터 오브젝트
	 * @param DesiredEntryClass 기본 요청 위젯 클래스 (매핑 실패 시 폴백)
	 * @param OwnerTable 이 위젯이 속할 테이블 뷰
	 * @return 생성된 엔트리 위젯
	 */
	virtual UUserWidget& OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable) override;

private:
	#if WITH_EDITOR
	/** DataListEntryMapping이 할당되지 않은 경우 컴파일 오류를 기록합니다. */
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& CompileLog) const override;
	#endif


	UPROPERTY(EditAnyWhere, Category="Frontend List View Setting")
	UDataAsset_DataListEntryMapping* DataListEntryMapping; // 데이터 오브젝트 타입을 엔트리 위젯 클래스로 매핑하는 데이터 에셋


};