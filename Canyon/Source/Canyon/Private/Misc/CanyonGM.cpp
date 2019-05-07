// Fill out your copyright notice in the Description page of Project Settings.


#include "CanyonGM.h"

int32 ACanyonGM::GetInfluenceForPlaceable
(
	const FString &FirstInfluenceQualifier,
	const FString &SecondInfluenceQualifier
)	const
{
	return m_InfluenceMapping[FirstInfluenceQualifier][SecondInfluenceQualifier];


}
