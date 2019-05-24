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

	UFUNCTION(BlueprintCallable)
		inline int32 GetPointsCurrent() const { return m_PointsCurrent; }

	UFUNCTION(BlueprintCallable)
		inline int32 GetPointsRequired() const { return m_PointsRequired; }

	UFUNCTION(BlueprintCallable)
		void SelectNewDeck();

	UFUNCTION()
		void OnDeckSelected(int32 DeckIndex);
	   	
	float GetPlaceableDependencyRadius(const FString &CategoryName) const;

	TSoftClassPtr<UUserWidget> GetPlaceableWidget(const FString &CategoryName) const;


	UPROPERTY(BlueprintAssignable)
		 FSimpleDynamicMulticastDelegate m_OnRequiredPointsReached;


protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
		void OnPointsChanged();

	UFUNCTION(BlueprintNativeEvent)
		TArray<class UDeckDatabaseNative *> OnInvokeNewDecks(int32 CurrentDeckGeneration);

	UFUNCTION()
		TArray<class UDeckDatabaseNative *> OnInvokeNewDecks_Implementation(int32 CurrentDeckGeneration);

	UFUNCTION(BlueprintCallable)
		TArray<class UDeckDatabaseNative *> GetRandomDecks(int32 NumDecks, FString SubCategory = "") const;


	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UDeckWidgetBase> m_DeckWidgetClass;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UPlaceableWidgetBase> m_PlaceableWidgetClass;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UPointIndicatorWidgetBase> m_PointIndicatorWidgetClass;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UPrettyWidget> m_LooseWidgetClass;


private:
	void ReceiveOnPointsChanged();

	void ReceiveOnInvokeNewDecks();

	void AddPointsRequired(int32 Points);

	void OnLoose();


	UPROPERTY()
		class UInfluenceDataObject *m_pInfluenceData;

	UPROPERTY()
		class UDeckWidgetBase *m_pDeckWidget;

	UPROPERTY()
		class UPlaceableWidgetBase *m_pPlaceableWidget;

	UPROPERTY()
		class UPointIndicatorWidgetBase *m_pPointWidget;

	UPROPERTY()
		class UPrettyWidget *m_pLooseWidget;

	UPROPERTY()
		TArray<class UDeckDatabaseNative *> m_apCurrentDeckData;

	int32 m_PointsCurrent;
	int32 m_PointsRequired;
	int32 m_DeckGenerationCurrent;
	int32 m_BuildingsRemaining;
		
	
};

