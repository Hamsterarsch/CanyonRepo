// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WidgetBase/PlaceableIconWidgetBase.h"
#include "UI/CategoryData.h"
#include "Placeables/DeckSelector.h"
#include "DeckState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDeckStateChangedDelegate, TSubclassOf<UPlaceableIconWidgetBase>, WidgetClassOfChanged, const FCategoryData &, NewData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeckStateNewDecksSetDelegate, const TArray<TSubclassOf<class UPrettyWidget>> &, aWidgetClasses);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeckChargeAddedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeckCommitted, const FDeckData &, CommitedDeck);

/**
 * 
 */
UCLASS()
class UDeckState : public UObject
{
	GENERATED_BODY()

public:
	UDeckState();

	void NotifyOnCategoryPlaceablePlaced(const FString &Category);
	
	inline int32 GetChargesCurrent() const { return m_ChargesAmount; }

	inline int32 GetDeckChargesCurrent() const { return m_DeckChargeCount; }

	inline bool GetAreDecksSelectable() const { return m_aPendingSelectableDecks.Num() > 0; }

	void AddDeckCharge();

	UFUNCTION()
		void NotifyAddDeckButtonClicked();

	void NotifyOnDeckWidgetClicked(int32 Index);

	void DebugAddChargesForCategory(const FString &Categoty, int32 Num);

	void AddCarryOverCharges(const FCarryOverCharges &CarryCharges);

	UFUNCTION()
		void ReceiveOnWidgetClicked(const class UWidget *pClickedWidget);

	UFUNCTION()
		UWidget *GetTooltipWidgetForDeckWidget(const UWidget *pInstigator);

	static UDeckState *Construct(class ACanyonGM *pGM);			

	UPROPERTY()
		FDeckChargeAddedDelegate m_eOnDeckChargeAdded;

	UPROPERTY()
		FOnDeckCommitted m_eOnDeckCommitted;


private:
	void AddDeck(const FDeckData &Deck);

	//either added an instance class to the instance list for this category
	//or loads a new instance and adds it (ppInstanceClass == nullptr)
	void AppendPendingPlaceableInstance(const FString &Category, const TSubclassOf<class APlaceableBase> *ppInstanceClass = nullptr);

	TSubclassOf<class APlaceableBase> PeekPendingPlaceableInstance(const FString &Category) const;

	TSubclassOf<class APlaceableBase> PopPendingPlaceableInstance(const FString &Category);

	int32 GetPendingInstanceNumForCategory(const FString &Category) const;

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
		TMap<FString, FChargeWrapper> m_PendingPlaceableInstances;
	
	int32 m_ChargesAmount;

	int32 m_DeckChargeCount;


public:
	using t_StateChangedDelegate = decltype(m_eOnDeckStateChanged)::FDelegate;

	using t_NewDecksSetDelegate = decltype(m_eOnDeckStateNewDecksSet)::FDelegate;


	void AddEventToOnStateChanged(t_StateChangedDelegate &Callback);

	void RemoveEventFromOnStateChanged(t_StateChangedDelegate &Callback);

	void AddEventToOnDeckStateNewDecksSet(t_NewDecksSetDelegate &Callback);

	void RemoveEventFromOnDeckStateNewDecksSet(t_NewDecksSetDelegate &Callback);

	
};
