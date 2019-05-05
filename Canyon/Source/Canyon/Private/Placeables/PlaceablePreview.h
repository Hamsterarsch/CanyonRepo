// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Placeables/PlaceableBase.h"
#include "PlaceablePreview.generated.h"

/**
 * 
 */
UCLASS()
class APlaceablePreview : public APlaceableBase
{
	GENERATED_BODY()

public:
	APlaceablePreview();

	static APlaceablePreview *SpawnPlaceablePreview
	(
		class UWorld *pWorld,
		const FTransform &Transform, 
		TSubclassOf<APlaceableBase> PreviewedPlaceableClass
	);

	UClass *GetPreviewedClass() const;


private:
	UFUNCTION()
		void BeginOverlapDependencyRadius
		(
			UPrimitiveComponent *pOverlappedComponent,
			AActor *pOtherActor,
			UPrimitiveComponent *pOtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult &SweepResult
		);

	UFUNCTION()
		void EndOverlapDependencyRadius
		(
			UPrimitiveComponent *pOverlappedComponent,
			AActor *pOtherActor,
			UPrimitiveComponent *pOtherComp,
			int32 OtherBodyIndex
		);

	void SetInfluenceRadius(float Radius);


	UPROPERTY()
		class USphereComponent *m_pInfluenceSphere;

	UPROPERTY()
		class URadiusVisComp *m_pRadiusVisComp;

	int32 m_InfluenceCurrentGain;

	TSubclassOf<APlaceableBase> m_PreviewedClass;


};
