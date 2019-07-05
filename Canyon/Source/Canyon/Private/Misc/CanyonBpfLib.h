// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CanyonBpfLib.generated.h"

/**
 * 
 */
UCLASS()
class UCanyonBpfLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		static TSubclassOf<class APlaceableBase> GetCategoryPlaceableClass(FString Category);

	static TArray<class UDeckDatabaseNative *> GetRandomDecks(const int32 NumDecks, FString SubCategory = "");

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static uint8 EnumStringToEnumByte(const class UUserDefinedEnum *pEnum, const FString &EnumIdentifier);

	UFUNCTION(BlueprintCallable)
		static void SetBrushImage(UPARAM(ref) struct FSlateBrush &Target, UObject *pObject);
	

};
