// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "RadiusVisComp.generated.h"

/**
 * 
 */
UCLASS(CustomConstructor, Meta=(BlueprintSpawnableComponent))
class URadiusVisComp : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	URadiusVisComp();

	void SetRadius(float Radius);

	   	
};
