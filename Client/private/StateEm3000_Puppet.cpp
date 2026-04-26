#include "pch.h"
#include "StateEm3000_Puppet.h"

#include "Em3000.h"
#include "Em3000Body.h"
#include "Em3000StateMachine.h"

StateEm3000_Puppet::StateEm3000_Puppet(const wstring& tag, const Shared<Em3000>& owner)
	: StateEm3000{tag, owner} {}

HRESULT StateEm3000_Puppet::Initialize()
{
	return StateEm3000::Initialize();
}

using em3000State = Em3000::EM3000_STATE;

Bool StateEm3000_Puppet::StateEnterInvoke()
{
	auto em3000Body = m_Body.lock();

	em3000Body->Set_Animation(ETOI(em3000State::SCREAMING), 0.1f, false);
	m_Owner.lock()->SummonPuppet(4);

	return true;
}

void StateEm3000_Puppet::Update(Float timeDelta)
{
	auto em3000Body = m_Body.lock();
	auto state = m_States.lock();
	Bool isFinished = em3000Body->Is_AnimationFinished();

	if (isFinished)
	{
		state->Change_State(em3000State::PHASE2_CHASE);
	}
}

void StateEm3000_Puppet::Late_Update(Float timeDelta)
{

}

void StateEm3000_Puppet::StateExitInvoke()
{

}

Shared<StateEm3000_Puppet> StateEm3000_Puppet::Create(const wstring& tag, const Shared<Em3000>& owner)
{
	auto prototype = make_shared<StateEm3000_Puppet>(tag, owner);

	if (FAILED(prototype->Initialize()))
	{
		MSG_BOX("Failed to Created : StateEm3000_Puppet");
		return nullptr;
	}

	return prototype;
}

