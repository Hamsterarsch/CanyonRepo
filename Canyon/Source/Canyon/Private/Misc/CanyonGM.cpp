// Fill out your copyright notice in the Description page of Project Settings.

#include "CanyonGM.h"
#include "ModuleManager.h"
#include "IAssetRegistry.h"
#include "InfluenceDataObject.h"
#include "AssetRegistryModule.h"
#include "Placeables/DeckDatabaseNative.h"
#include "Misc/CanyonHelpers.h"
#include "WidgetBase/DeckWidgetBase.h"
#include "WidgetBase/PlaceableWidgetBase.h"
#include "WidgetBase/PointIndicatorWidgetBase.h"
#include "WidgetBase/PrettyWidget.h"
#include <set>

//Public-------------

ACanyonGM::ACanyonGM() :
	m_PointsCurrent{ 0 },
	m_PointsRequired{ 0 },
	m_DeckGenerationCurrent{ 0 },
	m_BuildingsRemaining{ 0 },
	m_bIsDeckSelectPending{ true }
{
}

int32 ACanyonGM::GetInfluenceForPlaceable
(
	const FString &FirstInfluenceQualifier,
	const FString &SecondInfluenceQualifier
)	const
{
	return (*m_pInfluenceData)[FirstInfluenceQualifier][SecondInfluenceQualifier];
	

}

int32 ACanyonGM::GetInfluenceForBaseCategory(const FString& CategoryName) const
{
	return (*m_pInfluenceData)[CategoryName].m_BasePointAmount;


}

void ACanyonGM::AddPointsCurrent(const int32 Points)
{	
	m_PointsCurrent += Points;
	m_pPointWidget->OnPointsCurrentChanged(m_PointsCurrent);

	//equivalent with 'a building was placed'
	--m_BuildingsRemaining;

	ReceiveOnPointsChanged();


}

void ACanyonGM::AddPointsRequired(const int32 Points)
{
	m_PointsRequired += Points;
	m_pPointWidget->OnPointsRequiredChanged(m_PointsRequired);

	ReceiveOnPointsChanged();


}

void ACanyonGM::OnLoose()
{
	m_pLooseWidget->ShowWidget();


}

void ACanyonGM::SelectNewDeck()
{
	if(m_PointsCurrent >= m_PointsRequired)
	{
		ReceiveOnInvokeNewDecks();	
	}

	if(m_BuildingsRemaining <= 0 && !m_bIsDeckSelectPending)
	{
		OnLoose();
		//loose condition
	}


}

void ACanyonGM::OnDeckSelected(const int32 DeckIndex)
{
	m_bIsDeckSelectPending = false;
	UE_LOG(LogTemp, Warning, TEXT("Deck Name: %s"), *m_apCurrentDeckData[DeckIndex]->GetName());

	auto *pSelectedDeck{ m_apCurrentDeckData[DeckIndex] };
	auto NumDeckData{ pSelectedDeck->GetNumData() };

	for(int32 DataIndex{ 0 }; DataIndex < NumDeckData; ++DataIndex)
	{		
		auto Category{ pSelectedDeck->GetDependencyCategoryAtIndex(DataIndex) };
		
		auto MinAmount{ pSelectedDeck->GetMinAmountAtIndex(DataIndex) };
		auto MaxAmount{ pSelectedDeck->GetMaxAmountAtIndex(DataIndex) };
		auto AmountDist{ MaxAmount - MinAmount };
		auto BuildingAmount{ MinAmount + FMath::RoundToInt(static_cast<float>(FMath::Rand()) / RAND_MAX * AmountDist) };

		//Add amount
		m_BuildingsRemaining += BuildingAmount;

		//Spawn widget
		TSubclassOf<UUserWidget> AsSubclass{ SafeLoadClassPtr(GetPlaceableWidget(Category)) };
		m_pPlaceableWidget->AddPlaceableWidget(AsSubclass, BuildingAmount, Category);

		//Update needed points
		AddPointsRequired(BuildingAmount * (*m_pInfluenceData)[Category].m_BasePointRequirement);


	}

	m_pDeckWidget->HideWidget();
	m_bIsDeckSelectPending = false;


}

float ACanyonGM::GetPlaceableDependencyRadius(const FString& CategoryName) const
{
	return (*m_pInfluenceData)[CategoryName].m_InfluenceRadius;


}

