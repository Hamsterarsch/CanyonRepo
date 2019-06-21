#include "PlacementRuler.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Components/CanyonMeshCollisionComp.h"
#include "Misc/CollisionChannels.h"
#include "DrawDebugHelpers.h"
#include "Engine/LocalPlayer.h"
#include "Misc/CanyonLogs.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

CPlacementRuler::CPlacementRuler() :
	m_bInResnapRecovery{ false },
	m_LastTerrainTracePos{ EForceInit::ForceInitToZero },
	m_LastPlaceablePosition{ EForceInit::ForceInitToZero }
{
}

bool CPlacementRuler::HandleBuildingRules(APlaceableBase* pPlaceable, FVector& out_NewPos)
{
	m_bLastRet = HandleBuildingRulesInternal(pPlaceable, m_LastPlaceablePosition);
	out_NewPos = m_LastPlaceablePosition;

	return m_bLastRet;


}

//outputs a new position for the cursor root and whether or not the building could be placed there
bool CPlacementRuler::HandleBuildingRulesInternal(APlaceableBase *pPlaceable, FVector &out_NewPos)
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

		const float TargetZHeight{ m_LastTerrainTracePos.Z };
		const auto RayExtrusionFactor{ (TargetZHeight - ScreenWorldPos.Z) / ScreenWorldDir.Z };

		UE_LOG(LogCanyonPlacementRuler, Log, TEXT("Building placement denied bc there is no terrain"));

		out_NewPos = ScreenWorldPos + ScreenWorldDir * RayExtrusionFactor;
		return false;


	}
	//depen
	TerrainHit.ImpactPoint.Z += 1;
	m_LastTerrainTracePos = TerrainHit.ImpactPoint;


//trivial (+ todo: also 'fixes' false positive sweep result, bc sweep with no delta doesnt hit --> use overlap
	/*auto MovementDelta{FVector::Dist(m_LastPlaceablePosition, TerrainHit.ImpactPoint) };	
	if(FMath::IsNearlyZero(MovementDelta, 0.0001f ))
	{
		UE_LOG(LogCanyonPlacementRuler, Log, TEXT("Postponing placement evaluation due to no changed mouse pos: %f"), MovementDelta);

		out_NewPos = m_LastPlaceablePosition;
		return m_bLastRet;
	}*/
	

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

		UE_LOG(LogCanyonPlacementRuler, Log, TEXT("Building placement denied bc the terrain normals are invalid"));

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
			UE_LOG(LogCanyonPlacementRuler, Log, TEXT("Building placement denied bc of invalid terrain type"));

			out_NewPos = TerrainHit.ImpactPoint;
			return false;
		}
	}


//Mouse distance override
	FVector2D CursorRootScreenPos;
	FVector2D MousePos;
	pPlaceable->GetWorld()->GetFirstPlayerController()->ProjectWorldLocationToScreen(pPlaceable->GetActorLocation(), CursorRootScreenPos);
	pPlaceable->GetWorld()->GetFirstPlayerController()->GetMousePosition(MousePos.X, MousePos.Y);

	//snap after some distance between mouse and building
	auto Dist{ FVector2D::Distance(CursorRootScreenPos, MousePos) };
	if(Dist > 130)
	{
		UE_LOG(LogCanyonPlacementRuler, Log, TEXT("Building placement denied bc the mouse moved to far from the building"));

		m_bInResnapRecovery = true;
		out_NewPos = TerrainHit.ImpactPoint;
		return false;
	}


