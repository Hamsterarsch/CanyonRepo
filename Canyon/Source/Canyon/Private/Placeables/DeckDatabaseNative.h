#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SoftObjectPtr.h"
#include "Engine/UserDefinedStruct.h"
#include "DeckDatabaseNative.generated.h"

UCLASS(Blueprintable)
class UDeckDatabaseNative : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		TSubclassOf<UUserWidget> GetDeckWidget() const;

	UFUNCTION(BlueprintImplementableEvent)
		 FString GetDependencyCategoryAtIndex(int32 Index) const;

	UFUNCTION(BlueprintImplementableEvent)
		int32 GetMinAmountAtIndex(int32 Index) const;
	
	UFUNCTION(BlueprintImplementableEvent)
		 int32 GetMaxAmountAtIndex(int32 Index) const;

	UFUNCTION(BlueprintImplementableEvent)
		int32 GetRequiredDeckGeneration() const;


};