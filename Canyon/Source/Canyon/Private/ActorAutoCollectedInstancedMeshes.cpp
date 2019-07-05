#include "ActorAutoCollectedInstancedMeshes.h"
#include "Components/StaticInstancedMeshCanyonComp.h"
#include "MeshInstancer.h"
#include "Misc/CanyonGM.h"

void AActorAutoCollectedInstancedMeshes::NotifyOnMeshInstanceClicked()
{
	OnMeshInstanceClicked();
	ReceiveOnMeshInstanceClicked();


}

void AActorAutoCollectedInstancedMeshes::BeginGame()
{
	Super::BeginGame();

	if(!m_bCollectInstancedMeshes)
	{
		return;
		

	}

	auto *pMeshInstancer{ Cast<ACanyonGM>(GetWorld()->GetAuthGameMode())->GetMeshInstancer() };
	auto apComponents{ GetComponents() };

	TArray<UActorComponent *> apComponentsToRemove{};

	for(auto &&pComponent : apComponents)
	{
		if(auto *pAsMeshComp{ Cast<UStaticInstancedMeshCanyonComp>(pComponent) })
		{
			//add meshes to instancer here
			pMeshInstancer->AddMeshInstance
			(
				pAsMeshComp->GetComponentLocation(), 
				pAsMeshComp->GetComponentQuat(), 
				pAsMeshComp->GetComponentScale(), 
				this, 
				pAsMeshComp->GetStaticMesh()
			);

			apComponentsToRemove.Add(pAsMeshComp);
		}

	}

	for(auto &&pCompToDelete : apComponentsToRemove)
	{
		pCompToDelete->DestroyComponent();

	}


}
