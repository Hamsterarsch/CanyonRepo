#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RotatingSceneRoot.generated.h"


UCLASS(ClassGroup=(Movement, Canyon), Meta=(BlueprintSpawnableComponent), HideCategories=(Velocity))
class URotatingSceneRoot : public USceneComponent
{
	GENERATED_BODY()

public:
	URotatingSceneRoot();

	virtual void BeginPlay() override;


private:
	UPROPERTY(VisibleDefaultsOnly)
		class URotatingMovementComponent *pRotationComponent;


};

