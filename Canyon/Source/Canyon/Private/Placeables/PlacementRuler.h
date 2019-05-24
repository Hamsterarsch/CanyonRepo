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
	void HandlePenetratingHits(APlaceableBase *pPlaceable) const;

	bool m_bInResnapRecovery;
	FVector m_LastHitPosition;
	FVector m_LastPlaceablePosition;


};

bool TraceForTerrainUnderCursor(FHitResult &OutHit, const UWorld *pWorld);

