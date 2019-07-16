#include "Placeables/DeckSelector.h"
#include "AssetRegistryModule.h"
#include "Placeables/DeckDatabaseNative.h"
#include "CanyonHelpers.h"
#include "Placeables/InfluenceFloatMapDAL.h"
#include "Runtime/Engine/Classes/Curves/CurveFloat.h"
#include <list>

void UDeckSelector::Init()
{
	//fetch and categorize all available decks
	for(auto &&DataTuple : FetchAllDecks())
	{	
		if(DataTuple.Value->GetRequiredDeckGeneration() <= m_DeckGeneration)
		{
			m_aDecksValid.Emplace(std::move(DataTuple.Key), DataTuple.Value->m_RelativeProbability, DataTuple.Value->GetRequiredDeckGeneration());
		}
		else
		{
			m_aDecksInvalid.Emplace(std::move(DataTuple.Key), DataTuple.Value->m_RelativeProbability, DataTuple.Value->GetRequiredDeckGeneration());
		}
			   
	}

	RegenerateValidDeckData();
		

}

UDeckSelector* UDeckSelector::Construct(UClass* pClass)
{
	UDeckSelector *pObj;
	if(!pClass)
	{
		pObj = NewObject<UDeckSelector>();
	}
	else
	{
		pObj = NewObject<UDeckSelector>(Cast<UObject>(GetTransientPackage()), pClass);		
	}

	pObj->Init();
	return pObj;


}

UDeckSelector::UDeckSelector() :
	m_DeckGeneration{ 0 }
{
}

TArray<FDeckData> UDeckSelector::GetDeckData(int32 ForAmount)
{
	if(m_aDecksValid.Num() <= 0)
	{
		return {};


	}

	TArray<FDeckData> aOutData{};

	//track decks to remove from valid
	std::list<int32> UsedIndicesList{};
	
	if(m_aDecksValid.Num() > ForAmount)
	{
		//get specified amount of deck data
		for(int32 Index{0}; Index < ForAmount; ++Index)
		{
			m_ValidDeckIndexSampleSourceSet.Compact();
			auto PickedIndex{GetRandomIndexSeeded(m_ValidDeckIndexSampleSourceSet.Num()) };
			auto AsSetIndex{  FSetElementId::FromInteger(PickedIndex) };

			auto DeckIndex{ m_ValidDeckIndexSampleSourceSet[AsSetIndex] };
			m_ValidDeckIndexSampleSourceSet.Remove(DeckIndex);
			
			aOutData.Add(GetDeckDataFromValidDeckAt(DeckIndex));

			//remove the decks from valid later in order to not invalidate the probability set every pick
			UsedIndicesList.push_front(DeckIndex);

		}
	}
	else
	{
		//get deck data from remaining
		for(int32 ValidDeckIndex{ 0 }; ValidDeckIndex < m_aDecksValid.Num(); ++ValidDeckIndex)
		{
			aOutData.Add( GetDeckDataFromValidDeckAt(ValidDeckIndex) );
			
			UsedIndicesList.push_front(ValidDeckIndex);

		}
	}

	//remove old decks
	//has to be in order from highest to lowest index to not invalidate the remaining indices
	UsedIndicesList.sort([](int32 &Lhs, int32 &Rhs)
	{
		return Lhs > Rhs;
	});

	for(auto &&IndexToRemove : UsedIndicesList)
	{
		m_aDecksValid.RemoveAt(IndexToRemove);

	}

	RegenerateValidDeckData();
		
	return aOutData;


}

void UDeckSelector::FillUpDeckDataNonEndless(FDeckData &DeckData)
{
	auto *pDeckTemplate{ Cast<UDeckDatabaseNative>(DeckData.m_DeckAssetPath.TryLoad()) };

	const auto FillerAmount{ m_pFillerBuildingAmountSource->GetFloatValue(m_DeckGeneration) };

	if(FillerAmount > 0)
	{
		AddFillerChargesForDeck(FillerAmount, DeckData, pDeckTemplate);		
	}
	else if(FillerAmount == 0)
	{
		//need to add to issue here too
		for(auto &&Entry : DeckData.m_ChargeMapping)
		{
			AddToIssuedChargesForCategory(Entry.Key, Entry.Value);

		}
	}
	

}

