// Fill out your copyright notice in the Description page of Project Settings.

#include "CanyonGM.h"
#include "ModuleManager.h"
#include "IAssetRegistry.h"
#include "InfluenceDataObject.h"
#include "Placeables/DeckDatabaseNative.h"
#include "WidgetBase/DeckWidgetBase.h"
#include "WidgetBase/PlaceableIconWidgetBase.h"
#include "WidgetBase/PointIndicatorWidgetBase.h"
#include "WidgetBase/PrettyWidget.h"
#include "Player/RTSPlayerEye.h"
#include "CanyonHelpers.h"
#include "CanyonLogs.h"
#include "Placeables/DeckSelector.h"
#include "Misc/CategoryStringMappingDAL.h"
#include "CanyonGI.h"
#include "WidgetBlueprintLibrary.h"
#include "ActorDeferredPlay.h"
#include "Placeables/PlaceablePreview.h"
#include "MeshInstancer.h"
#include "Savegame/SavegameHandler.h"


//Public-------------

ACanyonGM::ACanyonGM() :
	m_CarryOverBuildingCountMax{ 4 },
	m_PointsOnLevelOpen{ 0 },
	m_bIsNextLevelAccessible{ false },
	m_apSelectedCarryPlaceables{},
	m_PointsCurrent{ 0 },
	m_PointsRequired{ 0 },
	m_bIsInPlaceableSelectionMode{ false }
{
}

void ACanyonGM::BeginGame()
{	
	FInputModeGameAndUI InputMode{};
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
	GetWorld()->GetFirstPlayerController()->SetInputMode(InputMode);

	UWidgetBlueprintLibrary::SetFocusToGameViewport();

#if UE_EDITOR
	//has to be init now if not done by GI
	auto *pPlayer{GetFirstPlayerPawn<ARTSPlayerEye>(GetWorld()) };
	pPlayer->BeginGame();
#endif

	auto RequiredPointsDelta{ FMath::RoundToInt(m_pRequiredPointsSource->GetFloatValue(m_pDeckSelector->GetDeckGeneration())) };
	SetPointsRequired(m_PointsCurrent + RequiredPointsDelta);

	AActorDeferredPlay::BroadcastBeginGame();
	
	   	 
}

void ACanyonGM::InitPointState(int32 CarryOverPoints)
{
	m_PointsRequired = CarryOverPoints;
	m_PointsCurrent = CarryOverPoints;
	m_PointsOnLevelOpen = CarryOverPoints;

	auto *pPlayer{GetFirstPlayerPawn<ARTSPlayerEye>(GetWorld()) };


	auto RequiredPointsDelta{ FMath::RoundToInt(m_pRequiredPointsSource->GetFloatValue(m_pDeckSelector->GetDeckGeneration())) };

	pPlayer->OnPointsRequiredChanged(m_PointsRequired + RequiredPointsDelta);
	pPlayer->OnPointsCurrentChanged(m_PointsCurrent);

	//don't use the regular OnPointsChanged here
	//bc this update is just po
	
}

void ACanyonGM::AddCarryOverChargesToIssued(const FCarryOverCharges &DeckData)
{
	m_pDeckSelector->AddCarryOverChargesToIssued(DeckData);


}

int32 ACanyonGM::GetInfluenceForPlaceable
(
	const FString &FirstInfluenceQualifier,
	const FString &SecondInfluenceQualifier
)	const
{
	return (*m_pInfluenceData)[FirstInfluenceQualifier][SecondInfluenceQualifier];


}

int32 ACanyonGM::GetInfluenceBasisForCategory(const FString& CategoryName) const
{
	return (*m_pInfluenceData)[CategoryName].m_BasePointAmount;


}

const TMap<FString, int32>& ACanyonGM::GetTempInfluenceMappingForCategory(const FString& CategoryName) const
{
	return (*m_pInfluenceData)[CategoryName].m_InfluenceMapping;


}

