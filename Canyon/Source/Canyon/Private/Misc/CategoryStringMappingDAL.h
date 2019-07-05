#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SlateFontInfo.h"
#include "CategoryStringMappingDAL.generated.h"

UCLASS(Blueprintable)
class UCategoryStringMappingDAL : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
		FString GetStringForCategory(const FString &Category);

	FString GetStringForCategory_Implementation(const FString &Category);
		

};
