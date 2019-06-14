// Fill out your copyright notice in the Description page of Project Settings.

#include "CanyonGM.h"
#include "ModuleManager.h"
#include "IAssetRegistry.h"
#include "InfluenceDataObject.h"
#include "AssetRegistryModule.h"
#include "Placeables/DeckDatabaseNative.h"
#include "Misc/CanyonHelpers.h"
#include "WidgetBase/DeckWidgetBase.h"
#include "WidgetBase/PlaceableIconWidgetBase.h"
#include "WidgetBase/PointIndicatorWidgetBase.h"
#include "WidgetBase/PrettyWidget.h"
#include <set>
#include "UI/DeckStateRenderer.h"
#include "UI/DeckState.h"
#include "WidgetBase/PlaceableWidgetBase.h"
#include "CanyonBpfLib.h"
#include "Player/RTSPlayerEye.h"


//Public-------------

ACanyonGM::ACanyonGM() :
	m_PointsCurrent{ 0 },
	m_PointsRequired{ 0 }
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

int32 ACanyonGM::GetInfluenceForBaseCategory(const FString& CategoryName) const
{
	return (*m_pInfluenceData)[CategoryName].m_BasePointAmount;


}

void ACanyonGM::AddPointsCurrent(const int32 Points)
{	
	m_PointsCurrent += Points;
	m_pPointWidget->OnPointsCurrentChanged(m_PointsCurrent);

	ReceiveOnPointsChanged();


}

void ACanyonGM::AddPointsRequiredFor(const FString &Category, const uint32 Amount)
{
	AddPointsRequired(Amount * (*m_pInfluenceData)[Category].m_BasePointRequirement);


}

void ACanyonGM::AddPointsRequired(const int32 Points)
{
	m_PointsRequired += Points;
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

	m_SessionSeed = FMath::Rand();
	FMath::SRandInit(m_SessionSeed);

	m_pInfluenceData = UInfluenceDataObject::CreateFromFile();
		
	m_pPointWidget = CreateWidget<UPointIndicatorWidgetBase>(GetWorld(), m_PointIndicatorWidgetClass.Get());
	m_pPointWidget->AddToViewport();


	m_pLooseWidget = CreateWidget<UPrettyWidget>(GetWorld(), m_LooseWidgetClass.Get());
	m_pLooseWidget->HideWidget();
	m_pLooseWidget->AddToViewport();


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