void ACanyonGM::AddPointsCurrent(const int32 Points)
{	
	m_PointsCurrent += Points;

	ReceiveOnPointsChanged();


}

void ACanyonGM::IncreaseDeckGeneration()
{
	m_pDeckSelector->IncreaseDeckGeneration();

	

	/*
	void ACanyonGM::AddPointsRequiredFor(const FString &Category, const uint32 Amount)
	{
		AddPointsRequired(Amount * (*m_pInfluenceData)[Category].m_BasePointRequirement);


	}
	*/


}

TArray<FDeckData> ACanyonGM::GetDeckData(int32 Amount)
{
	return m_pDeckSelector->GetDeckData(Amount);


}

FDeckData ACanyonGM::GetEndlessDeckData()
{
	return m_pDeckSelector->GetEndlessDeckData();


}

void ACanyonGM::FillUpDeckDataNonEndless(FDeckData& DeckData)
{
	m_pDeckSelector->FillUpDeckDataNonEndless(DeckData);


}

void ACanyonGM::NotifyPlaceableActionSelect(FHitResult &Hit)
{
	auto *pPlaceable{ Cast<APlaceableBase>(Hit.Actor.Get()) };


	if(!pPlaceable)
	{
		return;


	}

	if(pPlaceable->IsA<APlaceablePreview>())
	{
		UE_DEBUG_BREAK();
		UE_LOG(LogCanyonCommon, Warning, TEXT("Placeable preview was clicked on"));
		return;
	}

	//todo: use data asset
	if(pPlaceable->GetPlaceableCategory() == "Tree" || pPlaceable->GetPlaceableCategory() == "StartBuilding")
	{
		return;


	}

	if(m_bIsInPlaceableSelectionMode)
	{
		if(!pPlaceable->IsHightlighted() && m_apSelectedCarryPlaceables.Num() < m_CarryOverBuildingCountMax)
		{
			pPlaceable->ToggleSelectionHighlight();
			m_apSelectedCarryPlaceables.Add(pPlaceable);
		}
		else if(pPlaceable->IsHightlighted())
		{
			pPlaceable->ToggleSelectionHighlight();
			m_apSelectedCarryPlaceables.RemoveSingle(pPlaceable);			
		}

		auto *pPlayer{ GetFirstPlayerPawn<ARTSPlayerEye>(GetWorld()) };
		pPlayer->NotifyBuildingSelectionChanged(m_apSelectedCarryPlaceables.Num(), m_CarryOverBuildingCountMax);
	}


}

void ACanyonGM::DebugAddChargesForCategory(const FString& Category, int32 Num) const
{
	auto *pPlayer{ GetFirstPlayerPawn<ARTSPlayerEye>(GetWorld()) };
	if(pPlayer)
	{
		pPlayer->DebugAddChargesForCategory(Category, Num);
	}


}

bool ACanyonGM::IsInEndlessMode() const
{
	return m_pDeckSelector->UsesEndlessFillers();


}

FString ACanyonGM::GetPrettyNameForCategory(const FString& CategoryName)
{
	auto MappedString{ m_pPrettyCategoryNameSource->GetStringForCategory(CategoryName) };
	return MappedString.IsEmpty() ? CategoryName : MappedString;


}

void ACanyonGM::SetPointsRequired(const int32 Points)
{
	m_PointsRequired = Points;
	auto *pPlayer{GetFirstPlayerPawn<ARTSPlayerEye>(GetWorld()) };
	pPlayer->OnPointsRequiredChanged(m_PointsRequired);

	ReceiveOnPointsChanged();


}

void ACanyonGM::EnterNextLevel()
{
	auto *pGI{ Cast<UCanyonGI>(GetGameInstance()) };
	
	pGI->BeginLevelSwitch(m_aNextLevelsPool[GetRandomIndexSeeded(m_aNextLevelsPool.Num())]);


}

