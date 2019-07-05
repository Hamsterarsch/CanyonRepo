// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Placeables/DeckData.h"
#include "CanyonGI.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class UCanyonGI : public UGameInstance
{
	GENERATED_BODY()

public:
	UCanyonGI();

	virtual void Init() override;


	//should commit any analytics data, save the game and init the level switch
	void BeginLevelSwitch(const TSoftObjectPtr<UWorld> &NewLevel);


	UFUNCTION(BlueprintCallable)
		//menu hook
		void StartupGame(bool bContinueGame);

	UFUNCTION(BlueprintCallable)
		int32 GetCarryOverScore() const { return m_CarryOverScore; }

	void AddCarryOverCharge(const FString &CategoryName);

	void RemoveCarryOverCharge(const FString &CategoryName);


protected:
	UFUNCTION(BlueprintImplementableEvent)
		void OnBeginLoadingScreen(const FString &MapName);

	UFUNCTION(BlueprintImplementableEvent)
		void OnEndLoadingScreen(UWorld *pLoadedWorld);


	UPROPERTY(EditDefaultsOnly, Category="Canyon|Level")
		TArray<TSoftObjectPtr<UWorld>> m_aFirstLevelsPool;

	UPROPERTY(EditDefaultsOnly)
		bool m_bLoadAllPlaceablesOnStartup;


private:
	void FetchCarryOverDataFromOldLevel(const UWorld *pWorld);

	void SetupCarryOverDataInNewLevel(UWorld *pWorld) const;


	UFUNCTION()
		void ReceiveOnPreMapLoaded(const FString &MapName);

	UFUNCTION()
		void ReceiveOnPostMapLoaded(UWorld *pLoadedWorld);
		
	UPROPERTY()
		FDeckData m_CarryOverCharges;

	UPROPERTY()
		TArray<UObject *> m_apPreLoadedPlaceables;

	int32 m_CarryOverScore;

	int32 m_Seed;


};
