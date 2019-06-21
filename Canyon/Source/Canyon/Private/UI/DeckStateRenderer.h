// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/CategoryData.h"
#include "DeckStateRenderer.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeckWidgetClickedDelegate, const class UWidget *, pClickedWidget);

/**
 * 
 */
UCLASS()
class UDeckStateRenderer : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION()
		void NotifyOnIconWidgetChanged(TSubclassOf<class UPlaceableIconWidgetBase> WidgetClass, const struct FCategoryData &Data);

	UFUNCTION()
		void NotifyOnNewDecksSet(const TArray<TSubclassOf<class UPrettyWidget>> &aWidgetClasses);

	UFUNCTION()
		void NotifyOnIconClicked(const class UWidget *pClickedWidget);

	UFUNCTION()
		void NotifyOnDeckClicked(class UPrettyWidget *pClickedWidget);


	static UDeckStateRenderer *Construct(class ACanyonGM *pGM, class UDeckState *pDeckState, class UMainHudWidgetBase *pTargetWidget);


private:
	UPROPERTY()
		class ACanyonGM *m_pGM;

	UPROPERTY()
		class UDeckState *m_pDeckState;

	UPROPERTY()
		class UMainHudWidgetBase *m_pTargetWidget;

	UPROPERTY()
		TMap<TSubclassOf<class UPlaceableIconWidgetBase>, UPlaceableIconWidgetBase *> m_IconWidgetMap;

	UPROPERTY()
		TArray<class UPrettyWidget *> m_aPendingSelectableDeckWidgets;

	FDeckWidgetClickedDelegate m_eOnIconClicked;


public:
	using t_OnIconClickedDelegate = decltype(m_eOnIconClicked)::FDelegate;

	void AddEventToOnClicked(t_OnIconClickedDelegate &Callback);

	void RemoveEventFromOnClicked(t_OnIconClickedDelegate &Callback);
	

};
