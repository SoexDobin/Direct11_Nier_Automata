#include "pch.h"
#include "StateEm3000_Groggy.h"

#include "Em3000.h"
#include "Em3000Body.h"
#include "Em3000StateMachine.h"

StateEm3000_Groggy::StateEm3000_Groggy(const wstring& tag, const Shared<Em3000>& owner)
	: StateEm3000{tag, owner} {}
HRESULT StateEm3000_Groggy::Initialize()
{
	return StateEm3000::Initialize();
}

using em3000State = Em3000::EM3000_STATE;

Bool StateEm3000_Groggy::StateEnterInvoke()
{
	m_Body.lock()->Set_Animation(ETOI(em3000State::GROGGY_IN), 0.1f, false);

	return true;
}

void StateEm3000_Groggy::Update(Float timeDelta)
{
	auto em3000Body = m_Body.lock();
	Bool isFinished = m_Body.lock()->Is_AnimationFinished();
	uint32 animIndex = m_Body.lock()->Get_CurrentAnimationIndex();

	if (animIndex == ETOI(em3000State::GROGGY_LOOP_1))
		m_GroggyElapsed += timeDelta;

	if (isFinished && animIndex == ETOI(em3000State::GROGGY_END))
	{
		m_States.lock()->Change_State(em3000State::PHASE2_TRANSFORM);
	}

	if (m_GroggyElapsed >= m_GroggyTime)
	{
		em3000Body->Set_Animation(ETOI(em3000State::GROGGY_END), 0.1f, false);
		return;
	}

	if (isFinished && ETOI(em3000State::GROGGY_IN) == animIndex)
	{
		em3000Body->Set_Animation(ETOI(em3000State::GROGGY_LOOP_1), 0.1f, true);
	}
}

void StateEm3000_Groggy::Late_Update(Float timeDelta)
{

}

void StateEm3000_Groggy::StateExitInvoke()
{

}

Shared<StateEm3000_Groggy> StateEm3000_Groggy::Create(const wstring& tag, const Shared<Em3000>& owner)
{
	auto prototype = make_shared<StateEm3000_Groggy>(tag, owner);

	if (FAILED(prototype->Initialize()))
	{
		MSG_BOX("Failed to Created : StateEm3000_Groggy");
		return nullptr;
	}

	return prototype;
}