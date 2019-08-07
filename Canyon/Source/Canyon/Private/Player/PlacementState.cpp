#include "PlacementState.h"
#include "RTSPlayerEye.h"
#include "GameFramework/PlayerController.h"


CPlacementStateMachine::CPlacementStateMachine() :
	CPlacementStateMachine(nullptr)
{
}

CPlacementStateMachine::CPlacementStateMachine(class ARTSPlayerEye *pOwner) :
	m_pCurrentState{ std::make_unique<CPlacementState_Idle>() },
	m_pOwningEye{ pOwner },
	m_bIsInPlacement{ false }
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

void CPlacementStateMachine::SetIsInPlacement(bool bIsInPlacement)
{
	m_bIsInPlacement = bIsInPlacement;


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
		m_CommitStartTime = pParent->GetEye()->GetWorld()->GetTimeSeconds();
		break;
	case EAbstractInputEvent::ActionSelect_End:
		//auto PassedTimeCommit{ pParent->GetEye()->GetWorld()->GetTimeSeconds() - m_CommitStartTime };
		if((pParent->GetEye()->GetWorld()->GetTimeSeconds() - m_CommitStartTime) < pParent->GetEye()->GetPlacementAbortSuccessTime())
		{
			if (pParent->GetEye()->TryCommitPlaceablePreview())
			{
				return std::make_unique<CPlacementState_Idle>();
			}			
		}
		break;


	case EAbstractInputEvent::ActionContext_Start:
		m_AbortStartTime = pParent->GetEye()->GetWorld()->GetTimeSeconds();
		break;
	case EAbstractInputEvent::ActionContext_End:
		//auto PassedTimeAbort{ pParent->GetEye()->GetWorld()->GetTimeSeconds() - m_AbortStartTime };
		if ((pParent->GetEye()->GetWorld()->GetTimeSeconds() - m_AbortStartTime) < pParent->GetEye()->GetPlacementAbortSuccessTime())
		{
			pParent->GetEye()->DiscardCurrentPlaceablePreview(true);
			return std::make_unique<CPlacementState_Idle>();
		}
		break;

	case EAbstractInputEvent::ActionRotate_Inc:
		pParent->GetEye()->IncreaseBuildingRot();		
		break;

	case EAbstractInputEvent::ActionRotate_Dec:
		pParent->GetEye()->DecreaseBuildingRot();
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

void CPlacementState_PlacementBuilding::ReceiveOnExit(CPlacementStateMachine* pParent)
{
	pParent->SetIsInPlacement(false);


}

void CPlacementState_PlacementBuilding::ReceiveOnEnter(CPlacementStateMachine* pParent)
{
	pParent->SetIsInPlacement(true);


}


