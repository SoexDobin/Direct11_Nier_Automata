#include "pch.h"
#include "State2B_AttackGround.h"

#include <SpdLogger.h>

#include "Pl0000.h"
#include "Pl0000StateMachine.h"

State2B_AttackGround::State2B_AttackGround(const wstring& tag, const Shared<Pl0000>& owner)
	: State2B{tag, owner}
{
}

HRESULT State2B_AttackGround::Initialize()
{
	m_EnterAnim.emplace(Pl0000::PL0000_STATE::LIGHT_GROUND_RUN);
	m_EnterAnim.emplace(Pl0000::PL0000_STATE::HEAVY_GROUND_SPRINT);


	return S_OK;
}

Bool State2B_AttackGround::StateEnterInvoke()
{

	switch (auto prevState = m_States.lock()->Get_CurP10000State())
	{
	case Pl0000::PL0000_STATE::IDLE:

	case Pl0000::PL0000_STATE::RUN:

	default: 
		LOG_ERROR(L"[ENTER ATTACK GROUND] : No Enter state PrevIndex {} ", Helper::To_wString(magic_enum::enum_name(prevState)));
		return false;
	}
}

void State2B_AttackGround::Update(Float timeDelta)
{
	State2B::Update(timeDelta);
}

void State2B_AttackGround::Late_Update(Float timeDelta)
{
	
}

void State2B_AttackGround::StateExitInvoke()
{
	
}

Shared<State2B_AttackGround> State2B_AttackGround::Create(const wstring& tag, const Shared<Pl0000>& owner)
{
	auto instance = make_shared<State2B_AttackGround>(tag, owner);

	if (FAILED(instance->Initialize()))
	{
		LOG_ERROR(L"Failed to Create State2B_AttackGround");
		return nullptr;
	}

	return instance;
}

