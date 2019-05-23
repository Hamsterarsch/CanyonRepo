#include "PlacementRuler.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Components/CanyonMeshCollisionComp.h"
#include "Misc/CollisionChannels.h"
#include "DrawDebugHelpers.h"
#include "Engine/LocalPlayer.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

CPlacementRuler::CPlacementRuler() :
	m_bInResnapRecovery{ false }
{
}

//outputs a new position for the cursor root and whether or not the building could be placed there
bool CPlacementRuler::HandleBuildingRules(APlaceableBase *pPlaceable, FVector &out_NewPos)
{
	//preconditions
	//ForHit resulted form a trace against a per se valid placement surface (not surface type valid)
	//pPlaceable points to a valid object instance

	//Get hit or
	FHitResult TerrainHit;
	TraceForTerrainUnderCursor(TerrainHit, pPlaceable->GetWorld());
	
	//todo: handle no hit situations	
	if(!TerrainHit.IsValidBlockingHit())
	{
		//hit air, get last valid pos and intersection
		/*
		auto *pViewportClient{ pPlaceable->GetWorld()->GetFirstLocalPlayerFromController()->ViewportClient };
		
		
		pViewportClient->Viewport,
		pPlaceable->GetWorld()->Scene,
		pViewportClient->EngineShowFlags

		FVector CamPos;
		FRotator CamRot;
		pPlaceable->GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(CamPos, CamRot);
		
		//look at this impl
		*/
		auto *pPc{ pPlaceable->GetWorld()->GetFirstPlayerController() };
		FVector2D MousePos;
		pPc->GetMousePosition(MousePos.X, MousePos.Y);
		FVector ScreenWorldPos, ScreenWorldDir;
		pPc->DeprojectScreenPositionToWorld(MousePos.X, MousePos.Y, ScreenWorldPos, ScreenWorldDir);

		const float TargetZHeight{ m_LastHitPosition.Z };
		const auto RayExtrusionFactor{ (TargetZHeight - ScreenWorldPos.Z) / ScreenWorldDir.Z };

		out_NewPos = ScreenWorldPos + ScreenWorldDir * RayExtrusionFactor;
		return false;


	}
	//depen
	TerrainHit.ImpactPoint.Z += .5;
	
	//Normal constraint
	constexpr float NormalTolerance{ 10e-6 };
	auto ImpactNormalZ{ TerrainHit.ImpactNormal.GetUnsafeNormal().Z };
	if
	(
		(ImpactNormalZ + NormalTolerance) <= pPlaceable->GetPlaceableNormalZMin() 
		|| (ImpactNormalZ - NormalTolerance) >= pPlaceable->GetPlaceableNormalZMax() 
	)
	{
		out_NewPos = TerrainHit.ImpactPoint;
		return false;
	}

	//Surface type constraint
	if (auto *pPhyMat{ TerrainHit.PhysMaterial.Get() })
	{
		auto SurfaceType{ UPhysicalMaterial::DetermineSurfaceType(pPhyMat) };
		if
		(
			SurfaceType != pPlaceable->GetPlacableSurfaceType()
			&& SurfaceType != EPhysicalSurface::SurfaceType_Default
		)
		{
			out_NewPos = TerrainHit.ImpactPoint;
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

	//Mouse distance override
	FVector2D CursorRootScreenPos;
	FVector2D MousePos;
	pPlaceable->GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(pPlaceable->GetActorLocation(), CursorRootScreenPos);
	pPlaceable->GetWorld()->GetFirstPlayerController()->GetMousePosition(MousePos.X, MousePos.Y);

	//snap after some distance between mouse and building
	auto Dist{ FVector2D::Distance(CursorRootScreenPos, MousePos) };
	if(Dist > 130)
	{
		m_bInResnapRecovery = true;
		out_NewPos = TerrainHit.ImpactPoint;
		return false;
	}

	//handle building penetrations after snapping
	auto *pHullComp{ Cast<UCanyonMeshCollisionComp>(pPlaceable->GetCanyonMeshCollision()) };
	if(m_bInResnapRecovery)
	{
		auto ComponentQueryParams{ FComponentQueryParams::DefaultComponentQueryParams };
		ComponentQueryParams.AddIgnoredActor(pPlaceable);
		ComponentQueryParams.bTraceComplex = true;
		
		TArray<FOverlapResult> aOverlaps;
		pHullComp->GetWorld()->ComponentOverlapMulti
		(
			aOverlaps,
			pHullComp,
			pHullComp->GetComponentLocation(),
			pHullComp->GetComponentQuat(),
			ComponentQueryParams
		);

		UE_LOG(LogTemp, Log, TEXT("=>  %i"), aOverlaps.Num());
		if(aOverlaps.Num() != 0)
		{
			out_NewPos = TerrainHit.ImpactPoint;
			return false;


		}
		m_bInResnapRecovery = false;		
	}


	FVector DispToApply{ EForceInit::ForceInitToZero };
	TArray<FHitResult> aHits;


	//query sweep hits for the hull comp
	{
		auto ComponentQueryParams{ FComponentQueryParams::DefaultComponentQueryParams };
		ComponentQueryParams.AddIgnoredActor(pPlaceable);
		ComponentQueryParams.bIgnoreTouches = true;
		ComponentQueryParams.bTraceComplex = true;

		pHullComp->GetWorld()->ComponentSweepMulti
		(
			aHits,
			pHullComp,
			pHullComp->GetComponentLocation(),
			TerrainHit.ImpactPoint + pHullComp->RelativeLocation,
			pHullComp->GetComponentQuat(),
			ComponentQueryParams
		);
	}

	if(aHits.Num() > 0)
	{
		int HitsPenetratedOnStart{ 0 };
		for(auto &&Hit : aHits)
		{
			if(Hit.bStartPenetrating)
			{
				++HitsPenetratedOnStart;
			}
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Num Hits: %i,\t Num Pen: %i"), aHits.Num(), HitsPenetratedOnStart);

		if(HitsPenetratedOnStart > 0)
		{
			out_NewPos = TerrainHit.ImpactPoint;
			return false;
		}

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
		auto DepenetrationDisp{ aHits[0].ImpactNormal };
		if (aHits.Num() == 2)
		{
			DepenetrationDisp += aHits[1].ImpactNormal;
			DepenetrationDisp /= 2;
		}

		if(aHits.Num() > 2)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit slides: %i"), aHits.Num());
		}

		out_NewPos = aHits[0].Location - pHullComp->RelativeLocation + DepenetrationDisp;
		out_NewPos.Z = TerrainHit.ImpactPoint.Z;
		return true;
		
	}
	else
	{
		//no sweep results => set to location
		out_NewPos = TerrainHit.ImpactPoint;
		return true;
	}


}

FVector CPlacementRuler::ComputeTerrainDepenetration(const FHitResult &ForHit, const APlaceableBase *pPlaceable)
{

	//any intersections with terrain objects ?
		//yes -> 
	return FVector::ZeroVector;

}

void CPlacementRuler::HandlePenetratingHits(APlaceableBase *pPlaceable) const
{
	

}

bool TraceForTerrainUnderCursor(FHitResult& OutHit, const UWorld* pWorld)
{
	return pWorld->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel3, true, OutHit);
	   

}
