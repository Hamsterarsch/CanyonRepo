// Fill out your copyright notice in the Description page of Project Settings.

#include "RadiusVisComp.h"
#include "ConstructorHelpers.h"

URadiusVisComp::URadiusVisComp()
{
	ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshFinder{ TEXT("/Engine/BasicShapes/Sphere") };
	ConstructorHelpers::FObjectFinder<UMaterialInterface> VisMaterial{ TEXT("/Game/Comp/RadiusVis_M") };

	//static resolve is fine here
	SetStaticMesh(SphereMeshFinder.Object);
	SetMaterial(0, VisMaterial.Object);
	SetCollisionEnabled(ECollisionEnabled::NoCollision);


}

void URadiusVisComp::SetRadius(float Radius)
{
	auto Scale{ Radius / 50 };
	SetWorldScale3D({Scale, Scale, Scale});


}
