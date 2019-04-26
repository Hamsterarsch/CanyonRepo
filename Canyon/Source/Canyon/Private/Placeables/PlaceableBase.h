// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlaceableBase.generated.h"

UCLASS()
class APlaceableBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlaceableBase();

	virtual void Tick(float DeltaTime) override;

	float GetPlaceableNormalZMin() const;

	float GetPlaceableNormalZMax() const;
	
	EPhysicalSurface GetPlacableSurfaceType() const { return m_TargetSurface.GetValue(); };

	void NotifyPlaceable();

	void NotifyUnplaceable();

	TArray<class UActorComponent *> GetPlaceableMeshComps();


protected:
	virtual void BeginPlay() override;


	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = "0", ClampMax = "180"))
		float m_PlaceableMinSlopeAngle;

	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = "0", ClampMax = "180"))
		float m_PlaceableMaxSlopeAngle;

	UPROPERTY(EditDefaultsOnly)
		TEnumAsByte<EPhysicalSurface> m_TargetSurface;


};
