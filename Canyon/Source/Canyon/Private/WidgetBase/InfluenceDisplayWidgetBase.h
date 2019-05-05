// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InfluenceDisplayWidgetBase.generated.h"


/**
 * 
 */
UCLASS()
class UInfluenceDisplayWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		void BeginDisplayInfluence(int32 Influence);

	UFUNCTION(BlueprintImplementableEvent)
		int32 EndDisplayInfluence();

	
};
