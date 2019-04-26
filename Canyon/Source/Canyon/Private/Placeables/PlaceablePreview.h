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
	TSubclassOf<APlaceableBase> m_PreviewedClass;


};
