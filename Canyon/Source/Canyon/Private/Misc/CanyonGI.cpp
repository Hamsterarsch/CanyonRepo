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
#include "Engine/GameViewportClient.h"
#include "AssetRegistryModule.h"
#include "UMG/Public/Slate/SObjectWidget.h"
#include "UMG/Public/Blueprint/UserWidget.h"
#include "TimerManager.h"


UCanyonGI::UCanyonGI() :
	m_bLoadAllPlaceablesOnStartup{ true },
	m_LoadingScreenTransitionTime{ 0.25f },
	m_CarryOverCharges{},
	m_CarryOverScore{ 0 },
	m_Seed{ 0 },
	m_bHasLoadingScreenBegun{ false }
{
}

void UCanyonGI::Init()
{
	Super::Init();
	
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UCanyonGI::ReceiveOnPreMapLoaded);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UCanyonGI::ReceiveOnPostMapLoaded);
	FWorldDelegates::OnWorldCleanup.AddUObject(this, &UCanyonGI::CleanupWorld);

	//dummy so we dont play in editor without seed
	m_Seed = FMath::Rand();
	FMath::SRandInit(m_Seed);

	UE_LOG(LogCanyonCommon, Warning, TEXT("----INIT WITH SEED: %i ----"), m_Seed);




}

void UCanyonGI::Shutdown()
{
	Super::Shutdown();

	m_pLoadingScreenSlate.Reset();
	m_pLoadingScreenGC.Reset();


}

void UCanyonGI::BeginSwitchToNextLevel(const TSoftObjectPtr<UWorld>& NewLevel)
{
	m_TargetWorld = NewLevel;
	auto *pTargetWorld = SafeLoadObjectPtr(NewLevel);


	FTimerHandle Handle{};
	FTimerDelegate TimerDelegate{};
	TimerDelegate.BindUObject(this, &UCanyonGI::OnLoadingScreenTransitionTimeExpired);

	m_bHasLoadingScreenBegun = true;	
	auto *pLoadingScreen{ OnBeginLoadingScreen(*pTargetWorld->GetMapName()) };
	if(!m_pLoadingScreenGC.IsValid() || !m_pLoadingScreenSlate.IsValid())
	{
		SetupLoadingScreenReferences(pLoadingScreen);
		GetGameViewportClient()->AddViewportWidgetContent(m_pLoadingScreenSlate.ToSharedRef(), -1);
	}

	TimerManager->SetTimer(Handle, TimerDelegate, m_LoadingScreenTransitionTime, false);


}

void UCanyonGI::ExitGameloop()
{
	NotifyExitGameloop();
	BeginSwitchToNextLevel(m_MainMenuLevel);


}

void UCanyonGI::StartupGame(bool bContinueGame)
{
	if(m_aFirstLevelsPool.Num() <= 0)
	{
		return;


	}

	m_Seed = FMath::Rand();
	FMath::SRandInit(m_Seed);
	UE_LOG(LogCanyonCommon, Warning, TEXT("----STARTING UP GAME WITH SEED: %i ----"), m_Seed);

	auto &LevelPath{ m_aFirstLevelsPool[GetRandomIndexSeeded(m_aFirstLevelsPool.Num())] };
	auto *pTargetWorld = SafeLoadObjectPtr(LevelPath);
	m_TargetWorld = LevelPath;	

	m_bSkipCarryData = true;

	auto *pLoadingScreenWidget{ OnBeginLoadingScreen(*pTargetWorld->GetMapName()) };
	m_bHasLoadingScreenBegun = true;
	m_bWasGameStarted = true;

	SetupLoadingScreenReferences(pLoadingScreenWidget);
	GetGameViewportClient()->AddViewportWidgetContent(m_pLoadingScreenSlate.ToSharedRef(), -1);

	NotifyExitGameloop();

	FTimerHandle Handle{};
	FTimerDelegate TimerDelegate{};
	TimerDelegate.BindUObject(this, &UCanyonGI::OnLoadingScreenTransitionTimeExpired);

	TimerManager->SetTimer(Handle, TimerDelegate, m_LoadingScreenTransitionTime, false);
		
	   
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

void UCanyonGI::LoadPlaceables()
{
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

void UCanyonGI::SetupLoadingScreenReferences(UUserWidget *pLoadingScreen)
{	
	m_pLoadingScreenGC =
		pLoadingScreen->TakeDerivedWidget<SObjectWidget>([]( UUserWidget* Widget, TSharedRef<SWidget> Content ) -> TSharedPtr<SObjectWidget> {
		       return SNew( SObjectWidget, Widget )[ Content ];
		});
	
	
	auto LoadingScreenSlateRef{ pLoadingScreen->TakeWidget() };
	m_pLoadingScreenSlate = LoadingScreenSlateRef;


}

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

void UCanyonGI::OnLoadingScreenTransitionTimeExpired()
{
	auto *pTarget{ SafeLoadObjectPtr(m_TargetWorld) };
	UGameplayStatics::OpenLevel(GetWorld(), *pTarget->GetMapName());


}

void UCanyonGI::ReceiveOnPreMapLoaded(const FString& MapName)
{
	if(!m_bHasLoadingScreenBegun)
	{
		OnBeginLoadingScreen(MapName);		
	}

	if(!m_bSkipCarryData)
	{
		FetchCarryOverDataFromOldLevel(GetWorld());		
	}


}

void UCanyonGI::ReceiveOnPostMapLoaded(UWorld* pLoadedWorld)
{
	auto *pPlayer{ Cast<ARTSPlayerEye>(pLoadedWorld->GetFirstPlayerController()->GetPawn()) };
	auto *pGM{ Cast<ACanyonGM>(pLoadedWorld->GetAuthGameMode()) };
	if(pGM && pPlayer)
	{
#if UE_EDITOR < 1
		pPlayer->BeginGame();
		pGM->BeginGame();
		UE_LOG(LogCanyonCommon, Warning, TEXT("Entering conditional section after post map load"));
#endif
	}

	//game has to have started
	if(!m_bSkipCarryData)
	{
		SetupCarryOverDataInNewLevel(pLoadedWorld);		
	}
	m_bSkipCarryData = false;

	//reset carry over data
	m_CarryOverScore = 0;
	m_CarryOverCharges.m_Charges.Empty();

	if(m_bHasLoadingScreenBegun)
	{
		OnEndLoadingScreen(pLoadedWorld);		
	}

	m_TargetWorld.Reset();


}
