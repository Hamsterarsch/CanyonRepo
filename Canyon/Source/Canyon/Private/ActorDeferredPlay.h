#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorDeferredPlay.generated.h"

UCLASS(BlueprintType)
class AActorDeferredPlay : public AActor
{
	GENERATED_BODY()

public:
	static void BroadcastBeginGame();

	static void BroadcastEndGame();


protected:
	void NotifyBeginGame();

	virtual void BeginPlay() override;

	virtual void BeginGame() {}

	UFUNCTION(BlueprintImplementableEvent)
		void ReceiveBeginGame();


private:	
	static UPROPERTY()
		FSimpleMulticastDelegate s_eBeginGame;
	
	static bool s_bHasBegunGame;

	
};
