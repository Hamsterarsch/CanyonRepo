#include "Components/RotatingSceneRoot.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "GameFramework/Actor.h"

URotatingSceneRoot::URotatingSceneRoot()
{
	pRotationComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("Rotator"));

}

void URotatingSceneRoot::BeginPlay()
{
	Super::BeginPlay();

	//auto *pRotationComponent{ NewObject<URotatingMovementComponent>(GetOwner()) };	
	//pRotationComponent->RegisterComponent();

	if(pRotationComponent)
	{
	pRotationComponent->SetUpdatedComponent(this);
		
	}


}
