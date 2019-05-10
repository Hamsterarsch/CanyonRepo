// Fill out your copyright notice in the Description page of Project Settings.

#include "CanyonGM.h"
#include "InfluenceDataObject.h"

//Public-------------

int32 ACanyonGM::GetInfluenceForPlaceable
(
	const FString &FirstInfluenceQualifier,
	const FString &SecondInfluenceQualifier
)	const
{
	return (*m_pInfluenceData)[FirstInfluenceQualifier][SecondInfluenceQualifier];
	

}


//Protected-------------------

void ACanyonGM::BeginPlay()
{
	Super::BeginPlay();

	m_pInfluenceData = UInfluenceDataObject::CreateFromFile();


}
