// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "StaticMeshCanyonComp.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Rendering, Canyon), Meta=(BlueprintSpawnableComponent))
class UStaticMeshCanyonComp : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UStaticMeshCanyonComp();

	virtual void BeginPlay() override;

};
