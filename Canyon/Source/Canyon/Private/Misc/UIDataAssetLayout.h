#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SlateFontInfo.h"
#include "UIDataAssetLayout.generated.h"

UCLASS(Blueprintable)
class UUIDataAssetLayout : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		FSlateFontInfo m_FontChargeCounterDefault;


};