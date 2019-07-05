#include "ActorDeferredPlay.h"

FSimpleMulticastDelegate AActorDeferredPlay::s_eBeginGame;

bool AActorDeferredPlay::s_bHasBegunPlay;


void AActorDeferredPlay::BroadcastBeginGame()
{
	if(!s_bHasBegunPlay)
	{
		s_eBeginGame.Broadcast();
	}
	s_bHasBegunPlay = true;


}

void AActorDeferredPlay::NotifyBeginGame()
{
	BeginGame();
	ReceiveBeginGame();


}

void AActorDeferredPlay::BeginPlay()
{
	Super::BeginPlay();

	if(s_bHasBegunPlay)
	{
		BeginGame();
		return;


	}
	
	s_eBeginGame.AddUObject(this, &AActorDeferredPlay::NotifyBeginGame);


}
