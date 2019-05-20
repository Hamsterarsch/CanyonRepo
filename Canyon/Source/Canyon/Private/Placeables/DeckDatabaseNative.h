#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DeckDatabaseNative.generated.h"

UCLASS()
class UDeckDatabaseNative : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		class UUserDefinedStruct *GetDeckData() const;

	UFUNCTION(BlueprintImplementableEvent)
		int32 GetRequiredGeneration() const;


};