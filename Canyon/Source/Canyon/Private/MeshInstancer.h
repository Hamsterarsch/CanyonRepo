#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeshInstancer.generated.h"

USTRUCT()
struct FInstancerComponentData
{
	GENERATED_BODY()

public:
	FInstancerComponentData() = default;

	FInstancerComponentData(class UInstancedStaticMeshComponent *_pComponent, int32 _HashBias) :
		pComponent(_pComponent),
		HashBias(_HashBias)
	{		
	}

	UPROPERTY()
		class UInstancedStaticMeshComponent *pComponent;
	
	int32 HashBias;


};

UCLASS()
class AMeshInstancer : public AActor
{
	GENERATED_BODY()

public:
	AMeshInstancer();

	void AddMeshInstance
	(
		const FVector &WSPosition, 
		const FQuat &WSRotation,
		const FVector &WSScale,
		class AActorAutoCollectedInstancedMeshes *pInstigator, 
		UStaticMesh *pStaticMesh
	);

	class AActorAutoCollectedInstancedMeshes *NotifyInstigatorAboutClick(const UStaticMeshComponent *pMeshComponent, int32 ItemNumber) const;

	
private:
	

	UPROPERTY()
		TMap<int32, TWeakObjectPtr<class AActorAutoCollectedInstancedMeshes>> m_InstigatorMap;

	UPROPERTY()
		TMap<const UStaticMesh *, FInstancerComponentData> m_InstancingComponentsMap;


};
