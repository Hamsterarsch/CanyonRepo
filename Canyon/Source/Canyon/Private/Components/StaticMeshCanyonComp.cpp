// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticMeshCanyonComp.h"
#include "Misc/CollisionChannels.h"

UStaticMeshCanyonComp::UStaticMeshCanyonComp()
{	

}

void UStaticMeshCanyonComp::BeginPlay()
{
	SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);


}
