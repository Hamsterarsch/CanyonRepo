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
#include "Engine/StaticMesh.h"

CPlacementRuler::CPlacementRuler() :
	m_bInResnapRecovery{ false },
	m_LastTerrainTracePos{ EForceInit::ForceInitToZero },
	m_LastPlaceablePosition{ EForceInit::ForceInitToZero },
	m_LastPlaceablePositionValid{ EForceInit::ForceInitToZero }
{
}

bool CPlacementRuler::HandleBuildingRules(APlaceableBase* pPlaceable, FVector& out_NewPos)
{
	m_bLastRet = HandleBuildingRulesInternal(pPlaceable, m_LastPlaceablePosition);
	out_NewPos = m_LastPlaceablePosition;

	if(m_bLastRet)
	{
		m_LastPlaceablePositionValid = out_NewPos;
	}

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
	const bool bUseComplex{ false };

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
	/*
					//Normal constraint
					constexpr float NormalTolerance{ 10e-6 };
					auto ImpactNormalZ{ Hit.ImpactNormal.GetUnsafeNormal().Z };
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
	*/


	auto *pHullComp{ Cast<UCanyonMeshCollisionComp>(pPlaceable->GetCanyonMeshCollision()) };
	pHullComp->SetUseCCD(true);

	//collider corner based queries
	{
		constexpr float CornerTraceDepth{ 2 };
		auto *pVertexBuffer{ &pHullComp->GetStaticMesh()->RenderData->LODResources[0].VertexBuffers.PositionVertexBuffer };
		bool bIsPlaceable{ true };
		if(pVertexBuffer)
		{
			int32 Counter{ 0 };
			for(uint32 VertIndex{ 0 }; VertIndex < pVertexBuffer->GetNumVertices(); ++VertIndex)
			{
				const auto &VertexPos{ pVertexBuffer->VertexPosition(VertIndex) };
				if( FMath::IsNearlyZero(VertexPos.Z, .5f) )
				{
					++Counter;
					auto Transformed{ pHullComp->GetComponentTransform().TransformPosition(VertexPos) };

				//Edge check (all relevant points have to be close to the ground/ not floating
					FHitResult Hit;

					if
					(
						!pHullComp->GetWorld()->LineTraceSingleByChannel
						(
							Hit, 
							Transformed,
							Transformed - FVector{0,0,CornerTraceDepth},
							GetCCTerrain()
						)
					)
					{
						UE_LOG(LogTemp, Log, TEXT("Discard bc of edge"));
						out_NewPos = TerrainHit.ImpactPoint;
						return false;


					}

				//Surface type constraint
					if (auto *pPhyMat{ Hit.PhysMaterial.Get() })
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
				}

			}
			UE_LOG(LogTemp, Log, TEXT("Relevant vert count: %i"), Counter);
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
		
	if(m_bInResnapRecovery)
	{
		auto ComponentQueryParams{ FComponentQueryParams::DefaultComponentQueryParams };
		ComponentQueryParams.AddIgnoredActor(pPlaceable);
		ComponentQueryParams.bTraceComplex = bUseComplex;
		
		TArray<FOverlapResult> aOverlaps;
		pHullComp->GetWorld()->ComponentOverlapMulti
		(
			aOverlaps,
			pHullComp,
			TerrainHit.ImpactPoint + pHullComp->RelativeLocation,
			pHullComp->GetComponentQuat(),
			ComponentQueryParams
		);
		
		if(aOverlaps.Num() != 0)
		{
			UE_LOG(LogCanyonPlacementRuler, Log, TEXT("Building placement denied because of non zero overlap count:\t%i"), aOverlaps.Num());

			if(IsNewHullPositionValid(TerrainHit.ImpactPoint + pHullComp->RelativeLocation, pHullComp, 0.5, bUseComplex))
			{
				out_NewPos = TerrainHit.ImpactPoint;
				return true;

			}


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
		ComponentQueryParams.bTraceComplex = bUseComplex;

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
				
		if(IsNewHullPositionValid(TerrainHit.ImpactPoint + pHullComp->RelativeLocation, pHullComp, 0.5, bUseComplex))
		{
			out_NewPos = TerrainHit.ImpactPoint;
			return true;

		}

	}

	

	FVector SweepStart{ pPlaceable->GetActorLocation() - pHullComp->RelativeLocation };
	FVector2D MovementDisp{ TerrainHit.ImpactPoint - SweepStart };
	FVector SweepEnd{ SweepStart + FVector{ MovementDisp.X, MovementDisp.Y, 0} };

	if(MovementDisp.IsZero())
	{
		out_NewPos = m_LastPlaceablePosition;
		return m_bLastRet;
	}

	auto ComponentQueryParams{ FComponentQueryParams::DefaultComponentQueryParams };
	ComponentQueryParams.AddIgnoredActor(pPlaceable);
	ComponentQueryParams.bIgnoreTouches = true;
	ComponentQueryParams.bTraceComplex = bUseComplex;
	FVector NewHullPos{ m_LastPlaceablePosition };
	FVector LastHitPos{ m_LastPlaceablePosition };
	FVector2D LastMovementDisp{ MovementDisp };

	FVector AlternateNonMovedHitPos{ 0, 0, 0 };
	int32 IterationCount{ 0 };
	while (IterationCount < 16)
	{
		//sweep hits to find hits
		TArray<FHitResult> aHits;
				
		MovementDisp = FVector2D{ TerrainHit.ImpactPoint - SweepStart };
		SweepEnd = SweepStart + FVector{ MovementDisp.X, MovementDisp.Y, 0 };

		pHullComp->GetWorld()->ComponentSweepMulti
		(
			aHits,
			pHullComp,
			SweepStart,
			SweepEnd,
			pHullComp->GetComponentQuat(),
			ComponentQueryParams
		);

		auto NumHits{ aHits.Num() };
		if(aHits.Num() == 0)
		{			
			NewHullPos = SweepEnd;
			break;
		}

		aHits.Sort([](const decltype(aHits)::ElementType &Left, const decltype(aHits)::ElementType &Right)
		{
			if(Left.Time == Right.Time)
			{
				return Left.PenetrationDepth > Right.PenetrationDepth;
			}
			return Left.Time < Right.Time;


		});
	
		//always depen the first hit location, otherwise the displacement sweep will always get a pen hit
		//auto DepenCoeff{ aHits[0].PenetrationDepth >= 0.005f ? aHits[0].PenetrationDepth : 0.0001f };
		
		//aHits[0].Location += aHits[0].ImpactNormal * DepenCoeff;			
		
		
		
		/*if (FMath::IsNearlyZero(Perpendicular.X, 0.005f))
		{
			Perpendicular.X = 0;
		}

		if (FMath::IsNearlyZero(Perpendicular.Y, 0.005f))
		{
			Perpendicular.Y = 0;
		}*/
		
		auto DotMovementImpactNormal{ FVector2D::DotProduct(MovementDisp.GetSafeNormal(), FVector2D{aHits[0].ImpactNormal}) };
		for(int32 HitIndex{ 1 }; DotMovementImpactNormal > 0 && HitIndex < aHits.Num(); ++HitIndex)
		{
			DotMovementImpactNormal = FVector2D::DotProduct(MovementDisp.GetSafeNormal(), FVector2D{aHits[HitIndex].ImpactNormal});
			if(DotMovementImpactNormal < 0)
			{
				aHits[0] = aHits[HitIndex];
			}
			

		}

		auto FirstHitLocation{ SweepStart };
		if(DotMovementImpactNormal > 0)
		{
			NewHullPos = FirstHitLocation + FVector{ MovementDisp.X, MovementDisp.Y, 0 };
			break;
		}

		FVector2D Perpendicular{ -aHits[0].ImpactNormal.Y, aHits[0].ImpactNormal.X };
		auto Dot{ FVector2D::DotProduct(MovementDisp, Perpendicular) };
		FirstHitLocation = aHits[0].Location + aHits[0].ImpactNormal * aHits[0].PenetrationDepth;

		if(IterationCount == 0)
		{
			AlternateNonMovedHitPos = FirstHitLocation;			
		}

		if (FMath::IsNearlyZero(FVector2D::DotProduct(MovementDisp.GetSafeNormal(), Perpendicular), .05f))
		{			
			if(aHits.Num() == 1)
			{
				NewHullPos = FirstHitLocation;
				break;
			}

			/*
			//smaller check
			auto Scale{ FVector{ 1, 1, 1 } - aHits[0].ImpactNormal.GetAbs() * 0.0625 };
			Scale.Z = 1;

			auto OldScale{ pHullComp->RelativeScale3D };
			pHullComp->SetWorldScale3D(Scale);

			TArray<FHitResult> aDisplacementHits{};
			pHullComp->GetWorld()->ComponentSweepMulti
			(
				aDisplacementHits,
				pHullComp,
				SweepStart,
				SweepEnd,
				pHullComp->GetComponentQuat(),
				ComponentQueryParams
			);

			pHullComp->SetRelativeScale3D(OldScale);

			if(aDisplacementHits.Num() == 0)
			{
				NewHullPos = FirstHitLocation;
				break;
			}

			aDisplacementHits.Sort([](const decltype(aHits)::ElementType &Left, const decltype(aHits)::ElementType &Right)
			{
				return Left.Time < Right.Time;


			});

			

			NewHullPos = aDisplacementHits[0].Location + aDisplacementHits[0].ImpactNormal * aDisplacementHits[0].PenetrationDepth;
			break;
			*/

		}
		else if (Dot > 0 && DotMovementImpactNormal <= 0)
		{
			MovementDisp = Perpendicular * Dot;
		}
		else if(DotMovementImpactNormal <= 0)  //Dot < 0
		{
			MovementDisp = -Perpendicular * -Dot;
		}

		

		TArray<FHitResult> aDisplacementHits;
		pHullComp->GetWorld()->ComponentSweepMulti
		(
			aDisplacementHits,
			pHullComp,
			FirstHitLocation,
			FirstHitLocation + FVector{ MovementDisp.X, MovementDisp.Y, 0 },
			pHullComp->GetComponentQuat(),
			ComponentQueryParams
		);

		aDisplacementHits.Sort([](const decltype(aHits)::ElementType &Left, const decltype(aHits)::ElementType &Right)
		{
			return Left.Time < Right.Time;


		});

		aDisplacementHits.RemoveAll([HandledNormal = aHits[0].ImpactNormal, MovementDir = MovementDisp.GetSafeNormal()](const decltype(aHits)::ElementType &Elem)
		{
			return (Elem.ImpactNormal - HandledNormal).IsNearlyZero(0.05f) || Elem.Distance <= 0.01f || FVector2D::DotProduct(FVector2D{Elem.ImpactNormal}, MovementDir) > 0;


		});

		if(aDisplacementHits.Num() > 0)
		{
			aDisplacementHits.Sort([](const decltype(aHits)::ElementType &Left, const decltype(aHits)::ElementType &Right)
			{
				return Left.Time < Right.Time;


			});
			
			MovementDisp *= aDisplacementHits[0].Time;
			//NewHullPos = aDisplacementHits[0].Location;
			//break;			

			if(FMath::IsNearlyZero(FVector::DotProduct(aDisplacementHits[0].ImpactNormal, aHits[0].ImpactNormal), .05f))
			{
				NewHullPos = FirstHitLocation + FVector{ MovementDisp.X, MovementDisp.Y, 0 };
				break;
			}

			
			if(MovementDisp.IsNearlyZero(0.0001))
			{
				NewHullPos = FirstHitLocation;
				break;
			}
			
			

		}
		else
		{
			NewHullPos = FirstHitLocation + FVector{ MovementDisp.X, MovementDisp.Y, 0 };
			break;

		}

		SweepStart = FirstHitLocation + FVector{ MovementDisp.X, MovementDisp.Y, 0 };
		LastHitPos = FirstHitLocation;
		LastMovementDisp = MovementDisp;

		++IterationCount;
		if(IterationCount == 12)
		{
			UE_LOG(LogCanyonPlacement, Warning, TEXT("Building movement ran out of iterations"));
		}

	}


	out_NewPos = NewHullPos + pHullComp->RelativeLocation;
	auto Result{ IsNewHullPositionValid(out_NewPos, pHullComp, 0.5, bUseComplex) };

	if(!Result)
	{
		out_NewPos =   AlternateNonMovedHitPos;
		return true;
	}

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


struct OverlapKeyFunct : public BaseKeyFuncs<FOverlapResult, FOverlapResult, true>
{
public:
	typedef TCallTraits<FOverlapResult>::ParamType KeyInitType;
	typedef TCallTraits<FOverlapResult>::ParamType ElementInitType;

	/**
	 * @return The key used to index the given element.
	 */
	static FORCEINLINE KeyInitType GetSetKey(ElementInitType Element)
	{
		return Element;
	}

	/**
	 * @return True if the keys match.
	 */
	static FORCEINLINE bool Matches(KeyInitType A,KeyInitType B)
	{
		return A.Actor == B.Actor;
	}

	/** Calculates a hash index for a key. */
	static FORCEINLINE uint32 GetKeyHash(KeyInitType Key)
	{
		return GetTypeHash(Key.Actor);
	}

};

bool IsNewHullPositionValid(const FVector& HullPos, UCanyonMeshCollisionComp* pHullComp, const float Threshold, const bool bUseComplex)
{
	auto ComponentQueryParams{ FComponentQueryParams::DefaultComponentQueryParams };
	//ComponentQueryParams.bIgnoreTouches = true;
	ComponentQueryParams.bTraceComplex = bUseComplex;

	auto ObjectQueryParams{ FCollisionObjectQueryParams::DefaultObjectQueryParam };
	ObjectQueryParams.AddObjectTypesToQuery(GetCCPlaceables());

	TArray<FOverlapResult> aAllFoundOverlaps{};

	const TArray<FVector> aDisplacements{ { Threshold, Threshold, 0 }, { -Threshold, Threshold, 0 }, { Threshold, -Threshold, 0 }, { -Threshold, -Threshold, 0 } };
	TSet<FOverlapResult, OverlapKeyFunct> Set0{};
	TSet<FOverlapResult, OverlapKeyFunct> Set1{};
	TSet<FOverlapResult, OverlapKeyFunct> Set2{};
	TSet<FOverlapResult, OverlapKeyFunct> Set3{};

	for(int32 DisplacementIndex{ 0 }; DisplacementIndex < aDisplacements.Num(); ++DisplacementIndex)
	{
		TArray<FOverlapResult> aFoundOverlaps;

		pHullComp->GetWorld()->ComponentOverlapMultiByChannel
		(
			aFoundOverlaps,
			pHullComp,
			HullPos + aDisplacements[DisplacementIndex],
			pHullComp->GetComponentQuat(),
			GetCCPlaceables(),
			ComponentQueryParams,
			ObjectQueryParams
		);

		DrawDebugPoint(pHullComp->GetWorld(), HullPos + aDisplacements[DisplacementIndex], 10, FColor::White, false, 5, 2);

		switch(DisplacementIndex)
		{
		case 0:
			Set0.Append(std::move(aFoundOverlaps));		
			break;			
		case 1:
			Set1.Append(std::move(aFoundOverlaps));		
			break;			
		case 2:
			Set2.Append(std::move(aFoundOverlaps));		
			break;			
		case 3:
			Set3.Append(std::move(aFoundOverlaps));		
			break;
		default:
			check(false);
		}

	}

	const auto Intersection0{ Set0.Intersect(Set1) };
	const auto Intersection1{ Set2.Intersect(Set3) };
	const auto FinalIntersection{ Intersection0.Intersect(Intersection1) };

	auto aAsArray{ FinalIntersection.Array() };
	auto oldNum{ aAsArray.Num() };
	aAsArray.RemoveAll([pPlaceable = pHullComp->GetOwner()](const FOverlapResult &Elem)
	{
		return !Elem.Actor.IsValid() || Elem.Actor.Get() == pPlaceable;

	});
	auto newNum{ aAsArray.Num() };


	if(FinalIntersection.Num() > 0)
	{
		int32 d{ 0 };
		return false;
	}
	return true;


}
