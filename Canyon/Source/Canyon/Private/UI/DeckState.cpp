// Fill out your copyright notice in the Description page of Project Settings.

#include "DeckState.h"
#include "Placeables/DeckDatabaseNative.h"
#include "Misc/CanyonGM.h"
#include "Player/RTSPlayerEye.h"
#include "WidgetBase/PlaceableIconWidgetBase.h"
#include "Misc/CanyonBpfLib.h"
#include "Misc/CanyonLogs.h"
#include "WidgetBase/InfluenceTooltipWidgetBase.h"

//Public-------------------

UDeckState::UDeckState() :
	m_aPendingSelectableDecks{},
	m_ChargesAmount{ 0 },
	m_DeckChargeCount{ 0 }
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

	//pop old instance
	PopPendingPlaceableInstance(Category);
	

	if(pData->m_ChargeCount <= 0)
	{
		m_DataMapping.Remove(WidgetClass);
	}
	else if(GetPendingInstanceNumForCategory(Category) == 0)
	{
		//if charges are left but there are no pending instances
		//load a new placeable instance
		AppendPendingPlaceableInstance(Category);			
	}
	
	
}

void UDeckState::AddDeckCharge()
{
	++m_DeckChargeCount;
	m_eOnDeckChargeAdded.Broadcast();


}

void UDeckState::NotifyAddDeckButtonClicked()
{
	if(m_pGM->IsInEndlessMode() && m_aPendingSelectableDecks.Num() == 0)
	{
		UE_LOG(LogCanyonCommon, Log, TEXT("Using endless filler deck"));

		auto EndlessDeckData{ m_pGM->GetEndlessDeckData() };
		m_eOnDeckCommitted.Broadcast(EndlessDeckData);
		AddDeck(EndlessDeckData);
		return;


	}

	if(m_aPendingSelectableDecks.Num() >= m_DesiredDeckAmount)
	{
		return;


	}

	const auto NewDeckAmount{ m_DesiredDeckAmount - m_aPendingSelectableDecks.Num() };

	m_aPendingSelectableDecks.Append(m_pGM->GetDeckData(NewDeckAmount));
	
	TArray<TSubclassOf<UPrettyWidget>> aDeckWidgetClasses{};
	for(auto &&DeckData : m_aPendingSelectableDecks)
	{
		aDeckWidgetClasses.Add(DeckData.m_DeckWidgetClass);

	}

	m_eOnDeckStateNewDecksSet.Broadcast(aDeckWidgetClasses);


}

void UDeckState::NotifyOnDeckWidgetClicked(int32 Index)
{
	--m_DeckChargeCount;
	check(m_DeckChargeCount >= 0);

	m_pGM->FillUpDeckDataNonEndless(m_aPendingSelectableDecks[Index]);

	AddDeck(m_aPendingSelectableDecks[Index]);
	m_eOnDeckCommitted.Broadcast(m_aPendingSelectableDecks[Index]);
	m_aPendingSelectableDecks.RemoveAt(Index);


}

void UDeckState::DebugAddChargesForCategory(const FString& Category, const int32 Num)
{
	FDeckData Data{};
	Data.m_ChargeMapping.Add(Category, Num);

	AddDeck(Data);


}

void UDeckState::AddCarryOverCharges(const FCarryOverCharges& CarryCharges)
{
	for(auto &&Entry : CarryCharges.m_Charges)
	{
		for(auto &pSubclass : Entry.Value.Data)
		{
			AppendPendingPlaceableInstance(Entry.Key, &pSubclass);

		}

		//get widget class
		auto WidgetClass{ m_pGM->GetPlaceableWidget(Entry.Key) };

		//Find or create data node
		FCategoryData *pCatData{ m_DataMapping.Find(WidgetClass) };
		
		if(!pCatData)
		{
			//todo: maybe dont cache the complete deck but only buildings that had placement aborted, if all buildings will be preloaded by the game instance in the future.
			pCatData = &m_DataMapping.Add(WidgetClass, FCategoryData{ Entry.Key });
						

		}

		//Update deck data:
			//Add amount
		m_ChargesAmount += Entry.Value.Data.Num();
		pCatData->m_ChargeCount += Entry.Value.Data.Num();
		
		//Notify listeners
		m_eOnDeckStateChanged.Broadcast(WidgetClass, *pCatData);

	}


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
	
	auto pClassOf{ PeekPendingPlaceableInstance(pWidgetData->m_CategoryName) };

	if(!pClassOf || !pClassOf.Get())
	{
		UE_LOG(LogTemp, Warning, TEXT("deck state widget clicked, but placeable class not getable"));
	}
	pPlayer->CreateNewPlacablePreview(pClassOf);

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

			//there is no deck data for this cat so there shouldn't be any pending instances either
			//so load an instance of this placeable class and save it
			AppendPendingPlaceableInstance(pCatData->m_CategoryName);

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

void UDeckState::AppendPendingPlaceableInstance(const FString &Category, const TSubclassOf<class APlaceableBase> *ppInstanceClass)
{	
	auto &InstanceListWrapper{ m_PendingPlaceableInstances.FindOrAdd(Category) };

	if(ppInstanceClass)
	{
		InstanceListWrapper.Data.Add(*ppInstanceClass);		
	}
	else
	{
		InstanceListWrapper.Data.Add(UCanyonBpfLib::GetCategoryPlaceableClass(Category));
	}

	

}

TSubclassOf<APlaceableBase> UDeckState::PeekPendingPlaceableInstance(const FString &Category) const
{
	auto *pEntry{m_PendingPlaceableInstances.Find(Category) };

	if(!pEntry)
	{
		return nullptr;
	}

	//Existing entries are assumed to have at least one entry	
	return pEntry->Data[0];


}

TSubclassOf<APlaceableBase> UDeckState::PopPendingPlaceableInstance(const FString &Category)
{
	auto *pEntry{m_PendingPlaceableInstances.Find(Category) };

	if(!pEntry)
	{
		return nullptr;
	}

	return pEntry->Data.Pop();


}

int32 UDeckState::GetPendingInstanceNumForCategory(const FString& Category) const
{
	auto *pEntry{  m_PendingPlaceableInstances.Find(Category) };
	if(!pEntry)
	{
		return 0;
	}

	return pEntry->Data.Num();


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
