#include "Components/CanyonMeshCollisionComp.h"
#include "Engine/StaticMesh.h"
#include "Misc/CollisionChannels.h"

UCanyonMeshCollisionComp::UCanyonMeshCollisionComp()
{
	ToggleVisibility();
	SetCollisionObjectType(GetCCPlaceables());
	SetCollisionResponseToChannel(GetCCTerrain(), ECollisionResponse::ECR_Ignore);
}

void UCanyonMeshCollisionComp::BeginPlay()
{
	Super::BeginPlay();

}
