// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CanyonGM.generated.h"

/**
 * 
 */
UCLASS()
class ACanyonGM : public AGameModeBase
{
	GENERATED_BODY()

public:
	int32 GetInfluenceForPlaceable(const FString &FirstInfluenceQualifier, const FString &SecondInfluenceQualifier) const;

	ACanyonGM()
	{
		TMap<FString, int32> d{};
		d.Add("House", 3);
		m_InfluenceMapping.Add("House", std::move(d));
	}

private:
	TMap < FString, TMap<FString, int32> > m_InfluenceMapping;
	TMap < FString, TSoftClassPtr<UUserWidget> > m_UiMapping;

	
};
