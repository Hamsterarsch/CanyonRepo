#pragma once

#include "CoreMinimal.h"
#include "ActorDeferredPlay.h"
#include "ActorAutoCollectedInstancedMeshes.generated.h"

UCLASS(BlueprintType)
class AActorAutoCollectedInstancedMeshes : public AActorDeferredPlay
{
	GENERATED_BODY()

public:
	void NotifyOnMeshInstanceClicked();

	
protected:
	virtual void BeginGame() override;
	
	virtual void OnMeshInstanceClicked() {};


	UFUNCTION(BlueprintImplementableEvent)
		void ReceiveOnMeshInstanceClicked();

	UPROPERTY(EditDefaultsOnly)
		bool m_bCollectInstancedMeshes{ true };
	

};