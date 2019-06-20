// Fill out your copyright notice in the Description page of Project Settings.

#include "DeckState.h"
#include "Placeables/DeckDatabaseNative.h"
#include "Misc/CanyonGM.h"
#include "Player/RTSPlayerEye.h"
#include "WidgetBase/PlaceableIconWidgetBase.h"
#include "Misc/CanyonBpfLib.h"

//Public-------------------

void UDeckState::ChargeCountDecrementFor(const FString& Category)
{
	auto WidgetClass{ m_pGM->GetPlaceableWidget(Category) };
	auto *pData{ m_DataMapping.Find(WidgetClass) };
	
	checkf(pData, TEXT("Deck state does not contain data for this category"));
	--pData->m_ChargeCount;
	--m_ChargesAmount;

	m_eOnDeckStateChanged.Broadcast(WidgetClass, *pData);

	if(pData->m_ChargeCount <= 0)
	{
		m_DataMapping.Remove(WidgetClass);
	}


}

void UDeckState::ClearCachedPlaceableForCategory(const FString& Category)
{
	m_CachedPlaceableClasses.Remove(Category);


}

void UDeckState::NotifyOnDisplayNewDecks()
{
	const auto NewDeckAmount{ m_DesiredDeckAmount - m_aPendingSelectableDecks.Num() };

	m_aPendingSelectableDecks.Append(m_pGM->GetDeckData(NewDeckAmount));

	//no valid deck exist, assume endless mode
	if(m_aPendingSelectableDecks.Num() == 0)
	{
		AddDeck(m_pGM->GetEndlessDeckData());
		return;


	}
	
	TArray<TSubclassOf<UPrettyWidget>> aDeckWidgetClasses{};
	for(auto &&DeckData : m_aPendingSelectableDecks)
	{
		aDeckWidgetClasses.Add(DeckData.m_DeckWidgetClass);

	}

	m_eOnDeckStateNewDecksSet.Broadcast(aDeckWidgetClasses);


}

void UDeckState::NotifyOnDeckWidgetClicked(int32 Index)
{
	m_pGM->FillUpDeckDataNonEndless(m_aPendingSelectableDecks[Index]);

	AddDeck(m_aPendingSelectableDecks[Index]);
	m_aPendingSelectableDecks.RemoveAt(Index);


}

void UDeckState::ReceiveOnWidgetClicked(const UWidget* pClickedWidget)
{
	auto *pPlayer{ Cast<ARTSPlayerEye>( m_pGM->GetWorld()->GetFirstPlayerController()->GetPawn() ) };

	if(pPlayer->GetIsInPlacement())
	{
		return;
	}

	std::remove_reference_t<decltype(m_DataMapping)::KeyInitType> AsKey{ pClickedWidget->GetClass() };
	auto *pWidgetData{ m_DataMapping.Find(AsKey) };

	checkf(pWidgetData, TEXT("A deck widget has lost its data mapping"));

	//find or add entry
	auto &ClassOf{ m_CachedPlaceableClasses.FindOrAdd(pWidgetData->m_CategoryName) };

	//optional init
	if(!ClassOf.Get())
	{
		ClassOf = UCanyonBpfLib::GetCategoryPlaceableClass(pWidgetData->m_CategoryName);		
	}

	pPlayer->CreateNewPlacablePreview(ClassOf);

	//if the building placement is aborted no further handling is needed.
	//for successful placement, see ARTSPlayerEye::TryCommitPreviewBuilding,
	//where ClearCachedPlaceableForCategory and ChargeCountDecrementFor are called

}

void UDeckState::AddDeck(const FDeckData &Deck)
{	
	
	for(auto &&Entry : Deck.m_ChargeMapping)
	{		
		/*
		auto Category{ pDeck->GetDependencyCategoryAtIndex(DataIndex) };

		//todo: deprecation pending
		auto MinAmount{ pDeck->GetMinAmountAtIndex(DataIndex) };
		auto MaxAmount{ pDeck->GetMaxAmountAtIndex(DataIndex) };
		auto AmountDist{ MaxAmount - MinAmount };
		auto BuildingAmount{ MinAmount + FMath::RoundToInt(FMath::SRand() * AmountDist) };
		//
		*/

		//get widget class
		auto WidgetClass{ m_pGM->GetPlaceableWidget(Entry.Key) };

		//Find or create data node
		FCategoryData *pCatData{ m_DataMapping.Find(WidgetClass) };
		
		if(!pCatData)
		{
			pCatData = &m_DataMapping.Add(WidgetClass, FCategoryData{ Entry.Key });	
		}

		//Update deck data:
			//Add amount
		m_ChargesAmount += Entry.Value;
		pCatData->m_ChargeCount += Entry.Value;
		

		//Notify listeners
		m_eOnDeckStateChanged.Broadcast(WidgetClass, *pCatData);
		
	}

	//Update needed points in gm
	m_pGM->IncreaseDeckGeneration();
	
	
}

UDeckState* UDeckState::Construct(ACanyonGM* pGM)
{
	auto *pObj{ NewObject<UDeckState>() };

	pObj->m_pGM = pGM;

	return pObj;


}

void UDeckState::AddEventToOnStateChanged(t_StateChangedDelegate& Callback)
{
	m_eOnDeckStateChanged.Add(Callback);


}

void UDeckState::RemoveEventFromOnStateChanged(t_StateChangedDelegate& Callback)
{
	m_eOnDeckStateChanged.Remove(Callback);


}

void UDeckState::AddEventToOnDeckStateNewDecksSet(t_NewDecksSetDelegate& Callback)
{
	m_eOnDeckStateNewDecksSet.Add(Callback);


}

void UDeckState::RemoveEventFromOnDeckStateNewDecksSet(t_NewDecksSetDelegate& Callback)
{
	m_eOnDeckStateNewDecksSet.Add(Callback);


}
