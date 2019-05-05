// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaceablePreview.h"
#include "Engine/World.h"
#include "Misc/CanyonHelpers.h"
#include "Components/StaticMeshCanyonComp.h"
#include "Components/SphereComponent.h"
#include "Components/RadiusVisComp.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/CollisionChannels.h"


APlaceablePreview::APlaceablePreview() :
	m_InfluenceCurrentGain{ 0 }
{
	//component creation
	m_pInfluenceSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DependencySphere"));
	m_pInfluenceSphere->SetupAttachment(GetRootComponent());
	//collision setup
	m_pInfluenceSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	m_pInfluenceSphere->SetCollisionResponseToChannel(GetCCPlaceables(), ECollisionResponse::ECR_Overlap);
	m_pInfluenceSphere->OnComponentBeginOverlap.AddDynamic(this, &APlaceablePreview::BeginOverlapDependencyRadius);
	m_pInfluenceSphere->OnComponentEndOverlap.AddDynamic(this, &APlaceablePreview::EndOverlapDependencyRadius);

	m_pRadiusVisComp = CreateDefaultSubobject<URadiusVisComp>(TEXT("InfluenceVis"));
	m_pRadiusVisComp->SetupAttachment(GetRootComponent());


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
	
	//spawn placeable preview
	auto *pPreview{ pWorld->SpawnActorDeferred<APlaceablePreview>(APlaceablePreview::StaticClass(), Transform) };
	pPreview->m_PreviewedClass = PreviewedPlaceableClass;

	//duplicate mesh components from previewed class
	auto apNodes{ GetScsDataNodesForType<UStaticMeshCanyonComp>(pPreviewedPlaceableClass) };
	for(auto &&pNode : apNodes)
	{
		auto *pMeshComp{ NewObject<UStaticMeshCanyonComp>(pPreview, UStaticMeshCanyonComp::StaticClass(), NAME_None, RF_NoFlags, pNode->ComponentTemplate) };
		pMeshComp->AttachToComponent(pPreview->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		pMeshComp->SetRelativeLocation(Cast<USceneComponent>(pNode->ComponentTemplate)->RelativeLocation);
		pMeshComp->RegisterComponent();
	}

	auto *pCdo{ Cast<APlaceableBase>(pPreviewedPlaceableClass->ClassDefaultObject) };
	pPreview->SetInfluenceRadius(pCdo->GetInfluenceRadius());
	pPreview->InitInfluenceDisplayWidget(pCdo->GetInfluenceWidgetClass());
	pPreview->SetInfluenceDisplayed(0);

	UGameplayStatics::FinishSpawningActor(pPreview, Transform);
	return pPreview;


}

UClass *APlaceablePreview::GetPreviewedClass() const
{
	return m_PreviewedClass.Get();


}

void APlaceablePreview::BeginOverlapDependencyRadius
(
	UPrimitiveComponent *pOverlappedComponent,
	AActor *pOtherActor,
	UPrimitiveComponent *pOtherComp,
	int32 OtherBodyIndex, 
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	//only handle subclasses of a placeable
	auto *pOverlappedPlaceable{ Cast<APlaceableBase>(pOtherActor) };
	if(!pOverlappedPlaceable || pOverlappedPlaceable == this)
	{
		return;
	}

	m_InfluenceCurrentGain += pOverlappedPlaceable->BeginInfluenceVisFor(m_PreviewedClass);
	SetInfluenceDisplayed(m_InfluenceCurrentGain);


}

void APlaceablePreview::EndOverlapDependencyRadius
(
	UPrimitiveComponent *pOverlappedComponent, 
	AActor *pOtherActor,
	UPrimitiveComponent *pOtherComp,
	int32 OtherBodyIndex
)
{
	//only handle subclasses of a placeable
	auto *pOverlappedPlaceable{ Cast<APlaceableBase>(pOtherActor) };
	if (!pOverlappedPlaceable || pOverlappedPlaceable == this)
	{
		return;
	}

	m_InfluenceCurrentGain -= pOverlappedPlaceable->EndInfluenceVis();
	SetInfluenceDisplayed(m_InfluenceCurrentGain);


}

void APlaceablePreview::SetInfluenceRadius(float Radius)
{
	m_pInfluenceSphere->SetSphereRadius(Radius);
	m_pRadiusVisComp->SetRadius(Radius);


}
