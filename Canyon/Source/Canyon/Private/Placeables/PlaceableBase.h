// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorAutoCollectedInstancedMeshes.h"
#include "Engine/UserDefinedEnum.h"
#include "PlaceableBase.generated.h"


UCLASS()
class APlaceableBase : public AActorDeferredPlay
{
	GENERATED_BODY()
	
public:	
	APlaceableBase();

	float GetPlaceableNormalZMin() const;

	float GetPlaceableNormalZMax() const;
	
	EPhysicalSurface GetPlacableSurfaceType() const { return m_TargetSurface.GetValue(); };

	TArray<class UActorComponent *> GetPlaceableMeshComps();

	inline class UCanyonMeshCollisionComp *GetCanyonMeshCollision() { return m_pMeshCollisionComp; }

	UClass *GetInfluenceWidgetClass() const;

	int32 BeginInfluenceVisFor(const TSubclassOf<APlaceableBase> &TargetClass);

	int32 EndInfluenceVis();

	FString GetPlaceableCategory() const;

	inline bool IsHightlighted() const { return m_bIsHighlighted; }

	UFUNCTION(BlueprintImplementableEvent)
		int32 GetInfluenceEnumValue() const;

	UFUNCTION(BlueprintImplementableEvent)
		UUserDefinedEnum *GetInfluenceEnumClass() const;

	void ToggleSelectionHighlight();
	

protected:
	void SetInfluenceDisplayed(int32 Influence);

	void InitInfluenceDisplayWidget(UClass *pClass);
	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
		//Returns wheter the building is now selected (true) or not
		bool OnToggleSelectionHighlight();
		

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UInfluenceDisplayWidgetBase> m_InfluenceDisplayWidgetClass;

	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = "0", ClampMax = "180"))
		float m_PlaceableMinSlopeAngle;

	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = "0", ClampMax = "180"))
		float m_PlaceableMaxSlopeAngle;

	UPROPERTY(EditDefaultsOnly)
		TEnumAsByte<EPhysicalSurface> m_TargetSurface;

	UPROPERTY()
		class UWidgetComponent *m_pInfluenceWidgetComp;

	UPROPERTY(VisibleDefaultsOnly)
		class UCanyonMeshCollisionComp *m_pMeshCollisionComp;

	UPROPERTY(BlueprintReadOnly)
		bool m_bIsHighlighted;


private:
	static FString GetInfluenceQualifier(const TSubclassOf<APlaceableBase>& ForClass);




};
