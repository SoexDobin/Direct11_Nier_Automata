#include "pch.h"
#include "StateEm3000.h"

#include <Game.h>
#include"Em3000Body.h"
#include "Em3000StateMachine.h"
#include "Em3000Movement.h"

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
}

HRESULT StateEm3000::Initialize()
{
	uint32 levIndex = GAME_INSTANCE->Get_TargetLevelIndex();

	m_Body = static_pointer_cast<Em3000Body>(m_Owner.lock()->Find_PartObject(L"Em3000Body"));
	m_States = m_Owner.lock()->Get_Component<Em3000StateMachine>();
	m_Movement = m_Owner.lock()->Get_Component<Em3000Movement>();

	return S_OK;
}

Bool StateEm3000::HasTarget() const
{
	return false;
}

Vector3 StateEm3000::Calculate_Direction()
{
	return Vector3::Zero;
}
