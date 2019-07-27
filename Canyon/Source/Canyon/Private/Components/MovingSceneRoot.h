#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
//#include "MovingSceneRoot.generated.h"

/*
UCLASS(ClassGroup=(Movement, Canyon), Meta=(BlueprintSpawnableComponent), HideCategories=(Velocity))
class UMovingSceneRoot : public USceneComponent
{
	GENERATED_BODY()

public:
	UMovingSceneRoot();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void BeginPlay() override;

#if WITH_EDITOR	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif


protected:
	UPROPERTY(EditAnywhere, DisplayName="Position On One", Meta=(MakeEditWidget))
		FVector m_OnePosition;
		
	UPROPERTY(EditAnywhere, DisplayName="Position On Minus One", Meta=(MakeEditWidget))
		FVector m_OneMinusPosition;

	UPROPERTY(EditAnywhere, Instanced, DisplayName="Dynamics")
		class UCurveFloat *m_pDynamics;

	UPROPERTY(EditAnywhere)
		float m_IntervalLength;


private:
	float m_CurrentIntervalPos;
	FVector m_StartingRootPosition;

#if WITH_EDITOR
	UPROPERTY()
		class UArrowComponent *m_pToOneArrow;

	UPROPERTY()
		class UArrowComponent *m_pToOneMinusArrow;
#endif


};

void SpanArrowComponent(const FVector &From, const FVector &To, class UArrowComponent *pArrow);

*/