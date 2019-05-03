#pragma once

#include "CoreMinimal.h"
#include "CanyonGlobalData.generated.h"

UCLASS(Blueprintable)
class CANYON_API UCanyonGlobalData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
		class UStaticMesh *pSphereMesh;
	

};
