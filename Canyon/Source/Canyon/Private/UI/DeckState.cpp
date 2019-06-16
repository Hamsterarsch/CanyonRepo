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
	m_apPendingSelectableDecks = UCanyonBpfLib::GetRandomDecks(2);
	
	TArray<TSubclassOf<UPrettyWidget>> aDeckWidgetClasses{};
	for(auto *pDeckData : m_apPendingSelectableDecks)
	{
		aDeckWidgetClasses.Add(pDeckData->GetDeckWidget());

	}

	m_eOnDeckStateNewDecksSet.Broadcast(aDeckWidgetClasses);


}

void UDeckState::NotifyOnDeckWidgetClicked(int32 Index)
{
	AddDeck(m_apPendingSelectableDecks[Index]);
	m_apPendingSelectableDecks.Reset();


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

void UDeckState::AddDeck(const UDeckDatabaseNative* pDeck)
{	
	auto NumDeckData{ pDeck->GetNumData() };
	
	for(int32 DataIndex{ 0 }; DataIndex < NumDeckData; ++DataIndex)
	{		
		auto Category{ pDeck->GetDependencyCategoryAtIndex(DataIndex) };
		
		auto MinAmount{ pDeck->GetMinAmountAtIndex(DataIndex) };
		auto MaxAmount{ pDeck->GetMaxAmountAtIndex(DataIndex) };
		auto AmountDist{ MaxAmount - MinAmount };
		auto BuildingAmount{ MinAmount + FMath::RoundToInt(FMath::SRand() * AmountDist) };
				
		//get widget class
		auto WidgetClass{ m_pGM->GetPlaceableWidget(Category) };

		//Find or create data node
		FCategoryData *pCatData{ m_DataMapping.Find(WidgetClass) };
		
		if(!pCatData)
		{
			pCatData = &m_DataMapping.Add(WidgetClass, FCategoryData{ Category });	
		}

		//Update deck data:
			//Add amount
		m_ChargesAmount += BuildingAmount;
		pCatData->m_ChargeCount += BuildingAmount;
		
		//Update needed points in gm
		m_pGM->AddPointsRequiredFor(Category, static_cast<uint32>(BuildingAmount));

		//Notify listeners
		m_eOnDeckStateChanged.Broadcast(WidgetClass, *pCatData);
		
	}
	
	
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
