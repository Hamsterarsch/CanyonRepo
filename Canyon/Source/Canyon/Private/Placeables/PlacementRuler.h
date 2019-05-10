#pragma once
#include "Placeables/PlaceableBase.h"


class CPlacementRuler
{
public:
	CPlacementRuler();

	bool TryEnforceBuildingRules(const FHitResult &ForHit, APlaceableBase *pPlaceable, FVector &OutNewPos);

	//input hit result and preview building
	//sets the building to the impact point of the hit or tries to slide towards it 
	//also enforces surface types and other constraints
	FVector ComputeTerrainDepenetration(const FHitResult &ForHit, const APlaceableBase *pPlaceable);

	//reqs
	//max accepted normal angle for placement


};

