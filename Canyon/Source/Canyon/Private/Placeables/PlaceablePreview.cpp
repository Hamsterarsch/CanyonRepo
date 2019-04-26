// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaceablePreview.h"
#include "Engine/World.h"
#include "Misc/CanyonHelpers.h"
#include "Components/StaticMeshCanyonComp.h"
#include "Kismet/GameplayStatics.h"

APlaceablePreview::APlaceablePreview()
{
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));


}

APlaceablePreview *APlaceablePreview::SpawnPlaceablePreview
(
	UWorld *pWorld,	
	const FTransform &Transform, 
	TSubclassOf<APlaceableBase> PreviewedPlaceableClass
)
{
	auto *pPreviewedPlaceableClass{ PreviewedPlaceableClass.Get() };
	if(!pWorld | !pPreviewedPlaceableClass)
	{
		return nullptr;
	}
	
	auto *pPreview{ pWorld->SpawnActor<APlaceablePreview>(APlaceablePreview::StaticClass(), Transform) };
	pPreview->m_PreviewedClass = PreviewedPlaceableClass;

	//Mesh assignment
	auto apNodes{ GetScsDataNodesForType<UStaticMeshCanyonComp>(pPreviewedPlaceableClass) };
	for(auto &&pNode : apNodes)
	{
		auto *pMeshComp{ NewObject<UStaticMeshCanyonComp>(pPreview, UStaticMeshCanyonComp::StaticClass(), NAME_None, RF_NoFlags, pNode->ComponentTemplate) };
		pMeshComp->AttachToComponent(pPreview->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		pMeshComp->SetRelativeLocation(Cast<USceneComponent>(pNode->ComponentTemplate)->RelativeLocation);
		pMeshComp->RegisterComponent();
	}

	return pPreview;


}

UClass* APlaceablePreview::GetPreviewedClass() const
{
	return m_PreviewedClass.Get();


}
