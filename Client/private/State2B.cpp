#include "pch.h"
#include "State2B.h"

#include "Pl0000.h"
#include "Pl0000Input.h"
#include "Pl0000StateMachine.h"
#include "Pl0000Body.h"
#include "WP0070Body.h"
#include "WP0220Body.h"

namespace Client
{
	State2B::State2B(const wstring& tag, const Shared<Pl0000>& owner)
		: State{ tag }, m_Owner{ owner }
	{
	}

	State2B::~State2B()
	{
		m_Owner.reset();
		m_Body.reset();
		m_States.reset();
		m_Input.reset();
		m_LightWeapon.reset();
		m_HeavyWeapon.reset();
	}

	HRESULT State2B::Initialize()
	{
		m_Body = static_pointer_cast<Pl0000Body>(m_Owner.lock()->Find_PartObject(L"Pl0000Body"));

		m_LightWeapon = static_pointer_cast<WP0070Body>(m_Owner.lock()->Find_PartObject(L"WP0070Body"));
		m_HeavyWeapon = static_pointer_cast<WP0220Body>(m_Owner.lock()->Find_PartObject(L"WP0220Body"));

		m_States = m_Owner.lock()->Get_Component<Pl0000StateMachine>();
		m_Input = m_Owner.lock()->Get_Component<Pl0000Input>();

		return S_OK;
	}
}
