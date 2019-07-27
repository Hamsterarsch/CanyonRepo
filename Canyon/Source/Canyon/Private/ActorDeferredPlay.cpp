#include "ActorDeferredPlay.h"

FSimpleMulticastDelegate AActorDeferredPlay::s_eBeginGame;

bool AActorDeferredPlay::s_bHasBegunGame;


void AActorDeferredPlay::BroadcastBeginGame()
{
	if(!s_bHasBegunGame)
	{
		s_eBeginGame.Broadcast();
	}
	s_bHasBegunGame = true;


}

void AActorDeferredPlay::BroadcastEndGame()
{
	s_bHasBegunGame = false;
	s_eBeginGame.Clear();


}

void AActorDeferredPlay::NotifyBeginGame()
{
	BeginGame();
	ReceiveBeginGame();


}

void AActorDeferredPlay::BeginPlay()
{
	Super::BeginPlay();

	if(s_bHasBegunGame)
	{
		BeginGame();
		return;


	}
	
	s_eBeginGame.AddUObject(this, &AActorDeferredPlay::NotifyBeginGame);


}
