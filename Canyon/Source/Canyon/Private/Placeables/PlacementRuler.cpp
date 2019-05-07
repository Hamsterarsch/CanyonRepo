#include "PlacementRuler.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Components/CanyonMeshCollisionComp.h"
#include "Misc/CollisionChannels.h"
//#include "Env/EnvironmentActor.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

bool CPlacementRuler::TryEnforceBuildingRules(const FHitResult &ForHit, APlaceableBase *pPlaceable, FVector &OutNewPos)
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

	FVector DispToApply{ EForceInit::ForceInitToZero };
	TArray<FHitResult> aHits;


	//query sweep hits for the hull comp
	auto *pHullComp{ Cast<UCanyonMeshCollisionComp>(pPlaceable->GetCanyonMeshCollision()) };

	auto ComponentQueryParams{ FComponentQueryParams::DefaultComponentQueryParams };
	//ComponentQueryParams.AddIgnoredActor(pPlaceable);
	ComponentQueryParams.bIgnoreTouches = true;

	TArray<FHitResult> aCompHits;
	pHullComp->GetWorld()->ComponentSweepMulti
	(
		aCompHits,
		pHullComp,
		pHullComp->GetComponentLocation(),
		ForHit.ImpactPoint + pHullComp->RelativeLocation,
		pHullComp->GetComponentQuat(),
		ComponentQueryParams
	);
			
	if(aHits.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("%i"), aHits.Num());
		//resolve slide

		//check for conflicting normals in the sweep
		/*
		if(aHits.Num() >= 2)
		{
			for(auto &&OuterHit : aHits)
			{
				for(auto &&InnerHit : aHits)
				{
					if(OuterHit == InnerHit)
					{
						continue;
					}

					if
					(
						FVector::DotProduct(OuterHit.ImpactNormal.GetUnsafeNormal2D(), InnerHit.ImpactNormal.GetUnsafeNormal2D())
						>= 0
					)
					{
						//2 normals swept that point towards each other
						return false;
					}



				}

			}
		}
		*/
		//there were blocking hits so the closest determines the hull pos
		aHits.Sort([](const decltype(aHits)::ElementType &Left, const decltype(aHits)::ElementType &Right)
		{
			return Left.Time < Right.Time;


		});

		//new placeable pos
		OutNewPos = aHits[0].Location - pHullComp->RelativeLocation;


		/*
		//no conflicting normals, determine slide direction
		auto PlacementDisp{ ForHit.ImpactPoint - pPlaceable->GetActorLocation() };
		auto SlideDirection{ PlacementDisp.GetSafeNormal2D() };

		float SlideCoeffProduct{ 1 };
		for(auto &&Hit : aHits)
		{
			//zero means no slide
			auto SlideCoeff{ 1 + FMath::Clamp(DotProduct(SlideDirection, Hit.ImpactNormal), -1, 0) };
			SlideCoeffProduct *= SlideCoeff;

			auto PlaceableLoc{ Hit.Location - Cast<USceneComponent>(MeshComps[0])->RelativeLocation };

		}

		return false;
		*/
	}
	else
	{
		//no sweep results => set to location
		OutNewPos = ForHit.ImpactPoint;	
	}
	   
	return true;


}

FVector CPlacementRuler::ComputeTerrainDepenetration(const FHitResult &ForHit, const APlaceableBase *pPlaceable)
{

	//any intersections with terrain objects ?
		//yes -> 
	return FVector::ZeroVector;

}
