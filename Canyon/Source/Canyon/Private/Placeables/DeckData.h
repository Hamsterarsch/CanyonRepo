#pragma once
#include "CoreMinimal.h"
#include "DeckData.generated.h"


USTRUCT()
struct FDeckData
{
	GENERATED_BODY()

public:
	UPROPERTY()
		TMap<FString, int32> m_ChargeMapping;

	UPROPERTY()
		TSubclassOf<class UPrettyWidget> m_DeckWidgetClass;

	UPROPERTY()
		FSoftObjectPath m_DeckAssetPath;


};
