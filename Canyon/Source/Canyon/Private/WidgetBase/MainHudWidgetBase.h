#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetBase/PrettyWidget.h"
#include "MainHudWidgetBase.generated.h"



/**
 *
 */
UCLASS()
class UMainHudWidgetBase: public UPrettyWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		void OnNewPlaceableIconAvailable(class UPlaceableIconWidgetBase *pIconWidget);

	UFUNCTION(BlueprintImplementableEvent)
		void OnPlaceableIconUnavailable(class UPlaceableIconWidgetBase *pIconWidget);

	UFUNCTION(BlueprintImplementableEvent)
		void OnNewDecksSet(UPARAM(ref) TArray<class UPrettyWidget *> &DeckWidgets);

	UFUNCTION(BlueprintImplementableEvent)
		void OnDeckSelected(int32 Index);

	UFUNCTION(BlueprintImplementableEvent)
		void OnPointsCurrentChanged(int32 NewCurrentPoints);

	UFUNCTION(BlueprintImplementableEvent)
		void OnPointsRequiredChanged(int32 NewPointsRequired);

	UFUNCTION(BlueprintImplementableEvent)
		void OnNextLevelAccessible();

	UFUNCTION(BlueprintImplementableEvent)
		void SwitchToPlaceableSelectionUI();

	UFUNCTION(BlueprintImplementableEvent)
		void SwitchToPlaceablePlacementUI();

	UFUNCTION(BlueprintImplementableEvent)
		void OnPlaceableSelectionChanged(int32 CountSelectedCurrent, int32 CountSelectedMax);

	
};

