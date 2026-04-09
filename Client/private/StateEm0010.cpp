#include "pch.h"
#include "StateEm0010.h"

#include "Em0010.h"
#include "Em0010Movement.h"
#include "MonsterStateMachine.h"

using Client::MonsterStateMachine;

StateEm0010::StateEm0010(const wstring& tag, const Shared<Em0010>& owner)
	: State(tag), m_Owner(owner)
{
}

StateEm0010::~StateEm0010()
{
	m_Owner.reset();
	m_Body.reset();
	m_Movement.reset();
	m_States.reset();
}

HRESULT StateEm0010::Initialize()
{
	if (m_Owner.expired()) return E_FAIL;

	m_Body = static_pointer_cast<Em0010Body>(m_Owner.lock()->Find_PartObject(L"Em0010Body"));

	m_Movement = m_Owner.lock()->Get_Component<Em0010Movement>();
	m_States = m_Owner.lock()->Get_Component<MonsterStateMachine>();

	return S_OK;
}

