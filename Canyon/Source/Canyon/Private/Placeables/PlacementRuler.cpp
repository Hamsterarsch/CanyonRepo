#include "PlacementRuler.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
//#include "Env/EnvironmentActor.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

bool CPlacementRuler::TryEnforceBuildingRules(const FHitResult &ForHit, APlaceableBase *pPlaceable)
{
	//preconditions
	//ForHit resulted form a trace against a per se valid placement surface (not surface type valid)

	if(!pPlaceable)
	{
		return false;
	}	

	//Normal constraint
	constexpr float NormalTolerance{ 10e-6 };
	auto ImpactNormalZ{ ForHit.ImpactNormal.GetUnsafeNormal().Z };
	if
	(
		(ImpactNormalZ + NormalTolerance) <= pPlaceable->GetPlaceableNormalZMin() 
		|| (ImpactNormalZ - NormalTolerance) >= pPlaceable->GetPlaceableNormalZMax() 
	)
	{
		return false;
	}

	//Surface type constraint
	if (auto *pPhyMat{ ForHit.PhysMaterial.Get() })
	{
		auto SurfaceType{ UPhysicalMaterial::DetermineSurfaceType(pPhyMat) };
		if
		(
			SurfaceType != pPlaceable->GetPlacableSurfaceType()
			&& SurfaceType != EPhysicalSurface::SurfaceType_Default
		)
		{
			return false;
		}
	}

	//TerrainOverlapping
	/*
	TArray<AActor *> aOverlappers;
	pPlaceable->GetOverlappingActors(aOverlappers, AEnvironmentActor::StaticClass());
	if(aOverlappers.Num() > 0)
	{
		
	}
	*/
	return true;


}

FVector CPlacementRuler::ComputeTerrainDepenetration(const FHitResult &ForHit, const APlaceableBase *pPlaceable)
{

	//any intersections with terrain objects ?
		//yes -> 
	return FVector::ZeroVector;

}
