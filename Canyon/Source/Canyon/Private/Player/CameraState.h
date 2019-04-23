#pragma once
#include <memory>


enum class EAbstractInputEvent : size_t;

class ICameraState
{
public:
	virtual ~ICameraState() = default;

	virtual std::unique_ptr<ICameraState> HandleInput(EAbstractInputEvent Input) = 0;

	virtual void Update(class ARTSPlayerEye *pEye) = 0;

	virtual void ReceiveOnExit(class ARTSPlayerEye *pEye) = 0;

	virtual void ReceiveOnEnter(class ARTSPlayerEye *pEye) = 0;


};

class CCameraStateMachine
{
public:
	CCameraStateMachine();
	CCameraStateMachine(class ARTSPlayerEye *pOwner);

	void Update();

	void HandleInput(EAbstractInputEvent Input);


protected:
	std::unique_ptr<ICameraState> m_pCurrentState;

	class ARTSPlayerEye *m_pOwningEye;


};

class CCameraState_Idle : public ICameraState
{
public:
	virtual std::unique_ptr<ICameraState> HandleInput(EAbstractInputEvent Input) override;

	virtual void Update(class ARTSPlayerEye *pEye) override {};

	virtual void ReceiveOnExit(class ARTSPlayerEye *pEye) override {};

	virtual void ReceiveOnEnter(class ARTSPlayerEye *pEye) override {};


};

class CCameraState_Move : public ICameraState
{
public:
	virtual std::unique_ptr<ICameraState> HandleInput(EAbstractInputEvent Input) override;

	virtual void Update(class ARTSPlayerEye *pEye) override;

	virtual void ReceiveOnExit(class ARTSPlayerEye *pEye) override {};

	virtual void ReceiveOnEnter(class ARTSPlayerEye *pEye) override {};


};

class CCameraState_Rotate : public ICameraState
{
public:
	virtual std::unique_ptr<ICameraState> HandleInput(EAbstractInputEvent Input) override;

	virtual void Update(class ARTSPlayerEye *pEye) override;

	virtual void ReceiveOnExit(class ARTSPlayerEye *pEye) override;

	virtual void ReceiveOnEnter(class ARTSPlayerEye *pEye) override;

protected:
	FVector2D m_OnEnterMousePos;


};