//handle building penetrations after a snapping situation last frame
	auto *pHullComp{ Cast<UCanyonMeshCollisionComp>(pPlaceable->GetCanyonMeshCollision()) };
		
	if(m_bInResnapRecovery)
	{
		auto ComponentQueryParams{ FComponentQueryParams::DefaultComponentQueryParams };
		ComponentQueryParams.AddIgnoredActor(pPlaceable);
		//ComponentQueryParams.bTraceComplex = true;
		
		TArray<FOverlapResult> aOverlaps;
		pHullComp->GetWorld()->ComponentOverlapMulti
		(
			aOverlaps,
			pHullComp,
			pHullComp->GetComponentLocation(),
			pHullComp->GetComponentQuat(),
			ComponentQueryParams
		);
		
		if(aOverlaps.Num() != 0)
		{
			UE_LOG(LogCanyonPlacementRuler, Log, TEXT("Building placement denied because of non zero overlap count:\t%i"), aOverlaps.Num());

			out_NewPos = TerrainHit.ImpactPoint;
			return false;


		}
		m_bInResnapRecovery = false;		
	}

	//if target is unobstructed move to it
	{
		TArray<FOverlapResult> aOutOverlaps;

		auto ComponentQueryParams{ FComponentQueryParams::DefaultComponentQueryParams };
		ComponentQueryParams.AddIgnoredActor(pPlaceable);
		ComponentQueryParams.bTraceComplex = true;

		auto ObjectQueryParams{ FCollisionObjectQueryParams::DefaultObjectQueryParam };
		ObjectQueryParams.AddObjectTypesToQuery(GetCCPlaceables());
		ObjectQueryParams.AddObjectTypesToQuery(GetCCTerrain());

		pHullComp->GetWorld()->ComponentOverlapMulti
		(
			aOutOverlaps, 
			pHullComp, 
			TerrainHit.ImpactPoint + pHullComp->RelativeLocation, 
			pHullComp->GetComponentQuat(), 
			ComponentQueryParams, 
			ObjectQueryParams
		);

		if(aOutOverlaps.Num() == 0)
		{			
			out_NewPos = TerrainHit.ImpactPoint;
			return true;
		}

	}


	//sweep hits to find a pos at the nearest building
	TArray<FHitResult> aHits;
	{
		auto ComponentQueryParams{ FComponentQueryParams::DefaultComponentQueryParams };
		ComponentQueryParams.AddIgnoredActor(pPlaceable);
		ComponentQueryParams.bIgnoreTouches = true;
		//ComponentQueryParams.bTraceComplex = true;
		
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

	
	//the sweep didnt get any hits (maybe bc we didnt move the building).
	if(aHits.Num() == 0)
	{
				TArray<FOverlapResult> aOutOverlaps;

		auto ComponentQueryParams{ FComponentQueryParams::DefaultComponentQueryParams };
		ComponentQueryParams.AddIgnoredActor(pPlaceable);
		ComponentQueryParams.bTraceComplex = true;

		auto ObjectQueryParams{ FCollisionObjectQueryParams::DefaultObjectQueryParam };
		ObjectQueryParams.AddObjectTypesToQuery(GetCCPlaceables());
		ObjectQueryParams.AddObjectTypesToQuery(GetCCTerrain());

		pHullComp->GetWorld()->ComponentOverlapMulti
		(
			aOutOverlaps, 
			pHullComp, 
			TerrainHit.ImpactPoint + pHullComp->RelativeLocation, 
			pHullComp->GetComponentQuat(), 
			ComponentQueryParams, 
			ObjectQueryParams
		);

		if(aOutOverlaps.Num() == 0)
		{
		UE_LOG(LogTemp, Warning, TEXT("No sweeps overidden"));
			out_NewPos = TerrainHit.ImpactPoint;
			return true;
		}


		//if we are not penetrated
		UE_LOG(LogTemp, Warning, TEXT("No sweeps"));
		out_NewPos = m_LastPlaceablePosition;
		return m_bLastRet;		
	}

	int32 HitsPenOnStart{ 0 };
	for(auto &&Hit : aHits)
	{
		if(Hit.bStartPenetrating)
		{
			++HitsPenOnStart;
		}

	}

	if(HitsPenOnStart > 0)
	{
		UE_LOG(LogCanyonPlacementRuler, Log, TEXT("Building placement denied because of pen sweep hits"));
		out_NewPos = TerrainHit.ImpactPoint;
		return false;
	}

	//there were blocking hits so the closest determines the hull pos
	aHits.Sort([](const decltype(aHits)::ElementType &Left, const decltype(aHits)::ElementType &Right)
	{
		return Left.Time < Right.Time;


	});

	//new placeable pos from swept hit	
	FVector DepenetrationDisp{ aHits[0].ImpactNormal };

	out_NewPos = aHits[0].Location - pHullComp->RelativeLocation + DepenetrationDisp * .5;
	out_NewPos.Z = TerrainHit.ImpactPoint.Z;
	return true;


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
