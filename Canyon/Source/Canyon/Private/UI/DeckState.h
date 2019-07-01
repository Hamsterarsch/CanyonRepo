// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBase/PlaceableIconWidgetBase.h"
#include "UI/CategoryData.h"
#include "Placeables/DeckSelector.h"
#include "DeckState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDeckStateChangedDelegate, TSubclassOf<UPlaceableIconWidgetBase>, WidgetClassOfChanged, const FCategoryData &, NewData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeckStateNewDecksSetDelegate, const TArray<TSubclassOf<class UPrettyWidget>> &, aWidgetClasses);

/**
 * 
 */
UCLASS()
class UDeckState : public UObject
{
	GENERATED_BODY()

public:
	UDeckState();

	void ChargeCountDecrementFor(const FString &Category);

	void ClearCachedPlaceableForCategory(const FString &Category);

	inline int32 GetChargesCurrent() const { return m_ChargesAmount; }

	inline bool GetAreDecksSelectable() const { return m_bAreDecksSelectable; }

	void NotifyOnDisplayNewDecks();

	void NotifyOnDeckWidgetClicked(int32 Index);


	UFUNCTION()
		void ReceiveOnWidgetClicked(const class UWidget *pClickedWidget);

	UFUNCTION()
		UWidget *GetTooltipWidgetForDeckWidget(const UWidget *pInstigator);

	static UDeckState *Construct(class ACanyonGM *pGM);			


private:
	void AddDeck(const FDeckData &Deck);



	UPROPERTY()
		class ACanyonGM *m_pGM;
	
	UPROPERTY()
		TArray<FDeckData> m_aPendingSelectableDecks;

	const int32 m_DesiredDeckAmount{ 2 };

	UPROPERTY()
		FOnDeckStateChangedDelegate m_eOnDeckStateChanged;

	UPROPERTY()
		FOnDeckStateNewDecksSetDelegate m_eOnDeckStateNewDecksSet;
	
	UPROPERTY()
		TMap<TSubclassOf<class UPlaceableIconWidgetBase>, FCategoryData> m_DataMapping;

	UPROPERTY()
		TMap<FString, TSubclassOf<class APlaceableBase>> m_CachedPlaceableClasses;
	
	int32 m_ChargesAmount;

	bool m_bAreDecksSelectable;


public:
	using t_StateChangedDelegate = decltype(m_eOnDeckStateChanged)::FDelegate;

	using t_NewDecksSetDelegate = decltype(m_eOnDeckStateNewDecksSet)::FDelegate;


	void AddEventToOnStateChanged(t_StateChangedDelegate &Callback);

	void RemoveEventFromOnStateChanged(t_StateChangedDelegate &Callback);

	void AddEventToOnDeckStateNewDecksSet(t_NewDecksSetDelegate &Callback);

	void RemoveEventFromOnDeckStateNewDecksSet(t_NewDecksSetDelegate &Callback);

	
};
