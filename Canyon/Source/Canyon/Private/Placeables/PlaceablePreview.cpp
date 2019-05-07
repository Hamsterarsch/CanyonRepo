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
#include "Misc/CanyonLogs.h"
#include "Components/CanyonMeshCollisionComp.h"
#include "ConstructorHelpers.h"


APlaceablePreview::APlaceablePreview() :
	m_InfluenceCurrentGain{ 0 }
{
	//component creation
	m_pInfluenceSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DependencySphere"));
	m_pInfluenceSphere->SetupAttachment(GetRootComponent());
	//collision setup
	m_pInfluenceSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	m_pInfluenceSphere->SetCollisionResponseToChannel(GetCCPlaceables(), ECollisionResponse::ECR_Overlap);
	//m_pInfluenceSphere->OnComponentBeginOverlap.AddDynamic(this, &APlaceablePreview::BeginOverlapDependencyRadius);
	//m_pInfluenceSphere->OnComponentEndOverlap.AddDynamic(this, &APlaceablePreview::EndOverlapDependencyRadius);

	m_pRadiusVisComp = CreateDefaultSubobject<URadiusVisComp>(TEXT("InfluenceVis"));
	m_pRadiusVisComp->SetupAttachment(GetRootComponent());

	OnActorBeginOverlap.AddDynamic(this, &APlaceablePreview::ActorBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &APlaceablePreview::ActorEndOverlap);

	ConstructorHelpers::FObjectFinder<UMaterialInterface> PlaceableMaterialFinder{ TEXT("/Game/Placeables/Placeable_M") };
	ConstructorHelpers::FObjectFinder<UMaterialInterface> UnplaceableMaterialFinder{ TEXT("/Game/Placeables/Unplaceable_M") };

	m_pMaterialPlaceable = PlaceableMaterialFinder.Object;
	m_pMaterialUnplaceable = UnplaceableMaterialFinder.Object;


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

		auto *pAsSceneComp{ Cast<USceneComponent>(pNode->ComponentTemplate) };
		pMeshComp->SetRelativeLocation(pAsSceneComp->RelativeLocation);
		pMeshComp->SetRelativeRotation(pAsSceneComp->RelativeRotation);
		pMeshComp->SetRelativeScale3D(pAsSceneComp->RelativeScale3D);
		//pMeshComp->SetComponentToWorld(Cast<USceneComponent>(pNode->ComponentTemplate)->GetComponentTransform());
		pMeshComp->RegisterComponent();

	}

	//apNodes = GetScsDataNodesForType<UCanyonMeshCollisionComp>(pPreviewedPlaceableClass);
	
	auto *pCdo{ Cast<APlaceableBase>(pPreviewedPlaceableClass->ClassDefaultObject) };
	pPreview->m_pMeshCollisionComp = NewObject<UCanyonMeshCollisionComp>(pPreview, pCdo->GetCanyonMeshCollision()->GetClass(), NAME_None, RF_NoFlags);
	pPreview->m_pMeshCollisionComp->SetupAttachment(pPreview->GetRootComponent());
	pPreview->m_pMeshCollisionComp->RegisterComponent();
	//pPreview->m_pMeshCollisionComp->
	
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

void APlaceablePreview::NotifyPlaceable()
{
	UE_LOG(LogCanyonPlacement, Log, TEXT("Notfiy building placeable."));
	SetMaterialForAllMeshes(m_pMaterialPlaceable);


}

void APlaceablePreview::NotifyUnplaceable()
{
	UE_LOG(LogCanyonPlacement, Log, TEXT("Notfiy building unplaceable."));
	SetMaterialForAllMeshes(m_pMaterialUnplaceable);


}


//Private----------------------------

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

void APlaceablePreview::ActorBeginOverlap(AActor *pOverlappedActor, AActor *pOtherActor)
{
	auto *pOtherPlaceable{ Cast<APlaceableBase>(pOtherActor) };
	if(!pOtherPlaceable || pOtherPlaceable == this)
	{
		return;
	}

	m_InfluenceCurrentGain += pOtherPlaceable->BeginInfluenceVisFor(m_PreviewedClass);
	SetInfluenceDisplayed(m_InfluenceCurrentGain);


}
 
void APlaceablePreview::ActorEndOverlap(AActor *pOverlappedActor, AActor *pOtherActor)
{
	auto *pOtherPlaceable{ Cast<APlaceableBase>(pOtherActor) };
	if (!pOtherPlaceable || pOtherPlaceable == this)
	{
		return;
	}

	m_InfluenceCurrentGain -= pOtherPlaceable->EndInfluenceVis();
	SetInfluenceDisplayed(m_InfluenceCurrentGain);


}

void APlaceablePreview::SetInfluenceRadius(float Radius)
{
	m_pInfluenceSphere->SetSphereRadius(Radius);
	m_pRadiusVisComp->SetRadius(Radius);


}

void APlaceablePreview::SetMaterialForAllMeshes(UMaterialInterface* pMaterial)
{
	for (auto *pComponent : GetComponentsByClass(UStaticMeshComponent::StaticClass()))
	{
		auto *pAsMeshComp{ Cast<UStaticMeshComponent>(pComponent) };

		const auto CountUsedMaterials{ pAsMeshComp->GetNumMaterials() };
		for (int32 MaterialIndex{ 0 }; MaterialIndex < CountUsedMaterials; ++MaterialIndex)
		{
			if(pAsMeshComp->IsA<URadiusVisComp>())
			{
				continue;
			}

			pAsMeshComp->SetMaterial(MaterialIndex, pMaterial);

		}

	}

}
