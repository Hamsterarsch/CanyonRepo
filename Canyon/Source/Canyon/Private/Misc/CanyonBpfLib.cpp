// Fill out your copyright notice in the Description page of Project Settings.


#include "CanyonBpfLib.h"
#include "CanyonHelpers.h"
#include "Placeables/PlaceableBase.h"
#include "AssetRegistryModule.h"
#include "Placeables/DeckDatabaseNative.h"
#include <set>
#include "Misc/CanyonLogs.h"
#include "Paths.h"
#include "SlateBrush.h"

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

	UE_LOG(LogCanyonPlacement, Log, TEXT("Searching for placeable bp instance in path %s"), *(Filter.PackagePaths[0].ToString()) );

	Registry.Get().ScanFilesSynchronous({ TEXT("/Game/Placeables") });
	TArray<FAssetData> aFoundAssets;
	Registry.Get().GetAssets(Filter, aFoundAssets);

	if(aFoundAssets.Num() == 0)
	{
		UE_LOG(LogCanyonPlacement, Error, TEXT("Could not find any placeable bp for the specified category."));
		return nullptr;
	}
	
	auto &AssetData{ aFoundAssets[GetRandomIndex(aFoundAssets.Num())] };

	FString GeneratedClassPath{ AssetData.ObjectPath.ToString() };
	GeneratedClassPath += TEXT("_C");

	auto ClassObjectPath{FPackageName::ExportTextPathToObjectPath(GeneratedClassPath) };
	auto *pClass{ LoadClass<APlaceableBase>(nullptr, *GeneratedClassPath) };

	if(!pClass)
	{
		UE_LOG(LogCanyonPlacement, Error, TEXT("Could not load generated class for placeable bp instance"));
	}

	return pClass;
	/*
	//todo: clean this up, it should suffice to add _C to the found assets path and loading that (this equals the generated class)
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
	*/


}

TArray<UDeckDatabaseNative *> UCanyonBpfLib::GetRandomDecks(const int32 NumDecks, FString SubCategory)
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

uint8 UCanyonBpfLib::EnumStringToEnumByte(const UUserDefinedEnum* pEnum, const FString& EnumIdentifier)
{
	if(!pEnum)
	{
		return 255;

		
	}

	return pEnum->GetIndexByNameString(EnumIdentifier);


}

void UCanyonBpfLib::SetBrushImage(FSlateBrush &Target, UObject* pObject)
{
	Target.SetResourceObject(pObject);


}