TSoftClassPtr<UUserWidget> ACanyonGM::GetPlaceableWidget(const FString& CategoryName) const
{
	return (*m_pInfluenceData)[CategoryName].m_UiWidgetClass;


}


//Protected-------------------

void ACanyonGM::BeginPlay()
{
	Super::BeginPlay();

	m_pInfluenceData = UInfluenceDataObject::CreateFromFile();
	m_pDeckWidget = CreateWidget<UDeckWidgetBase>(GetWorld(), m_DeckWidgetClass.Get());
	m_pDeckWidget->m_OnDeckSelected.AddDynamic(this, &ACanyonGM::OnDeckSelected);
	m_pDeckWidget->HideWidget();
	m_pDeckWidget->AddToViewport();

	m_pPlaceableWidget = CreateWidget<UPlaceableWidgetBase>(GetWorld(), m_PlaceableWidgetClass.Get());
	m_pPlaceableWidget->AddToViewport();

	m_pPointWidget = CreateWidget<UPointIndicatorWidgetBase>(GetWorld(), m_PointIndicatorWidgetClass.Get());
	m_pPointWidget->AddToViewport();

	m_pLooseWidget = CreateWidget<UPrettyWidget>(GetWorld(), m_LooseWidgetClass.Get());
	m_pLooseWidget->HideWidget();
	m_pLooseWidget->AddToViewport();

	//ReceiveOnInvokeNewDecks();
	m_OnRequiredPointsReached.Broadcast();


}

TArray<UDeckDatabaseNative *> ACanyonGM::OnInvokeNewDecks_Implementation(const int32 CurrentDeckGeneration)
{	
	return GetRandomDecks(2);


}


//Private----------------------

void ACanyonGM::ReceiveOnPointsChanged()
{
	if(m_PointsCurrent >= m_PointsRequired)
	{
		m_OnRequiredPointsReached.Broadcast();
	}


}

void ACanyonGM::ReceiveOnInvokeNewDecks()
{
	m_pDeckWidget->ShowWidget();
	m_apCurrentDeckData = OnInvokeNewDecks(m_DeckGenerationCurrent);

	m_bIsDeckSelectPending = true;

	for(auto *pDeck : m_apCurrentDeckData)
	{
		m_pDeckWidget->ReceiveOnInvokeDeck(pDeck->GetDeckWidget());

	}
	
	++m_DeckGenerationCurrent;


}

TArray<UDeckDatabaseNative *> ACanyonGM::GetRandomDecks(const int32 NumDecks, FString SubCategory) const
{
	//get data assets
	auto &Registry{ FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")) };
	
	FARFilter Filter{};
	Filter.ClassNames.Add(*UDeckDatabaseNative::StaticClass()->GetName());
	Filter.bRecursiveClasses = true;

	if(!SubCategory.IsEmpty())
	{
		SubCategory.InsertAt(0, '/');
	}
	
	Filter.PackagePaths.Add( *(TEXT("/Game/Placeables/Decks") + SubCategory));
	Filter.bRecursivePaths = true;
	
	TArray<FAssetData> aFoundAssets;
	Registry.Get().GetAssets(Filter, aFoundAssets);

	if(aFoundAssets.Num() < NumDecks)
	{
		return {};
	}


	//populate set	
	std::set<int32> UnusedIndicesSet{};
	for(int32 Index{ 0 }; Index < aFoundAssets.Num(); ++Index)
	{
		UnusedIndicesSet.emplace(Index);

	}


	//get random decks
	TArray<UDeckDatabaseNative *> aOutDecks{};
	for(int32 DeckIndex{ 0 }; DeckIndex < NumDecks; ++DeckIndex)
	{
		auto SetOffset{ GetRandomIndex(UnusedIndicesSet.size()) };
		auto SetItr{ UnusedIndicesSet.begin() };

		//retarded itr increment
		for(int32 Index{ 0 }; Index < SetOffset; ++Index)
		{
			++SetItr;

		}

		aOutDecks.Add
		(
			Cast<UDeckDatabaseNative>
			(
				aFoundAssets[ *SetItr ].GetAsset()
			)
		);
		UnusedIndicesSet.erase(SetItr);		

	}

	return aOutDecks;


}
