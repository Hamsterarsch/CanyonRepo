// Fill out your copyright notice in the Description page of Project Settings.


#include "CanyonBpfLib.h"
#include "Misc/CanyonHelpers.h"
#include "Placeables/PlaceableBase.h"
#include "AssetRegistryModule.h"
#include "Paths.h"

TSubclassOf<APlaceableBase> UCanyonBpfLib::GetCategoryPlaceableClass(FString Category)
{
		//get data assets
	auto &Registry{ FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")) };

	FARFilter Filter{};
	//todo: doesnt work for assets based on bps deriving from this base
	//Filter.ClassNames.Add(*APlaceableBase::StaticClass()->GetName());
	//Filter.bRecursiveClasses = true;

	if(!Category.IsEmpty())
	{
		Category.InsertAt(0, '/');
	}

	Filter.PackagePaths.Add(*(TEXT("/Game/Placeables") + Category));
	Filter.bRecursivePaths = true;

	Registry.Get().ScanFilesSynchronous({ TEXT("/Game/Placeables") });
	TArray<FAssetData> aFoundAssets;
	Registry.Get().GetAssets(Filter, aFoundAssets);

	if(aFoundAssets.Num() == 0)
	{
		return nullptr;
	}
	
	auto &AssetData{ aFoundAssets[GetRandomIndex(aFoundAssets.Num())] };


	//todo: clean this up, it should suffic to add _C to the found assets path and loading that (this equals the generated class)
#if WITH_EDITOR
	auto *pAsset{ AssetData.GetAsset() };
	auto *pAsBp{ Cast<UBlueprint>(pAsset) };
	   	 	
	return pAsBp->GeneratedClass->IsChildOf<APlaceableBase>() ? pAsBp->GeneratedClass.Get() : nullptr;
#else
	auto PathToGenerated{ AssetData.TagsAndValues.FindTag(TEXT("GeneratedClass")) };
	auto ClassObjectPath{ FPackageName::ExportTextPathToObjectPath(PathToGenerated.GetValue()) };

	auto GeneratedClass{ LoadObject<UBlueprintGeneratedClass>(nullptr, *ClassObjectPath) };
	UE_LOG(LogTemp, Warning, TEXT("Found generated class %s at %s"), *GeneratedClass->GetName(), *ClassObjectPath);

	auto b = GeneratedClass->IsChildOf<APlaceableBase>();
	if(!b)
	{
		UE_LOG(LogTemp, Warning, TEXT("not a subclass of placeable"));
	}

	return b ? GeneratedClass : nullptr;
#endif


}
