#include "pch.h"
#include "State2B.h"

namespace Client
{
	State2B::State2B(const wstring& tag, const Shared<P10000Body>& owner)
		: State{ tag }, m_Owner{ owner }
	{
	}

	State2B::~State2B()
	{
		m_Owner.reset();
	}
}
