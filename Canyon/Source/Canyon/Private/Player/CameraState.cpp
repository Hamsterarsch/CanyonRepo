#include "CameraState.h"
#include "RTSPlayerEye.h"
#include "Components/InputComponent.h"
#include "RTSPlayerController.h"


CCameraStateMachine::CCameraStateMachine() :
	CCameraStateMachine(nullptr)
{
}

CCameraStateMachine::CCameraStateMachine(class ARTSPlayerEye *pOwner) :
	m_pCurrentState{ std::make_unique<CCameraState_Idle>() },
	m_pOwningEye{ pOwner }
{
}

void CCameraStateMachine::Update()
{
	if (m_pCurrentState && m_pOwningEye)
	{
		m_pCurrentState->Update(m_pOwningEye);
	}

}

void CCameraStateMachine::HandleInput(EAbstractInputEvent Input)
{
	if (m_pCurrentState)
	{
		auto pNewState{ m_pCurrentState->HandleInput(Input) };

		if (pNewState && m_pOwningEye)
		{
			m_pCurrentState->ReceiveOnExit(m_pOwningEye);
			m_pCurrentState = std::move(pNewState);
			m_pCurrentState->ReceiveOnEnter(m_pOwningEye);
		}
	}

}


//STATES

std::unique_ptr<ICameraState> CCameraState_Idle::HandleInput(EAbstractInputEvent Input)
{
	switch (Input)
	{
	case EAbstractInputEvent::ActionRotate_Start:
		return std::make_unique<CCameraState_Rotate>();
		break;
	case EAbstractInputEvent::ActionContext_Start:
		return std::make_unique<CCameraState_Move>();
		break;
	}
	return nullptr;


}

std::unique_ptr<ICameraState> CCameraState_Move::HandleInput(EAbstractInputEvent Input)
{
	switch (Input)
	{
	case EAbstractInputEvent::ActionRotate_Start:
		return std::make_unique<CCameraState_Rotate>();
		break;
	case EAbstractInputEvent::ActionContext_End:
		return std::make_unique<CCameraState_Idle>();
		break;
	}
	return nullptr;


}

void CCameraState_Move::Update(ARTSPlayerEye *pEye)
{
	auto *pIC{ pEye->InputComponent };
	if (!pIC)
	{
		return;
	}

	auto MouseX{ pIC->GetAxisValue(ARTSPlayerEye::s_AxisMouseX) };
	auto MouseY{ pIC->GetAxisValue(ARTSPlayerEye::s_AxisMouseY) };

	pEye->AddRightMovementFromMouse(MouseX);
	pEye->AddForwardMovementFromMouse(MouseY);


}

std::unique_ptr<ICameraState> CCameraState_Rotate::HandleInput(EAbstractInputEvent Input)
{
	switch (Input)
	{
	case EAbstractInputEvent::ActionContext_Start:
		return std::make_unique<CCameraState_Move>();
		break;
	case EAbstractInputEvent::ActionRotate_End:
		return std::make_unique<CCameraState_Idle>();
		break;
	}
	return nullptr;


}

void CCameraState_Rotate::Update(class ARTSPlayerEye *pEye)
{
	auto *pIC{ pEye->InputComponent };
	if (!pIC)
	{
		return;
	}

	auto *pCtrl{ Cast<ARTSPlayerController>(pEye->GetController()) };
	if (!pCtrl)
	{
		return;
	}

	auto MouseX{ pIC->GetAxisValue(ARTSPlayerEye::s_AxisMouseX) };
	auto MouseY{ pIC->GetAxisValue(ARTSPlayerEye::s_AxisMouseY) };

	pEye->AddCameraYawFromMouse(MouseX);
	pEye->AddCameraPitchFromMouse(MouseY);

	pCtrl->SetMouseLocation(m_OnEnterMousePos.X, m_OnEnterMousePos.Y);
	

}

void CCameraState_Rotate::ReceiveOnExit(class ARTSPlayerEye *pEye)
{
	auto *pCtrl{ Cast<ARTSPlayerController>(pEye->GetController()) };
	if (!pCtrl)
	{
		return;
	}

	pCtrl->SetMouseLocation(m_OnEnterMousePos.X, m_OnEnterMousePos.Y);


}

void CCameraState_Rotate::ReceiveOnEnter(class ARTSPlayerEye *pEye)
{
	auto *pCtrl{ Cast<ARTSPlayerController>(pEye->GetController()) };
	if (!pCtrl)
	{
		return;
	}

	pCtrl->GetMousePosition(m_OnEnterMousePos.X, m_OnEnterMousePos.Y);


}