FDeckData UDeckSelector::GetEndlessDeckData()
{	
	auto FillerBuildingAmount{ FMath::RoundToInt(m_pFillerBuildingAmountSource->GetFloatValue(m_DeckGeneration)) };

	FDeckData OutData{};
	if(FillerBuildingAmount <= 0)
	{
		return OutData;
	}

	AddFillerChargesToDeckData(FillerBuildingAmount, OutData);
	return OutData;


}

void UDeckSelector::IncreaseDeckGeneration()
{
	++m_DeckGeneration;
	SearchForNewValidDecks();


}

void UDeckSelector::AddCarryOverChargesToIssued(const FCarryOverCharges &ChargeData)
{
	for(auto &&Entry : ChargeData.m_Charges)
	{
		auto &IssuedChargeCount{ m_IssuedChargesMap.FindOrAdd(Entry.Key) };
		IssuedChargeCount += Entry.Value.Data.Num();
		
	}




}

void UDeckSelector::AddFillerChargesToDeckData(const int32 FillerChargeCount, FDeckData &DeckData, UDeckDatabaseNative *pDeckTemplate)
{	
	//trivial case
	if(FillerChargeCount <= 0)
	{
		return;


	}
	
	if(pDeckTemplate)
	{
		//postponed for non endless decks
		//AddFillerChargesForDeck(FillerChargeCount, DeckData, pDeckTemplate);
	}
	else
	{
		AddFillerChargesForEndless(FillerChargeCount, DeckData);
	}


}

void UDeckSelector::AddFillerChargesForEndless(const int32 FillerChargeCount, FDeckData& DeckData)
{
	//construct cache if needed
	if
	(		
		m_aEndlessFillerCats.Num() == 0
		&& m_aEndlessFillerProbs.Num() == 0	
	)
	{
		auto FillerProbMap{ m_pFillerChargesProbForEndless->GetMap() };
		if(FillerProbMap.Num() > 0)
		{
			FillerProbMap.GenerateKeyArray(m_aEndlessFillerCats);
			FillerProbMap.GenerateValueArray(m_aEndlessFillerProbs);

			//build the prob set
			m_EndlessFillerProbSampleSet.Reset();

			//approximate memory
			m_EndlessFillerProbSampleSet.Reserve(m_aEndlessFillerCats.Num() * m_aEndlessFillerProbs.Num());

			//parallel iteration
			for(int32 CatIndex{ 0 }; CatIndex < m_aEndlessFillerCats.Num(); ++CatIndex)
			{
				//Add the index to the cat x times to the set to scale the probability
				for(int32 TimesAdded{ 0 }; TimesAdded < m_aEndlessFillerProbs[CatIndex]; ++TimesAdded)
				{
					m_EndlessFillerProbSampleSet.Add(CatIndex);

				}

			}
		}
		else
		{
			return;
		}
	}
		
	AddFillerCharges(FillerChargeCount, DeckData, m_aEndlessFillerCats, m_aEndlessFillerProbs, m_EndlessFillerProbSampleSet, true);


}

