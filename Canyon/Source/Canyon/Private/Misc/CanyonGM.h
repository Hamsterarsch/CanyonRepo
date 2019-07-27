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

	void BeginGame();

	void InitPointState(int32 CarryOverPoints);

	void AddCarryOverChargesToIssued(const struct FCarryOverCharges &DeckData);

	int32 GetInfluenceForPlaceable(const FString &FirstInfluenceQualifier, const FString &SecondInfluenceQualifier) const;

	int32 GetInfluenceBasisForCategory(const FString &CategoryName) const;
	
	const TMap<FString, int32> &GetTempInfluenceMappingForCategory(const FString &CategoryName) const;

	void AddPointsCurrent(int32 Points);

	void IncreaseDeckGeneration();

	TArray<struct FDeckData> GetDeckData(int32 Amount = 2);

	FDeckData GetEndlessDeckData();

	void FillUpDeckDataNonEndless(FDeckData &DeckData);
	
	inline class AMeshInstancer *GetMeshInstancer() { return m_pMeshInstancer; }

	void NotifyPlaceableActionSelect(FHitResult &Hit);

	UFUNCTION(Exec)
		void DebugAddChargesForCategory(const FString &Category, int32 Num) const;

	UFUNCTION(BlueprintCallable)
		bool IsInEndlessMode() const;

	UFUNCTION(BlueprintCallable)
		FString GetPrettyNameForCategory(const FString &CategoryName);

	UFUNCTION(BlueprintCallable)
		inline int32 GetPointsCurrent() const { return m_PointsCurrent; }

	UFUNCTION(BlueprintCallable)
		inline int32 GetPointsRequired() const { return m_PointsRequired; }

	UFUNCTION(BlueprintCallable)
		inline bool GetIsNextLevelAccessible() const { return m_bIsNextLevelAccessible; }

//Level switch interface
	UFUNCTION(BlueprintCallable)
		void EnterNextLevel();

	UFUNCTION(BlueprintCallable)
		void EnterPlaceableSelectionMode();

	UFUNCTION(BlueprintCallable)
		void AbortPlaceableSelectionMode();

	UFUNCTION(BlueprintCallable)
		bool TryCommitPlaceableSelection();

	UFUNCTION(BlueprintCallable)
		void ForceCommitPlaceableSelection();
//end level switch interface

	UFUNCTION(BlueprintCallable)
		int32 GetRequiredPointsDeltaForNextLevel() const { return m_NextLevelRequiredPointsDelta; }

	float GetPlaceableDependencyRadius(const FString &CategoryName) const;

	TSubclassOf<class UPlaceableIconWidgetBase> GetPlaceableWidget(const FString &CategoryName) const;

	FSimpleDynamicMulticastDelegate m_OnRequiredPointsReached;


protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
		void OnPointsChanged();

	UFUNCTION(BlueprintImplementableEvent)
		void NotifyOnLoose();

	UFUNCTION(BlueprintImplementableEvent)
		void NotifyOnNewDeckAvailable();

	UFUNCTION(BlueprintImplementableEvent)
		void NotifyNextLevelAvailable();


	UPROPERTY(EditDefaultsOnly, Category="Canyon|UI")
		class UCategoryStringMappingDAL *m_pPrettyCategoryNameSource;

	UPROPERTY(EditDefaultsOnly, Category="Canyon|Level")
		UCurveFloat *m_pRequiredPointsSource;

	UPROPERTY(EditDefaultsOnly, Category="Canyon|Level")
		class UInfluenceFloatMapDAL *m_pDeckFillerProbOverride;
	
	UPROPERTY(EditDefaultsOnly, Category="Canyon|Level")
		TSubclassOf<class UDeckSelector> m_DeckSelectorClass;

	UPROPERTY(EditDefaultsOnly, Category="Canyon|Level")
		TArray<TSoftObjectPtr<UWorld>> m_aNextLevelsPool;

	UPROPERTY(EditDefaultsOnly, Category="Canyon|Level")
		int32 m_NextLevelRequiredPointsDelta;

	UPROPERTY(EditDefaultsOnly, Category="Canyon|Level", Meta=(ClampMin="1"))
		int32 m_CarryOverBuildingCountMax;


private:
	void ReceiveOnPointsChanged();

	void SetPointsRequired(int32 Points);

	UPROPERTY()
		class UDeckSelector *m_pDeckSelector;

	UPROPERTY()
		class UInfluenceDataObject *m_pInfluenceData;

	UPROPERTY()
		int32 m_PointsOnLevelOpen;

	UPROPERTY()
		bool m_bIsNextLevelAccessible;

	UPROPERTY()
		class AMeshInstancer *m_pMeshInstancer;

	UPROPERTY()
		TArray<class APlaceableBase *> m_apSelectedCarryPlaceables;

	int32 m_PointsCurrent;
	int32 m_PointsRequired;
	bool m_bIsInPlaceableSelectionMode;
		
	
};

