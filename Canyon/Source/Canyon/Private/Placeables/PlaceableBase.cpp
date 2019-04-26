// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaceableBase.h"
#include "Misc/CanyonLogs.h"
#include "Components/StaticMeshCanyonComp.h"

// Sets default values
APlaceableBase::APlaceableBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlaceableBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlaceableBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float APlaceableBase::GetPlaceableNormalZMin() const
{
	return FMath::Cos(FMath::DegreesToRadians(m_PlaceableMaxSlopeAngle));


}

float APlaceableBase::GetPlaceableNormalZMax() const
{
	return FMath::Cos(FMath::DegreesToRadians(m_PlaceableMinSlopeAngle));


}

void APlaceableBase::NotifyPlaceable()
{
	UE_LOG(LogCanyonPlacement, Log, TEXT("Notfiy building placeable."));
}

void APlaceableBase::NotifyUnplaceable()
{
	UE_LOG(LogCanyonPlacement, Log, TEXT("Notfiy building unplaceable."));
}

TArray<UActorComponent *> APlaceableBase::GetPlaceableMeshComps()
{
	return GetComponentsByClass(UStaticMeshCanyonComp::StaticClass());


}

