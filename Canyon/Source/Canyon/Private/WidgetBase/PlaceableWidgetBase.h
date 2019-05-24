// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlaceableWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class UPlaceableWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		void AddPlaceableWidget(TSubclassOf<UUserWidget> WidgetClass, int32 Amount, const FString &DependencyCatName);

	
};
