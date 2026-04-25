#include "pch.h"
#include "StateEm3000_Range.h"

#include "Em3000Body.h"
#include "Em3000StateMachine.h"
#include "Random_Helper.h"

StateEm3000_Range::StateEm3000_Range(const wstring& tag, const Shared<Em3000>& owner)
	: StateEm3000(tag, owner) {}

HRESULT StateEm3000_Range::Initialize()
{
	using em3000state = Em3000::EM3000_STATE;

	m_EnterAnim.emplace(ETOI(em3000state::DANMAK_START_1));
	m_EnterAnim.emplace(ETOI(em3000state::DANMAK_START_2));
	m_EnterAnim.emplace(ETOI(em3000state::DANMAK_START_3));

	m_AttackAnim.emplace(ETOI(em3000state::DANMAK_LOOP_1));
	m_AttackAnim.emplace(ETOI(em3000state::DANMAK_LOOP_2));
	m_AttackAnim.emplace(ETOI(em3000state::DANMAK_LOOP_3));
	
	m_EndAnim.emplace(ETOI(em3000state::DANMAK_END_1));
	m_EndAnim.emplace(ETOI(em3000state::DANMAK_END_2));
	m_EndAnim.emplace(ETOI(em3000state::DANMAK_END_3));

	return StateEm3000::Initialize();
}

using em3000state = Em3000::EM3000_STATE;

Bool StateEm3000_Range::StateEnterInvoke()
{
	auto em3000Body = m_Body.lock();
	auto state = m_States.lock();
	
	m_EntryAnimIndex = Helper::Random_Int(0, 2);
	em3000Body->Set_Animation(m_RangeEnter[m_EntryAnimIndex], 0.1f, false);

	return true;
}

void StateEm3000_Range::Update(Float timeDelta)
{
	auto em3000 = m_Owner.lock();
	auto em3000Body = m_Body.lock();
	auto state = m_States.lock();
	auto movement = m_Movement.lock();

	uint32 animIndex = em3000Body->Get_CurrentAnimationIndex();
	Bool IsFinished = em3000Body->Is_AnimationFinished();

	if (IsFinished && m_EnterAnim.contains(animIndex))
	{
		
		em3000Body->Set_Animation(m_RangeAttack[m_EntryAnimIndex], 0.1f, false);

		if (m_EntryAnimIndex == 0)
			em3000Body->Fire_Bullet(true);
		else if (m_EntryAnimIndex == 1)
			em3000Body->Fire_Bullet(false);

		return;
	}

	if (IsFinished && m_AttackAnim.contains(animIndex))
	{

		em3000Body->Set_Animation(m_RangeEnd[m_EntryAnimIndex], 0.1f, false);

		if (m_EntryAnimIndex == 0 || m_EntryAnimIndex == 1)
			em3000Body->Stop_Bullet();

		return;
	}

	if (IsFinished && m_EndAnim.contains(animIndex))
	{
		state->Change_State(em3000state::IDLE);
		return;
	}
}

void StateEm3000_Range::Late_Update(Float timeDelta)
{
	
}

void StateEm3000_Range::StateExitInvoke()
{
	
}

Shared<StateEm3000_Range> StateEm3000_Range::Create(const wstring& tag, const Shared<Em3000>& owner)
{
	auto prototype = make_shared<StateEm3000_Range>(tag, owner);

	if (FAILED(prototype->Initialize()))
	{
		MSG_BOX("Failed to Created : Em3000State_Range");
		return nullptr;
	}

	return prototype;
}

