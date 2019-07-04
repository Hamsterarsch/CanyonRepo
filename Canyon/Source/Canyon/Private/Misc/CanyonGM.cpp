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


//Public-------------

ACanyonGM::ACanyonGM() :
	m_PointsOnLevelOpen{ 0 },
	m_bIsNextLevelAccessible{ false },
	m_PointsCurrent{ 0 },
	m_PointsRequired{ 0 }
{
}

void ACanyonGM::BeginGame()
{	
	FInputModeGameAndUI InputMode{};
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
	GetWorld()->GetFirstPlayerController()->SetInputMode(InputMode);

	UWidgetBlueprintLibrary::SetFocusToGameViewport();


}

void ACanyonGM::InitPointState(int32 CarryOverPoints)
{
	m_PointsRequired = CarryOverPoints;
	m_PointsCurrent = CarryOverPoints;
	m_PointsOnLevelOpen = CarryOverPoints;

	auto *pPlayer{GetFirstPlayerPawn<ARTSPlayerEye>(GetWorld()) };
	
	pPlayer->OnPointsRequiredChanged(m_PointsRequired);
	pPlayer->OnPointsCurrentChanged(m_PointsCurrent);

	//don't use the regular OnPointsChanged here
	//bc this update is just po
	
}

void ACanyonGM::AddDeckDataToIssuedCharges(const FDeckData& DeckData)
{
	m_pDeckSelector->AddDeckDataToIssued(DeckData);


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

	auto RequiredPointsDelta{ FMath::RoundToInt(m_pRequiredPointsSource->GetFloatValue(m_pDeckSelector->GetDeckGeneration())) };

	SetPointsRequired(m_PointsCurrent + RequiredPointsDelta);

	

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

void ACanyonGM::OnLoose()
{
	auto *pPlayer{GetFirstPlayerPawn<ARTSPlayerEye>(GetWorld()) };
	pPlayer->OnLoose();


}


void ACanyonGM::EnterNextLevel()
{
	auto *pGI{ Cast<UCanyonGI>(GetGameInstance()) };
	
	pGI->BeginLevelSwitch(m_aNextLevelsPool[GetRandomIndexSeeded(m_aNextLevelsPool.Num())]);


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


#if UE_EDITOR
	BeginGame();
#endif
	

}

//Private----------------------

void ACanyonGM::ReceiveOnPointsChanged()
{
	auto *pPlayer{ GetFirstPlayerPawn<ARTSPlayerEye>(GetWorld()) };

	pPlayer->OnPointsCurrentChanged(m_PointsCurrent);
	if(m_PointsCurrent >= m_PointsRequired)
	{
		pPlayer->NotifyOnDisplayNewDecks();		
	}
	
	//loose condition
	if
	(
		m_PointsCurrent < m_PointsRequired
		&& pPlayer->GetCurrentChargesForPlaceables() <= 0 
		&& !pPlayer->GetAreDecksSelectable() 
	)
	{
		OnLoose();
	}

	//next level accessible
	if(	(m_PointsCurrent - m_PointsOnLevelOpen) >= m_NextLevelRequiredPointsDelta && !m_bIsNextLevelAccessible)
	{
		m_bIsNextLevelAccessible = true;		
		pPlayer->OnNextLevelAccessible();
	}


}
