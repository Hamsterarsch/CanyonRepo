// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserWidget.h"
#include "PrettyWidget.generated.h"


UCLASS()
class UPrettyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		void ShowWidget();

	UFUNCTION(BlueprintImplementableEvent)
		void HideWidget();


};
