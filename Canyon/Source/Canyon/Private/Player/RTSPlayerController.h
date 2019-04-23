// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RTSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CANYON_API ARTSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ARTSPlayerController();

	void HideMouseCursor()
	{
		bShowMouseCursor = false;

	}

	void ShowMouseCursor()
	{
		bShowMouseCursor = true;

	}

	

protected:


};
