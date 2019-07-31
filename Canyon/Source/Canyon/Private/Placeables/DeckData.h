#pragma once
#include "CoreMinimal.h"
#include "DeckData.generated.h"


USTRUCT(BlueprintType)
struct FDeckData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		TMap<FString, int32> m_ChargeMapping;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UPrettyWidget> m_DeckWidgetClass;

	UPROPERTY(EditAnywhere)
		FSoftObjectPath m_DeckAssetPath;


};

USTRUCT()
struct FChargeWrapper
{
	GENERATED_BODY()

public:
	FChargeWrapper() :
		Data()
	{		
		Data.Reserve(1);
	}

	UPROPERTY()
		TArray<TSubclassOf<class APlaceableBase>> Data;


};

USTRUCT()
struct FCarryOverCharges
{
	GENERATED_BODY()

public:
	UPROPERTY()
		TMap<FString, FChargeWrapper>
		m_Charges;


};