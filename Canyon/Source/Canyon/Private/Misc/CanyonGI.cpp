// Fill out your copyright notice in the Description page of Project Settings.

#include "CanyonGI.h"
#include "Engine/World.h"
#include "CanyonHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Misc/CanyonLogs.h"
#include "CanyonGM.h"
#include "Player/RTSPlayerEye.h"
#include "ModuleManager.h"
#include "AssetRegistryModule.h"


UCanyonGI::UCanyonGI() :
	m_bLoadAllPlaceablesOnStartup{ true },
	m_CarryOverCharges{},
	m_CarryOverScore{ 0 },
	m_Seed{ 0 }
{
}

void UCanyonGI::Init()
{
	Super::Init();
	
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UCanyonGI::ReceiveOnPreMapLoaded);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UCanyonGI::ReceiveOnPostMapLoaded);

	//dummy so we dont play in editor without seed
	m_Seed = FMath::Rand();
	FMath::SRandInit(m_Seed);

	UE_LOG(LogCanyonCommon, Warning, TEXT("----INIT WITH SEED: %i ----"), m_Seed);

	if(!m_bLoadAllPlaceablesOnStartup)
	{
		return;


	}
	UE_LOG(LogCanyonCommon, Warning, TEXT("Begin pre loading placeable assets"));

	auto &Registry{ FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")) };

	FARFilter Filter{};
	
	Filter.PackagePaths.Add(TEXT("/Game/Placeables"));
	Filter.bRecursivePaths = true;


	//ensure registry population
	Registry.Get().ScanFilesSynchronous({ TEXT("/Game/Placeables") });

	//fetch
	TArray<FAssetData> aFoundAssets;
	Registry.Get().GetAssets(Filter, aFoundAssets);

	for(auto &&AssetData : aFoundAssets)
	{		
		if(AssetData.ObjectPath.ToString().Contains("Decks"))
		{
			continue;
		}

		auto *pLoaded{ LoadClass<UObject>(nullptr, *(AssetData.ObjectPath.ToString() + "_C")) };
		if(pLoaded)
		{
			m_apPreLoadedPlaceables.Add(pLoaded);			
		}
		
	}
		UE_LOG(LogCanyonCommon, Warning, TEXT("End pre loading placeable assets"));

}

void UCanyonGI::BeginLevelSwitch(const TSoftObjectPtr<UWorld>& NewLevel)
{
	auto *pTargetWorld{ SafeLoadObjectPtr(NewLevel) };

	UGameplayStatics::OpenLevel(GetWorld(), *pTargetWorld->GetMapName());


}

void UCanyonGI::StartupGame(bool bContinueGame)
{
	if(m_aFirstLevelsPool.Num() <= 0)
	{
		return;


	}

	m_Seed = FMath::Rand();
	FMath::SRandInit(m_Seed);

	auto &LevelPath{ m_aFirstLevelsPool[GetRandomIndexSeeded(m_aFirstLevelsPool.Num())] };
	auto *pTargetWorld{ SafeLoadObjectPtr(LevelPath) };
	
	UE_LOG(LogCanyonCommon, Warning, TEXT("----STARTING UP GAME WITH SEED: %i ----"), m_Seed);
	
	UGameplayStatics::OpenLevel(GetWorld(), *pTargetWorld->GetMapName());
	
	   
}

void UCanyonGI::AddCarryOverCharge(const FString& CategoryName)
{
	auto &Entry{ m_CarryOverCharges.m_ChargeMapping.FindOrAdd(CategoryName) };

	++Entry;


}

void UCanyonGI::RemoveCarryOverCharge(const FString& CategoryName)
{
	auto &Entry{ m_CarryOverCharges.m_ChargeMapping.FindOrAdd(CategoryName) };

	if(Entry >= 0)
	{
		--Entry;
	}


}


//Private---------------------

void UCanyonGI::FetchCarryOverDataFromOldLevel(const UWorld* pWorld)
{
	check(pWorld);

	auto *pGM{ Cast<ACanyonGM>(pWorld->GetAuthGameMode()) };
	if(!pGM)
	{
		return;

		
	}

	 m_CarryOverScore = pGM->GetPointsCurrent();


}

void UCanyonGI::SetupCarryOverDataInNewLevel(UWorld* pWorld) const
{
	check(pWorld);

	auto *pGM{ Cast<ACanyonGM>(pWorld->GetAuthGameMode()) };
	if(!pGM)
	{
		return;


	}

	pGM->InitPointState(m_CarryOverScore);
	pGM->AddDeckDataToIssuedCharges(m_CarryOverCharges);


}

void UCanyonGI::ReceiveOnPreMapLoaded(const FString& MapName)
{
	OnBeginLoadingScreen(MapName);
	FetchCarryOverDataFromOldLevel(GetWorld());


}

void UCanyonGI::ReceiveOnPostMapLoaded(UWorld* pLoadedWorld)
{
	SetupCarryOverDataInNewLevel(pLoadedWorld);

	auto *pPlayer{ Cast<ARTSPlayerEye>(pLoadedWorld->GetFirstPlayerController()->GetPawn()) };
	auto *pGM{ Cast<ACanyonGM>(pLoadedWorld->GetAuthGameMode()) };
	if(pGM && pPlayer)
	{
#if !UE_EDITOR
		pPlayer->BeginGame();
		pGM->BeginGame();
#endif
	}

	OnEndLoadingScreen(pLoadedWorld);


}
