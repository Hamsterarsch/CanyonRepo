#include "PlacementState.h"
#include "RTSPlayerEye.h"
#include "GameFramework/PlayerController.h"


CPlacementStateMachine::CPlacementStateMachine() :
	CPlacementStateMachine(nullptr)
{
}

CPlacementStateMachine::CPlacementStateMachine(class ARTSPlayerEye *pOwner) :
	m_pCurrentState{ std::make_unique<CPlacementState_Idle>() },
	m_pOwningEye{ pOwner }
{
}

void CPlacementStateMachine::Update()
{
	if (m_pCurrentState && m_pOwningEye)
	{
		m_pCurrentState->Update(this);
	}


}

void CPlacementStateMachine::HandleInput(EAbstractInputEvent Input)
{
	if (m_pCurrentState && m_pOwningEye)
	{
		auto pNewState{ m_pCurrentState->HandleInput(this, Input) };		
		if (pNewState)
		{
			m_pCurrentState->ReceiveOnExit(this);
			m_pCurrentState = std::move(pNewState);
			m_pCurrentState->ReceiveOnEnter(this);
		}
	}


}



//STATES

std::unique_ptr<IPlacementState> CPlacementState_Idle::HandleInput(CPlacementStateMachine *pParent, EAbstractInputEvent Input)
{
	switch (Input)
	{
	case EAbstractInputEvent::PlaceBuilding_Start:
		return std::make_unique<CPlacementState_PlacementBuilding>();
		break;
	case EAbstractInputEvent::ActionSelect_Start:
		break;
	}
	return nullptr;


}

std::unique_ptr<IPlacementState> CPlacementState_PlacementBuilding::HandleInput(CPlacementStateMachine *pParent, EAbstractInputEvent Input)
{
	switch (Input)
	{
	case EAbstractInputEvent::ActionSelect_Start:
		if (pParent->GetEye()->TryCommitPlaceablePreview())
		{
			return std::make_unique<CPlacementState_Idle>();
		}
		break;
	case EAbstractInputEvent::ActionContext_Start:
		m_AbortStartTime = pParent->GetEye()->GetWorld()->GetTimeSeconds();
		break;
	case EAbstractInputEvent::ActionContext_End:
		auto m_PassedTime{ pParent->GetEye()->GetWorld()->GetTimeSeconds() - m_AbortStartTime };
		if (m_PassedTime < m_AbortSuccessTime)
		{
			pParent->GetEye()->DiscardCurrentPlaceablePreview();
			return std::make_unique<CPlacementState_Idle>();
		}
		break;
	}
	return nullptr;


}

void CPlacementState_PlacementBuilding::Update(CPlacementStateMachine *pParent)
{
	//update preview cursor position
	//todo: recheck pParent->GetEye()->UpdatePreviewCursorPos();	

	//notify preview about placable state
	pParent->GetEye()->UpdateCurrentPlaceablePreview();


}