void ACanyonGM::EnterPlaceableSelectionMode()
{
	auto *pPlayer{ GetFirstPlayerPawn<ARTSPlayerEye>(GetWorld()) };

	pPlayer->NotifyBuildingSelectionChanged(0, m_CarryOverBuildingCountMax);
	pPlayer->SwitchToPlaceableSelectionMode();

	m_bIsInPlaceableSelectionMode = true;


}

void ACanyonGM::AbortPlaceableSelectionMode()
{
	m_bIsInPlaceableSelectionMode = false;
	
	auto *pPlayer{ GetFirstPlayerPawn<ARTSPlayerEye>(GetWorld()) };

	pPlayer->SwitchToPlaceablePlacementMode();


}

bool ACanyonGM::TryCommitPlaceableSelection()
{
	//if selected amount == needed
	if(m_apSelectedCarryPlaceables.Num() == m_CarryOverBuildingCountMax)
	{		
		//commit selection and return true
		auto *pGI{ Cast<UCanyonGI>(GetGameInstance()) };
		pGI->BuildCarryOverChargesFormSelection(m_apSelectedCarryPlaceables);

		return true;


	}
	//else less -> return false
	return false;


}

void ACanyonGM::ForceCommitPlaceableSelection()
{
	//commit selection
	auto *pGI{ Cast<UCanyonGI>(GetGameInstance()) };
	pGI->BuildCarryOverChargesFormSelection(m_apSelectedCarryPlaceables);

	
}

float ACanyonGM::GetPlaceableDependencyRadius(const FString& CategoryName) const
{
	return (*m_pInfluenceData)[CategoryName].m_InfluenceRadius;


}

TSubclassOf<UPlaceableIconWidgetBase> ACanyonGM::GetPlaceableWidget(const FString& CategoryName) const
{
	return SafeLoadClassPtr((*m_pInfluenceData)[CategoryName].m_UiWidgetClass);
	

}


//Protected-------------------

void ACanyonGM::BeginPlay()
{
	Super::BeginPlay();

	m_pInfluenceData = UInfluenceDataObject::CreateFromFile();
	m_pDeckSelector = UDeckSelector::Construct(m_DeckSelectorClass.Get());
	m_pMeshInstancer = GetWorld()->SpawnActor<AMeshInstancer>();

#if UE_EDITOR
	BeginGame();
#endif
	auto *pHandler{ NewObject<USavegameHandler>() };
	pHandler->SaveGame();
	
}

//Private----------------------

void ACanyonGM::ReceiveOnPointsChanged()
{
	auto *pPlayer{ GetFirstPlayerPawn<ARTSPlayerEye>(GetWorld()) };

	pPlayer->OnPointsCurrentChanged(m_PointsCurrent);
	if(m_PointsCurrent >= m_PointsRequired)
	{
		for(int32 GenerationOffset{ 1 }; m_PointsCurrent >= m_PointsRequired; ++GenerationOffset)
		{
			auto RequiredPointsDelta{ FMath::RoundToInt(m_pRequiredPointsSource->GetFloatValue(m_pDeckSelector->GetDeckGeneration() + GenerationOffset)) };
			m_PointsRequired += RequiredPointsDelta;
			pPlayer->NotifyOnNewDeckAvailable();	
		}

		//cant call this bc it would recurse SetPointsRequired(m_PointsCurrent + RequiredPointsDelta);	
		
		pPlayer->OnPointsRequiredChanged(m_PointsRequired);		
		NotifyOnNewDeckAvailable();
		
	}
	
	//loose condition
	if
	(
		m_PointsCurrent < m_PointsRequired
		&& pPlayer->GetCurrentChargesForPlaceables() <= 0 
		&& !pPlayer->GetAreDecksSelectable()
		&& !m_bIsNextLevelAccessible
	)
	{
		NotifyOnLoose();
	}

	//next level accessible
	if(	(m_PointsCurrent - m_PointsOnLevelOpen) >= m_NextLevelRequiredPointsDelta && !m_bIsNextLevelAccessible)
	{
		m_bIsNextLevelAccessible = true;		
		pPlayer->OnNextLevelAccessible();
		NotifyNextLevelAvailable();
	}


}
