#include "MeshInstancer.h"
#include "ActorAutoCollectedInstancedMeshes.h"
#include "Engine/StaticMesh.h"
#include "Components/InstancedStaticMeshComponent.h"

AMeshInstancer::AMeshInstancer() :
	m_InstigatorMap{},
	m_InstancingComponentsMap{}
{
}

void AMeshInstancer::AddMeshInstance
(
	const FVector &WSPosition, 
	const FQuat &WSRotation,
	const FVector &WSScale,
	AActorAutoCollectedInstancedMeshes *pInstigator, 
	UStaticMesh *pStaticMesh
)
{
	FInstancerComponentData *pInstancingComponentData{ nullptr };
	if(m_InstancingComponentsMap.Num() > 0)
	{
		pInstancingComponentData = m_InstancingComponentsMap.Find(pStaticMesh);
	}
	
	if(!pInstancingComponentData)
	{
		//only 999 instances of one mesh are supported
		pInstancingComponentData = &m_InstancingComponentsMap.Add(pStaticMesh, FInstancerComponentData{ NewObject<UInstancedStaticMeshComponent>(this), m_InstancingComponentsMap.Num() * 1000 });
		pInstancingComponentData->pComponent->SetStaticMesh(pStaticMesh);
		pInstancingComponentData->pComponent->SetupAttachment(GetRootComponent());
		pInstancingComponentData->pComponent->RegisterComponent();
	}

	//Map instigator
	auto InstanceIndex{ pInstancingComponentData->pComponent->AddInstanceWorldSpace(FTransform{WSRotation, WSPosition, WSScale}) };
	
	m_InstigatorMap.Add(pInstancingComponentData->HashBias + InstanceIndex, TWeakObjectPtr<class AActorAutoCollectedInstancedMeshes>{ pInstigator });
	
	
}

class AActorAutoCollectedInstancedMeshes *AMeshInstancer::NotifyInstigatorAboutClick(const UStaticMeshComponent *pMeshComponent, const int32 ItemNumber) const
{
	auto *pInstancingComponentData{ m_InstancingComponentsMap.Find(pMeshComponent->GetStaticMesh()) };
	if(!pInstancingComponentData)
	{
		return nullptr;


	}

	auto *pWeakInstigatorPtr{ m_InstigatorMap.Find(pInstancingComponentData->HashBias + ItemNumber) };
	if(!pWeakInstigatorPtr)
	{
		return nullptr;


	}

	if(auto *pInstigator{ pWeakInstigatorPtr->Get() })
	{
		pInstigator->NotifyOnMeshInstanceClicked();
		return pInstigator;


	}

	return nullptr;
	

}
