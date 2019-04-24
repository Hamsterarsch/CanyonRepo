#pragma once
#include "Placeables/PlaceableBase.h"


class CPlacementRuler
{
public:
	bool TryEnforceBuildingRules(const FHitResult &ForHit, APlaceableBase *pPlaceable);

	//input hit result and preview building
	//output position where the building is placable 
	//(and additional data regarding support structures underneath, etc)
	FVector ComputeTerrainDepenetration(const FHitResult &ForHit, const APlaceableBase *pPlaceable);

	//reqs
	//max accepted normal angle for placement


};