void UDeckSelector::AddFillerChargesForDeck(const int32 FillerChargeCount, FDeckData &DeckData, UDeckDatabaseNative *pDeckTemplate)
{
	//trivial case
	auto FillerProbMap{ pDeckTemplate->GetRelativeFillerProbabilites() };
	   
	if(FillerProbMap.Num() <= 0 || FillerChargeCount <= 0)
	{
		return;


	}

	//include categories unlocked with this deck into the issue list
	//(so they can be filled in this deck too)
	for(auto Entry : DeckData.m_ChargeMapping)
	{		
		AddToIssuedChargesForCategory(Entry.Key, Entry.Value);

	}

	//only use unlocked buildings
	ApplyFillerMapConstraints(FillerProbMap);
	
	if(FillerProbMap.Num() > 0)
	{
		//reparse map data
		TArray<FString> aProbMapCategories;
		FillerProbMap.GenerateKeyArray(aProbMapCategories);
		
		TArray<int32> aFillerProbMapValues;
		FillerProbMap.GenerateValueArray(aFillerProbMapValues);
		   
		//generate sample source
		TSet<int32, DefaultKeyFuncs<int32, true>> FillerSampleSourceSet{};

		//approximate memory needed
		FillerSampleSourceSet.Reserve(aProbMapCategories.Num() * aFillerProbMapValues.Num());

		for(int32 CatIndex{ 0 }; CatIndex < aProbMapCategories.Num(); ++CatIndex)
		{
			for(int32 TimesAdded{ 0 }; TimesAdded < aFillerProbMapValues[CatIndex]; ++TimesAdded)
			{
				FillerSampleSourceSet.Add(CatIndex);
				
			}

		}

		//dont add filler charges to issued
		AddFillerCharges(FillerChargeCount, DeckData, aProbMapCategories, aFillerProbMapValues, FillerSampleSourceSet, true);
	}

	

}

void UDeckSelector::AddFillerCharges
(
	const int32 FillerChargeCount, 
	FDeckData &DeckData, 
	const TArray<FString> &aFillerCat, 
	const TArray<int32> &aFillerProbs, 
	TSet<int32, DefaultKeyFuncs<int32, true>> &FillerProbSampleSet,
	const bool bAddFillersToIssued
)
{
	//compact set to enable indexed access
	FillerProbSampleSet.Compact();

	//pick x filler buildings
	for(int32 NumPickedFillers{ 0 }; NumPickedFillers < FillerChargeCount; ++NumPickedFillers)
	{		
		const auto PickedIndex{ GetRandomIndexSeeded(FillerProbSampleSet.Num()) };		
		auto AsSetIndex{ FSetElementId::FromInteger(PickedIndex)};

		auto FillerCategory{ aFillerCat[ FillerProbSampleSet[AsSetIndex] ] };

		//constraint charge amount per map
		int32 NumCharges{ 1 };
		auto MaxChargeTypeOnMap{ static_cast<int32>(m_pPlaceableMaxPerMap->GetValueForCategory(FillerCategory)) };
		if(MaxChargeTypeOnMap >= 1)
		{
			auto *pIssueMapItem{ m_IssuedChargesMap.Find(FillerCategory) };
			auto ChargesAlreadyIssued{ pIssueMapItem ? *pIssueMapItem : 0 };

			NumCharges = FMath::Clamp(NumCharges, 0, MaxChargeTypeOnMap - ChargesAlreadyIssued);
		}
		checkf(NumCharges >= 0, TEXT("Deck Selector charges smaller than zero"));
			   
		auto &Value{ DeckData.m_ChargeMapping.FindOrAdd(FillerCategory) };
		Value += NumCharges;

		if(bAddFillersToIssued)
		{
			AddToIssuedChargesForCategory(FillerCategory, NumCharges);
		}

	}
	

}

void UDeckSelector::RegenerateValidDeckData()
{	
	m_ValidDeckIndexSampleSourceSet.Reset();

	for(int32 ValidDeckIndex{ 0 }; ValidDeckIndex < m_aDecksValid.Num(); ++ValidDeckIndex)
	{
		for(int32 TimesAdded{ 0 }; TimesAdded < m_aDecksValid[ValidDeckIndex].RelativeProb; ++TimesAdded)
		{
			m_ValidDeckIndexSampleSourceSet.Add(ValidDeckIndex);

		}

	}


}

void UDeckSelector::SearchForNewValidDecks()
{
	std::list<int32> ValidatedDeckIndices{};
	for(int32 InvalidDeckIndex{ 0 }; InvalidDeckIndex < m_aDecksInvalid.Num(); ++InvalidDeckIndex)
	{
		if(m_aDecksInvalid[InvalidDeckIndex].ReqGeneration <= m_DeckGeneration)
		{
			m_aDecksValid.Add(std::move(m_aDecksInvalid[InvalidDeckIndex]));
			ValidatedDeckIndices.push_front(InvalidDeckIndex);
		}
		

	}

	//todo: maybe remove with predicate search for invalid path
	for(auto &&ValidatedIndex : ValidatedDeckIndices)
	{
		m_aDecksInvalid.RemoveAt(ValidatedIndex);

	}


}

