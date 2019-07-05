// Fill out your copyright notice in the Description page of Project Settings.

#include "DeckState.h"
#include "Placeables/DeckDatabaseNative.h"
#include "Misc/CanyonGM.h"
#include "Player/RTSPlayerEye.h"
#include "WidgetBase/PlaceableIconWidgetBase.h"
#include "Misc/CanyonBpfLib.h"
#include "WidgetBase/InfluenceTooltipWidgetBase.h"

//Public-------------------

UDeckState::UDeckState() :
	m_ChargesAmount{ 0 },
	m_bAreDecksSelectable{ false }
{
}

void UDeckState::NotifyOnCategoryPlaceablePlaced(const FString& Category)
{
	auto WidgetClass{ m_pGM->GetPlaceableWidget(Category) };
	auto *pData{ m_DataMapping.Find(WidgetClass) };
	
	checkf(pData, TEXT("Deck state does not contain data for this category"));
	--pData->m_ChargeCount;
	--m_ChargesAmount;

	m_eOnDeckStateChanged.Broadcast(WidgetClass, *pData);

	//clear old instance
	ClearCachedPlaceableForCategory(Category);

	if(pData->m_ChargeCount <= 0)
	{
		m_DataMapping.Remove(WidgetClass);
	}
	else
	{
		//if charges are left, load new placeable instance
		m_CachedPlaceableClasses.Add(Category, UCanyonBpfLib::GetCategoryPlaceableClass(Category));
	}
	
	
}

void UDeckState::NotifyOnDisplayNewDecks()
{
	m_bAreDecksSelectable = true;
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
	m_bAreDecksSelectable = false;
	m_pGM->FillUpDeckDataNonEndless(m_aPendingSelectableDecks[Index]);

	AddDeck(m_aPendingSelectableDecks[Index]);
	m_aPendingSelectableDecks.RemoveAt(Index);


}

void UDeckState::DebugAddChargesForCategory(const FString& Category, const int32 Num)
{
	FDeckData Data{};
	Data.m_ChargeMapping.Add(Category, Num);

	AddDeck(Data);


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
	auto *pClassOf{ m_CachedPlaceableClasses.Find(pWidgetData->m_CategoryName) };

	if(!pClassOf || !pClassOf->Get())
	{
		UE_LOG(LogTemp, Warning, TEXT("deck state widget clicked, but placeable class not getable"));
	}
	pPlayer->CreateNewPlacablePreview(*pClassOf);

	//if the building placement is aborted no further handling is needed.
	//for successful placement, see ARTSPlayerEye::TryCommitPreviewBuilding,
	//where ClearCachedPlaceableForCategory and NotifyOnCategoryPlaceablePlaced are called

}

UWidget* UDeckState::GetTooltipWidgetForDeckWidget(const UWidget* pInstigator)
{
	std::remove_reference_t<decltype(m_DataMapping)::KeyInitType> AsKey{ pInstigator->GetClass() };
	auto *pWidgetData{ m_DataMapping.Find(AsKey) };
	checkf(pWidgetData, TEXT("A deck widget has lost its data mapping"));

	auto *pTooltipClass{ LoadClass<UInfluenceTooltipWidgetBase>(nullptr, TEXT("WidgetBlueprint'/Game/Widgets/TooltipWidget_BP.TooltipWidget_BP_C'")) };
	auto *pWidget { CreateWidget<UInfluenceTooltipWidgetBase>(m_pGM->GetWorld(), pTooltipClass) };
	
	pWidget->SetHeaderName(m_pGM->GetPrettyNameForCategory(pWidgetData->m_CategoryName));
	pWidget->SetInfluenceRelationships(m_pGM->GetTempInfluenceMappingForCategory(pWidgetData->m_CategoryName));

	return pWidget;
	

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
			//todo: maybe dont cache the complete deck but only buildings that had placement aborted, if all buildings will be preloaded by the game instance in the future.
			pCatData = &m_DataMapping.Add(WidgetClass, FCategoryData{ Entry.Key });

			//also pre load an instance of this placeable class and cache it
			auto &ClassOf{ m_CachedPlaceableClasses.Add(Entry.Key) };

			//init
			ClassOf = UCanyonBpfLib::GetCategoryPlaceableClass(Entry.Key);	
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

void UDeckState::ClearCachedPlaceableForCategory(const FString& Category)
{
	m_CachedPlaceableClasses.Remove(Category);


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
