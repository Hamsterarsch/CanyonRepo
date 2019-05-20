// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CanyonGM.generated.h"

/**
 * 
 */
UCLASS(CustomConstructor)
class ACanyonGM : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACanyonGM();

	int32 GetInfluenceForPlaceable(const FString &FirstInfluenceQualifier, const FString &SecondInfluenceQualifier) const;

	void AddPointsCurrent(int32 Points);

	void AddPointsRequired(int32 Points);

	UFUNCTION(BlueprintCallable)
		inline int32 GetPointsCurrent() const { return m_PointsCurrent; }

	UFUNCTION(BlueprintCallable)
		inline int32 GetPointsRequired() const { return m_PointsRequired; }


protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
		void OnPointsChanged();


private:
	void ReceiveOnPointsChanged();

	void InvokeNewDecks();


	UPROPERTY()
		class UInfluenceDataObject *m_pInfluenceData;

	int32 m_PointsCurrent;
	int32 m_PointsRequired;
	int32 m_DeckGeneration;
	
	
};
