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

	FWorldDelegates::OnWorldCleanup.AddUObject(this, &UCanyonGI::CleanupWorld);


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

void UCanyonGI::BuildCarryOverChargesFormSelection(const TArray<APlaceableBase*> &apSelected)
{
	for(const auto *pPlaceable : apSelected)
	{
		auto &InstanceClassData{ m_CarryOverCharges.m_Charges.FindOrAdd(pPlaceable->GetPlaceableCategory()) };
		InstanceClassData.Data.Add(pPlaceable->GetClass());

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

	auto *pPlayer{ GetFirstPlayerPawn<ARTSPlayerEye>(pWorld) };
	checkf(pPlayer, TEXT("Could not setup carry data in new level bc player was invalid"));
	
	pGM->InitPointState(m_CarryOverScore);
	pGM->AddCarryOverChargesToIssued(m_CarryOverCharges);
	pPlayer->AddCarryOverChargesToDeck(m_CarryOverCharges);


}

void UCanyonGI::CleanupWorld(UWorld* pWorld, bool bSessionEnded, bool bCleanupResources)
{
	if(bCleanupResources)
	{
		AActorDeferredPlay::BroadcastEndGame();
	}

}

void UCanyonGI::ReceiveOnPreMapLoaded(const FString& MapName)
{
	OnBeginLoadingScreen(MapName);
	FetchCarryOverDataFromOldLevel(GetWorld());


}

void UCanyonGI::ReceiveOnPostMapLoaded(UWorld* pLoadedWorld)
{
	auto *pPlayer{ Cast<ARTSPlayerEye>(pLoadedWorld->GetFirstPlayerController()->GetPawn()) };
	auto *pGM{ Cast<ACanyonGM>(pLoadedWorld->GetAuthGameMode()) };
	if(pGM && pPlayer)
	{
#if !UE_EDITOR
		pPlayer->BeginGame();
		pGM->BeginGame();
#endif
	}

	//game has to have started
	SetupCarryOverDataInNewLevel(pLoadedWorld);

	//reset carry over data
	m_CarryOverCharges.m_Charges.Empty();

	OnEndLoadingScreen(pLoadedWorld);
	
	

}
