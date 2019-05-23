// Fill out your copyright notice in the Description page of Project Settings.

#include "CanyonGM.h"
#include "ModuleManager.h"
#include "IAssetRegistry.h"
#include "InfluenceDataObject.h"
#include "AssetRegistryModule.h"
#include "Placeables/DeckDatabaseNative.h"
#include "WidgetBase/DeckWidgetBase.h"
#include <set>

//Public-------------

ACanyonGM::ACanyonGM() :
	m_PointsCurrent{ 0 },
	m_PointsRequired{ 0 },
	m_DeckGenerationCurrent{ 0 }
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

void ACanyonGM::AddPointsCurrent(const int32 Points)
{
	m_PointsCurrent += Points;
	ReceiveOnPointsChanged();


}

void ACanyonGM::AddPointsRequired(const int32 Points)
{
	m_PointsRequired += Points;
	ReceiveOnPointsChanged();


}

void ACanyonGM::OnDeckSelected(int32 DeckIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Deck Name: %s"), *m_apCurrentDeckData[DeckIndex]->GetName());
	m_pDeckWidget->HideWidget();


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

	//ReceiveOnInvokeNewDecks();


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
		ReceiveOnInvokeNewDecks();
	}


}

void ACanyonGM::ReceiveOnInvokeNewDecks()
{
	m_pDeckWidget->ShowWidget();
	 m_apCurrentDeckData = OnInvokeNewDecks(m_DeckGenerationCurrent);

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
	
	Filter.PackagePaths.Add(*(TEXT("/Game/Placeables/Decks") + SubCategory));
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


//Global----------------------

int32 GetRandomIndex(int32 ArrSize)
{
	return FMath::FloorToInt((static_cast<float>(FMath::Rand()) / RAND_MAX) * (ArrSize - 1));


}
