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
		//Pitch mapping setup
		TMap<FString, int32> Column{};
		Column.Add("House", 3);
		Column.Add("Factory", -5);
		Column.Add("Market", 7);
		m_InfluenceMapping.Add("House", std::move(Column));

		Column = TMap<FString, int32>{};
		Column.Add("House", 3);
		Column.Add("Market", 5);
		Column.Add("Factory", -9);
		m_InfluenceMapping.Add("Factory", std::move(Column));

		Column = TMap<FString, int32>{};
		Column.Add("House", 3);
		Column.Add("Factory", 6);
		Column.Add("Market", -10);
		m_InfluenceMapping.Add("Market", std::move(Column));

	}

private:
	TMap < FString, TMap<FString, int32> > m_InfluenceMapping;
	TMap < FString, TSoftClassPtr<UUserWidget> > m_UiMapping;

	
};
