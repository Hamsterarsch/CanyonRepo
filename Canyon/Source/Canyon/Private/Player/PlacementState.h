#pragma once
#include <memory>


enum class EAbstractInputEvent : size_t;

class IPlacementState
{
public:
	virtual ~IPlacementState() = default;

	virtual std::unique_ptr<IPlacementState> HandleInput(class CPlacementStateMachine *pParent, EAbstractInputEvent Input) = 0;

	virtual void Update(class CPlacementStateMachine *pParent) = 0;

	virtual void ReceiveOnExit(class CPlacementStateMachine *pParent) = 0;

	virtual void ReceiveOnEnter(class CPlacementStateMachine *pParent) = 0;


};

class CPlacementStateMachine
{
public:
	CPlacementStateMachine();
	CPlacementStateMachine(class ARTSPlayerEye *pOwner);

	void Update();

	void HandleInput(EAbstractInputEvent Input);

	inline class ARTSPlayerEye *GetEye() { return m_pOwningEye; }
	
	inline bool GetIsInPlacement() const { return m_bIsInPlacement; }

	void SetIsInPlacement(bool bIsInPlacement);


protected:
	std::unique_ptr<IPlacementState>	m_pCurrentState;
	class ARTSPlayerEye *m_pOwningEye;
	bool m_bIsInPlacement;
	


};

class CPlacementState_Idle : public IPlacementState
{
public:
	virtual std::unique_ptr<IPlacementState> HandleInput(CPlacementStateMachine *pParent, EAbstractInputEvent Input) override;

	virtual void Update(CPlacementStateMachine *pParent) override {};

	virtual void ReceiveOnExit(CPlacementStateMachine *pParent) override {};

	virtual void ReceiveOnEnter(CPlacementStateMachine *pParent) override {};


};

class CPlacementState_PlacementBuilding : public IPlacementState
{
public:
	virtual std::unique_ptr<IPlacementState> HandleInput(CPlacementStateMachine *pParent, EAbstractInputEvent Input) override;
	
	virtual void Update(CPlacementStateMachine *pParent) override;

	virtual void ReceiveOnExit(CPlacementStateMachine *pParent) override;

	virtual void ReceiveOnEnter(CPlacementStateMachine *pParent) override;


protected:
	float m_CommitStartTime;
	float m_AbortStartTime;


};