void UDeckSelector::ApplyFillerMapConstraints(TMap<FString, int32> &ToFillerMap)
{
	std::list<FString> InvalidFillerCats{};

	for(auto &&Entry : ToFillerMap)
	{
		if(!m_IssuedChargesMap.Contains(Entry.Key))
		{
			InvalidFillerCats.emplace_back(Entry.Key);
		}

	}

	for(auto &&KeyToRemove : InvalidFillerCats)
	{
		ToFillerMap.Remove(KeyToRemove);

	}
	ToFillerMap.Compact();


}

FDeckData UDeckSelector::GetDeckDataFromValidDeckAt(const int32 Index) 
{
	auto *pDeckDataAsset{ Cast<UDeckDatabaseNative>(m_aDecksValid[Index].Path.TryLoad()) };

	//fixed buildings
	FDeckData DeckData{};
	DeckData.m_DeckWidgetClass = pDeckDataAsset->GetDeckWidget();
	DeckData.m_DeckAssetPath = std::move(m_aDecksValid[Index].Path);

	{
		const auto NumData{ pDeckDataAsset->GetNumData() };
		for(int32 DataIndex{0}; DataIndex < NumData; ++DataIndex)
		{
			auto Category{ pDeckDataAsset->GetDependencyCategoryAtIndex(DataIndex) };
			auto NumCharges{ pDeckDataAsset->GetMinAmountAtIndex(DataIndex) };

			//constraint charge amount per map
			auto MaxChargeTypeOnMap{ static_cast<int32>(m_pPlaceableMaxPerMap->GetValueForCategory(Category)) };
			if(MaxChargeTypeOnMap >= 1)
			{
				auto *pIssueMapItem{ m_IssuedChargesMap.Find(Category) };
				auto ChargesAlreadyIssued{ pIssueMapItem ? *pIssueMapItem : 0 };

				NumCharges = FMath::Clamp(NumCharges, 0, MaxChargeTypeOnMap - ChargesAlreadyIssued);
			}
			checkf(NumCharges >= 0, TEXT("Deck Selector charges smaller than zero"));

			//postpone this to endless decks only
			//AddToIssuedChargesForCategory(Category, NumCharges);

			DeckData.m_ChargeMapping.Add(Category, NumCharges);
		}
	}

	//filler handling
	auto FillerBuildingAmount{ FMath::RoundToInt(m_pFillerBuildingAmountSource->GetFloatValue(m_DeckGeneration)) };

	AddFillerChargesToDeckData(FillerBuildingAmount, DeckData, pDeckDataAsset);


	return DeckData;


}

void UDeckSelector::AddToIssuedChargesForCategory(const FString& Category, int32 ChargeCount)
{
	auto &Value{ m_IssuedChargesMap.FindOrAdd(Category) };
	Value += ChargeCount;


}

TArray<TTuple<FSoftObjectPath, class UDeckDatabaseNative *>> UDeckSelector::FetchAllDecks() const
{
	auto &Registry{ FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")) };

	//filter
	FARFilter Filter{};
	Filter.ClassNames.Add(*UDeckDatabaseNative::StaticClass()->GetName());
	Filter.bRecursiveClasses = true;
	   	
	Filter.PackagePaths.Add(TEXT("/Game/Placeables/Decks"));
	Filter.bRecursivePaths = true;

	//fetch
	TArray<FAssetData> aFoundAssets;
	Registry.Get().GetAssets(Filter, aFoundAssets);

	if(aFoundAssets.Num() <= 0)
	{
		return {};


	}

	//type conversion
	TArray<TTuple<FSoftObjectPath, UDeckDatabaseNative *>> aOutDecks{};
	for(auto &&AssetData : aFoundAssets)
	{		
		aOutDecks.Emplace(AssetData.ToSoftObjectPath(), Cast<UDeckDatabaseNative>(AssetData.FastGetAsset(true)) );		

	}

	return aOutDecks;


}
