// Fill out your copyright notice in the Description page of Project Settings.


#include "PlacableBase.h"

// Sets default values
APlacableBase::APlacableBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlacableBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlacableBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

