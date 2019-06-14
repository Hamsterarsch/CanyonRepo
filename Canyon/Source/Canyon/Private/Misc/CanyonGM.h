// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CanyonGM.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSimpleDynamicMulticastDelegate);

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

	int32 GetInfluenceForBaseCategory(const FString &CategoryName) const;

	void AddPointsCurrent(int32 Points);

	void AddPointsRequiredFor(const FString &Category, uint32 Amount);


	UFUNCTION(BlueprintCallable)
		inline int32 GetPointsCurrent() const { return m_PointsCurrent; }

	UFUNCTION(BlueprintCallable)
		inline int32 GetPointsRequired() const { return m_PointsRequired; }

	float GetPlaceableDependencyRadius(const FString &CategoryName) const;

	TSubclassOf<class UPlaceableIconWidgetBase> GetPlaceableWidget(const FString &CategoryName) const;

	FSimpleDynamicMulticastDelegate m_OnRequiredPointsReached;


protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
		void OnPointsChanged();

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UPointIndicatorWidgetBase> m_PointIndicatorWidgetClass;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UPrettyWidget> m_LooseWidgetClass;


private:
	void ReceiveOnPointsChanged();

	void AddPointsRequired(int32 Points);

	void OnLoose();


	UPROPERTY()
		class UInfluenceDataObject *m_pInfluenceData;

	UPROPERTY()
		class UPointIndicatorWidgetBase *m_pPointWidget;

	UPROPERTY()
		class UPrettyWidget *m_pLooseWidget;

	int32 m_PointsCurrent;
	int32 m_PointsRequired;
	int32 m_SessionSeed;
		
	
};

