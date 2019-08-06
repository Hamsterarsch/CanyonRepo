#include "Components/CanyonMeshCollisionComp.h"
#include "Engine/StaticMesh.h"
#include "Misc/CollisionChannels.h"

UCanyonMeshCollisionComp::UCanyonMeshCollisionComp()
{
	ToggleVisibility();
	//SetCollisionObjectType(GetCCPlaceables());
	//SetCollisionResponseToChannel(GetCCTerrain(), ECollisionResponse::ECR_Block);
}

void UCanyonMeshCollisionComp::BeginPlay()
{
	Super::BeginPlay();
	SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Ignore);


}
