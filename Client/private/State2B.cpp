#include "pch.h"
#include "State2B.h"

#include "P10000.h"
#include "P10000Input.h"
#include "P10000StateMachine.h"
#include "P10000Body.h"
#include "WP0070Body.h"
#include "WP0220Body.h"

namespace Client
{
	State2B::State2B(const wstring& tag, const Shared<P10000>& owner)
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
		m_Body = static_pointer_cast<P10000Body>(m_Owner.lock()->Find_PartObject(L"P10000Body"));

		m_LightWeapon = static_pointer_cast<WP0070Body>(m_Owner.lock()->Find_PartObject(L"WP0070Body"));
		m_HeavyWeapon = static_pointer_cast<WP0220Body>(m_Owner.lock()->Find_PartObject(L"WP0220Body"));

		m_States = m_Owner.lock()->Get_Component<P10000StateMachine>();
		m_Input = m_Owner.lock()->Get_Component<P10000Input>();

		return S_OK;
	}
}
