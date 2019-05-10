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


protected:
	virtual void BeginPlay() override;


private:
	UPROPERTY()
		class UInfluenceDataObject *m_pInfluenceData;
	
	
};
