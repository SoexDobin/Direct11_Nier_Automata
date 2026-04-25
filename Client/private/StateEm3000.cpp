#include "pch.h"
#include "StateEm3000.h"

#include <Game.h>
#include"Em3000Body.h"
#include "Em3000StateMachine.h"
#include "Em3000Movement.h"
#include "Em3000MeleeSight.h"

StateEm3000::StateEm3000(const wstring& tag, const Shared<Em3000>& owner)
	: State{ tag }, m_Owner{ owner }
{

}

StateEm3000::~StateEm3000()
{
	m_Owner.reset();
	m_Body.reset();
	m_Movement.reset();
	m_States.reset();
	m_MeleeSight.reset();
}

HRESULT StateEm3000::Initialize()
{
	uint32 levIndex = GAME_INSTANCE->Get_TargetLevelIndex();

	m_Body = static_pointer_cast<Em3000Body>(m_Owner.lock()->Find_PartObject(L"Em3000Body"));
	m_States = m_Owner.lock()->Get_Component<Em3000StateMachine>();
	m_Movement = m_Owner.lock()->Get_Component<Em3000Movement>();
	m_MeleeSight = static_pointer_cast<Em3000MeleeSight>(m_Owner.lock()->Find_PartObject(L"Em3000MeleeSight"));

	return S_OK;
}

Bool StateEm3000::Has_FrontMeleeTarget() const
{
	if (m_MeleeSight.expired()) return false;

	return m_MeleeSight.lock()->Is_TargetFront();
}

Bool StateEm3000::Has_MeleeTarget() const
{
	if (m_MeleeSight.expired()) return false;

	return m_MeleeSight.lock()->Has_Target();
}
