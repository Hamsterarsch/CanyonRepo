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

	inline int32 GetCurrentInfluence() const { return m_InfluenceCurrentGain; }

	void NotifyPlaceable();

	void NotifyUnplaceable();


private:
	UFUNCTION()
		void ActorBeginOverlap(AActor *pOverlappedActor, AActor *pOtherActor);

	UFUNCTION()
		void ActorEndOverlap(AActor *pOverlappedActor, AActor *pOtherActor);

	void SetInfluenceRadius(float Radius);

	void SetMaterialForAllMeshes(UMaterialInterface *pMaterial);


	UPROPERTY()
		class USphereComponent *m_pInfluenceSphere;

	UPROPERTY()
		class URadiusVisComp *m_pRadiusVisComp;

	UPROPERTY()
		UMaterialInterface *m_pMaterialUnplaceable;

	UPROPERTY()
		UMaterialInterface *m_pMaterialPlaceable; 

	int32 m_InfluenceCurrentGain;

	TSubclassOf<APlaceableBase> m_PreviewedClass;


};
