#include "Components/CanyonMeshCollisionComp.h"
#include "Engine/StaticMesh.h"
#include "Misc/CollisionChannels.h"

UCanyonMeshCollisionComp::UCanyonMeshCollisionComp()
{
	ToggleVisibility();
	SetCollisionProfileName(TEXT("Placeables"));
	//SetCollisionObjectType(GetCCPlaceables());
	//SetCollisionResponseToChannel(GetCCTerrain(), ECollisionResponse::ECR_Block);
}

void UCanyonMeshCollisionComp::BeginPlay()
{
	Super::BeginPlay();

}
