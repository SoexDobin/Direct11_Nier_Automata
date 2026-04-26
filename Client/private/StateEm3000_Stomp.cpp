#include "pch.h"
#include "StateEm3000_Stomp.h"

#include "Em3000.h"
#include "Em3000Body.h"
#include "Em3000Movement.h"
#include "Em3000StateMachine.h"

StateEm3000_Stomp::StateEm3000_Stomp(const wstring& tag, const Shared<Em3000>& owner)
	: StateEm3000(tag, owner)
{
}

HRESULT StateEm3000_Stomp::Initialize()
{
	return StateEm3000::Initialize();
}

using em3000State = Em3000::EM3000_STATE;

Bool StateEm3000_Stomp::StateEnterInvoke()
{

	auto em3000Body = m_Body.lock();
	em3000Body->Set_Animation(ETOI(em3000State::JUMP_STOMP), 0.1f, false);

	return true;
}

void StateEm3000_Stomp::Update(Float timeDelta)
{
	auto em3000 = m_Owner.lock();
	auto em3000Body = m_Body.lock();
	auto state = m_States.lock();
	auto movement = m_Movement.lock();

	if (Bool isFinished = em3000Body->Is_AnimationFinished())
	{
		Float dist = Get_DistanceToTarget();
		if (dist > m_TargetDistance)
		{
			state->Change_State(em3000State::PHASE2_CHASE);
		}
		else
		{
			em3000Body->Set_Animation(ETOI(em3000State::JUMP_STOMP), 0.1f, false);
		}
	}
	
}

void StateEm3000_Stomp::Late_Update(Float timeDelta)
{
	
}

void StateEm3000_Stomp::StateExitInvoke()
{
	
}

Shared<StateEm3000_Stomp> StateEm3000_Stomp::Create(const wstring& tag, const Shared<Em3000>& owner)
{
	auto prototype = make_shared<StateEm3000_Stomp>(tag, owner);

	if (FAILED(prototype->Initialize()))
	{
		MSG_BOX("Failed to Created : StateEm3000_Stomp");
		return nullptr;
	}

	return prototype;
}

