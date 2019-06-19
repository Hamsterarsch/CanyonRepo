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
#include "Placeables/DeckSelector.h"


//Public-------------

ACanyonGM::ACanyonGM() :
	m_PointsCurrent{ 0 },
	m_PointsRequired{ 0 },
	m_SessionSeed{ 0 }	
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

int32 ACanyonGM::GetInfluenceBasisForCategory(const FString& CategoryName) const
{
	return (*m_pInfluenceData)[CategoryName].m_BasePointAmount;


}

void ACanyonGM::AddPointsCurrent(const int32 Points)
{	
	m_PointsCurrent += Points;
	m_pPointWidget->OnPointsCurrentChanged(m_PointsCurrent);

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

void ACanyonGM::SetPointsRequired(const int32 Points)
{
	m_PointsRequired = Points;
	m_pPointWidget->OnPointsRequiredChanged(m_PointsRequired);

	ReceiveOnPointsChanged();


}

void ACanyonGM::OnLoose()
{
	m_pLooseWidget->ShowWidget();


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

	//non savegame init
	m_SessionSeed = FMath::Rand();
	FMath::SRandInit(m_SessionSeed);

	m_pInfluenceData = UInfluenceDataObject::CreateFromFile();
		
	m_pPointWidget = CreateWidget<UPointIndicatorWidgetBase>(GetWorld(), m_PointIndicatorWidgetClass.Get());
	m_pPointWidget->AddToViewport();
	
	m_pLooseWidget = CreateWidget<UPrettyWidget>(GetWorld(), m_LooseWidgetClass.Get());
	m_pLooseWidget->HideWidget();
	m_pLooseWidget->AddToViewport();

	m_pDeckSelector = UDeckSelector::Construct(m_DeckSelectorClass.Get());


}

//Private----------------------

void ACanyonGM::ReceiveOnPointsChanged()
{
	auto *pPlayer{ Cast<ARTSPlayerEye>(GetWorld()->GetFirstPlayerController()->GetPawn()) };
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


}
