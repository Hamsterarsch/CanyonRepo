#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InfluenceFloatMapDAL.generated.h"

UCLASS(Blueprintable)
class UInfluenceFloatMapDAL : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
		float GetValueForCategory(const FString &CategoryName) const;

	UFUNCTION()
		float GetValueForCategory_Implementation(const FString &CategoryName) const { return -1; }

	UFUNCTION(BlueprintImplementableEvent)
		TMap<FString, int32> GetMap() const;


};
