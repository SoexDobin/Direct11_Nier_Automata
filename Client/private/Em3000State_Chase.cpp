#include "pch.h"
#include "Em3000State_Chase.h"

#include "Em3000.h"
#include "Em3000Body.h"
#include "Em3000Movement.h"
#include "Em3000StateMachine.h"

StateEm3000_Chase::StateEm3000_Chase(const wstring& tag, const Shared<Em3000>& owner)
	: StateEm3000{ tag, owner } {}

HRESULT StateEm3000_Chase::Initialize()
{
	return StateEm3000::Initialize();
}

using em3000State = Em3000::EM3000_STATE;

Bool StateEm3000_Chase::StateEnterInvoke()
{
	auto em3000Body = m_Body.lock();
	em3000Body->Set_Animation(ETOI(em3000State::READY_CHASE), 0.1f, false);

	return true;
}

void StateEm3000_Chase::Update(Float timeDelta)
{
	auto em3000 = m_Owner.lock();
	auto em3000Body = m_Body.lock();
	auto state = m_States.lock();
	auto movement = m_Movement.lock();

	uint32 animIndex = em3000Body->Get_CurrentAnimationIndex();
	Bool isFinished = em3000Body->Is_AnimationFinished();


	if (isFinished && animIndex == ETOI(em3000State::READY_CHASE))
	{
		em3000Body->Set_Animation(ETOI(em3000State::CHASE), 0.1f, true);
	}

	// TODO Chase 수행 제어
	Float dist = Get_DistanceToTarget();
	if (dist <= m_TargetDistance)
	{
		state->Change_State(em3000State::PHASE2_STOMP);
		return;
	}

	Em3000Movement::EM3000_MOVEMENT_DATA movementData{};
	movementData.isMove = true;
	movementData.direction = Get_DirectionToTarget();
	movementData.useRootMotionDir = true; // Transform Frame 데이터 기반 이동
	movement->Set_MovementData(movementData);
}

void StateEm3000_Chase::Late_Update(Float timeDelta)
{
	
}

void StateEm3000_Chase::StateExitInvoke()
{
	
}

Shared<StateEm3000_Chase> StateEm3000_Chase::Create(const wstring& tag, const Shared<Em3000>& owner)
{
	auto prototype = make_shared<StateEm3000_Chase>(tag, owner);

	if (FAILED(prototype->Initialize()))
	{
		MSG_BOX("Failed to Created : StateEm3000_Chase");
		return nullptr;
	}

	return prototype;
}

