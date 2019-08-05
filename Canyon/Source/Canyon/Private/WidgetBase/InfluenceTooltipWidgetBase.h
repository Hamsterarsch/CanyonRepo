//

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InfluenceTooltipWidgetBase.generated.h"


/**
 * 
 */
UCLASS()
class UInfluenceTooltipWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		void SetBaseValue(int32 Value);

	UFUNCTION(BlueprintImplementableEvent)
		void SetHeaderName(const FString &Name);

	UFUNCTION(BlueprintImplementableEvent)
		void SetInfluenceRelationships(const TMap<FString, int32> &InfluenceMapping);

	
};
