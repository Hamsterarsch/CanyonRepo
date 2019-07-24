#pragma once
#include "Placeables/PlaceableBase.h"


class CPlacementRuler
{
public:
	CPlacementRuler();

	bool HandleBuildingRules(APlaceableBase *pPlaceable, FVector &out_NewPos);

	//input hit result and preview building
	//sets the building to the impact point of the hit or tries to slide towards it 
	//also enforces surface types and other constraints
	FVector ComputeTerrainDepenetration(const FHitResult &ForHit, const APlaceableBase *pPlaceable);

	//reqs
	//max accepted normal angle for placement

private:
	bool HandleBuildingRulesInternal(APlaceableBase *pPlaceable, FVector &out_NewPos);

	void HandlePenetratingHits(APlaceableBase *pPlaceable) const;

	bool AreAllCornersGrounded( const FVector &OutPosition, UCanyonMeshCollisionComp *pHull) const;

	bool m_bInResnapRecovery;
	bool m_bLastRet{ false };
	FVector m_LastTerrainTracePos;
	FVector m_LastPlaceablePosition;
	FVector m_LastPlaceablePositionValid;
	float m_LastTerrainTraceZ;


};

bool TraceForTerrainUnderCursor(FHitResult &OutHit, const UWorld *pWorld);

bool IsNewHullPositionValid(const FVector &HullPos, class UCanyonMeshCollisionComp *pHullComp, float Threshold, bool bUseComplex);