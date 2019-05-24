// Fill out your copyright notice in the Description page of Project Settings.


#include "CanyonBpfLib.h"
#include "Misc/CanyonHelpers.h"
#include "Placeables/PlaceableBase.h"
#include "AssetRegistryModule.h"

TSubclassOf<APlaceableBase> UCanyonBpfLib::GetCategoryPlaceableClass(FString Category)
{
		//get data assets
	auto &Registry{ FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")) };

	FARFilter Filter{};
	Filter.ClassNames.Add(*APlaceableBase::StaticClass()->GetName());
	Filter.bRecursiveClasses = true;

	if(!Category.IsEmpty())
	{
		Category.InsertAt(0, '/');
	}

	Filter.PackagePaths.Add(*(TEXT("/Game/Placeables/") + Category));
	Filter.bRecursivePaths = true;
	
	TArray<FAssetData> aFoundAssets;
	Registry.Get().GetAssets(Filter, aFoundAssets);

	if(aFoundAssets.Num() == 0)
	{
		return nullptr;
	}

	return aFoundAssets[GetRandomIndex(aFoundAssets.Num())].GetAsset()->GetClass();


}
