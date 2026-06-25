// LeeHwaRang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "OptionsDataRegistry.generated.h"

class UListDataObject_Base;
class UListDataObject_Collection;

/**
 * 옵션 화면에서 사용하는 탭 데이터를 생성하고 관리하는 레지스트리.
 * 옵션 화면이 생성된 직후 InitOptionsDataRegistry를 호출하여 각 탭 컬렉션을 초기화하고,
 * GetRegisteredOptionsTabCollections로 등록된 탭 목록을 제공합니다.
 */
UCLASS()
class FRONTEDUI_API UOptionsDataRegistry : public UObject
{
	GENERATED_BODY()

public:
	/** 옵션 레지스트리를 초기화합니다. 옵션 화면 생성 직후 호출되며, 각 카테고리 탭 컬렉션을 등록합니다. */
	void InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer);

	/** 등록된 옵션 탭 컬렉션 배열을 반환합니다. */
	const TArray<UListDataObject_Collection*>& GetRegisteredOptionsTabCollections() const { return RegisteredOptionsTabCollections; }

	/**
	 * @brief 선택된 탭 ID에 해당하는 탭 컬렉션의 자식 항목 배열을 반환합니다.
	 * 해당 ID의 탭이 없으면 checkf로 실행이 중단됩니다.
	 *
	 * @param InSelectedTabID 검색할 탭의 고유 식별자
	 * @return 해당 탭의 자식 리스트 데이터 배열
	 */
	TArray<UListDataObject_Base*> GetListSourceItemsBySelectedTabID(const FName& InSelectedTabID);

private:
	void FindChildListDataRecursively(UListDataObject_Base* InParentData, TArray<UListDataObject_Base*>& OutFoundChildListData) const;

	void InitGameplayCollectionTab();                                 // 게임플레이 탭 컬렉션 생성 및 등록
	void InitAudioCollectionTab();                                    // 오디오 탭 컬렉션 생성 및 등록
	void InitVideoCollectionTab();                                    // 비디오 탭 컬렉션 생성 및 등록
	void InitControlCollectionTab(ULocalPlayer* InOwningLocalPlayer); // 컨트롤 탭 컬렉션 생성 및 등록
	void InitSystemCollectionTab(ULocalPlayer* InOwningLocalPlayer);  // 시스템 탭 컬렉션 생성 및 등록

	UPROPERTY(Transient)
	TArray<UListDataObject_Collection*> RegisteredOptionsTabCollections; // 등록된 탭 컬렉션 배열 (GC 추적, 직렬화 제외)
};