#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "CanyonMeshCollisionComp.generated.h"

/**
 *
 */
UCLASS(ClassGroup = (Canyon), Meta = (BlueprintSpawnableComponent))
class UCanyonMeshCollisionComp : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UCanyonMeshCollisionComp();

	virtual void BeginPlay() override;


};

