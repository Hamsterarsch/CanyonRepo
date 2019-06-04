// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PointIndicatorWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class UPointIndicatorWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		void OnPointsCurrentChanged(int32 NewCurrentPoints);

	UFUNCTION(BlueprintImplementableEvent)
		void OnPointsRequiredChanged(int32 NewPointsRequired);

	
};
