#pragma once
#include "Engine/ActorChannel.h"

constexpr ECollisionChannel GetCCTerrain()
{
	static constexpr ECollisionChannel Channel{ ECollisionChannel::ECC_GameTraceChannel1 };
	return Channel;

	
}

constexpr ECollisionChannel GetCCPlaceables()
{
	static constexpr ECollisionChannel Channel{ ECollisionChannel::ECC_GameTraceChannel2 };
	return Channel;


}

inline FName GetCollisionPresetPlaceables()
{
	static const FName Name{ "Placeables" };
	return Name;


}