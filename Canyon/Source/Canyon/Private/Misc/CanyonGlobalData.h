#pragma once

#include "CoreMinimal.h"
#include "CanyonGlobalData.generated.h"

UCLASS()
class CANYON_API UCanyonGlobalData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FName m_TestPropertyName{ "Test" };
	

};
