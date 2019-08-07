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

	virtual void Shutdown() override;

	//should commit any analytics data, save the game and init the level switch
	void BeginSwitchToNextLevel(const TSoftObjectPtr<UWorld> &NewLevel);

	UFUNCTION(BlueprintCallable)
		void ExitGameloop();

	UFUNCTION(BlueprintCallable)
		//menu hook
		void StartupGame(bool bContinueGame);

	UFUNCTION(BlueprintCallable)
		int32 GetCarryOverScore() const { return m_CarryOverScore; }

	UFUNCTION(BlueprintCallable)
		inline bool WasGameStarted() { return m_bWasGameStarted; }

	void BuildCarryOverChargesFormSelection(const TArray<APlaceableBase *> &apSelected);


protected:
	UFUNCTION(BlueprintImplementableEvent)
		void NotifyStartupGame();

	UFUNCTION(BlueprintImplementableEvent)
		void NotifyExitGameloop();

	UFUNCTION(BlueprintImplementableEvent)
		class UUserWidget *OnBeginLoadingScreen(const FString &MapName);

	UFUNCTION(BlueprintImplementableEvent)
		void OnEndLoadingScreen(UWorld *pLoadedWorld);


	UPROPERTY(EditDefaultsOnly, Category="Canyon|Level")
		TArray<TSoftObjectPtr<UWorld>> m_aFirstLevelsPool;

	UPROPERTY(EditDefaultsOnly, Category="Canyon|Level")
		TSoftObjectPtr<UWorld> m_MainMenuLevel;

	UPROPERTY(EditDefaultsOnly)
		bool m_bLoadAllPlaceablesOnStartup;

	UPROPERTY(EditDefaultsOnly)
		float m_LoadingScreenTransitionTime;


private:
	void LoadPlaceables();

	void SetupLoadingScreenReferences(class UUserWidget *pLoadingScreen);

	void FetchCarryOverDataFromOldLevel(const UWorld *pWorld);

	void SetupCarryOverDataInNewLevel(UWorld *pWorld) const;

	void CleanupWorld(UWorld *pWorld, bool bSessionEnded, bool bCleanupResources);


	UFUNCTION()
		void OnLoadingScreenTransitionTimeExpired();

	UFUNCTION()
		void ReceiveOnPreMapLoaded(const FString &MapName);

	UFUNCTION()
		void ReceiveOnPostMapLoaded(UWorld *pLoadedWorld);
		
	UPROPERTY()
		FCarryOverCharges m_CarryOverCharges;

	UPROPERTY()
		TArray<UObject *> m_apPreLoadedPlaceables;

	UPROPERTY()
		UWorld *m_pTargetWorld;

	int32 m_CarryOverScore;

	int32 m_Seed;

	bool m_bHasLoadingScreenBegun;

	TSharedPtr<class SObjectWidget> m_pLoadingScreenGC;

	TSharedPtr<class SWidget> m_pLoadingScreenSlate;

	bool m_bWasGameStarted;


};
