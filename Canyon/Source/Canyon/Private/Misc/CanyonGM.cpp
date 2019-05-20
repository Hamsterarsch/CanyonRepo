// Fill out your copyright notice in the Description page of Project Settings.

#include "CanyonGM.h"
#include "InfluenceDataObject.h"

//Public-------------

ACanyonGM::ACanyonGM() :
	m_PointsCurrent{ 0 },
	m_PointsRequired{ 0 },
	m_DeckGeneration{ 0 }
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

void ACanyonGM::AddPointsCurrent(const int32 Points)
{
	m_PointsCurrent += Points;
	ReceiveOnPointsChanged();


}

void ACanyonGM::AddPointsRequired(const int32 Points)
{
	m_PointsRequired += Points;
	ReceiveOnPointsChanged();


}


//Protected-------------------

void ACanyonGM::BeginPlay()
{
	Super::BeginPlay();

	m_pInfluenceData = UInfluenceDataObject::CreateFromFile();


}


//Private----------------------

void ACanyonGM::ReceiveOnPointsChanged()
{
	if(m_PointsCurrent >= m_PointsRequired)
	{
		InvokeNewDecks();
	}


}

void ACanyonGM::InvokeNewDecks()
{



	++m_DeckGeneration;


}
