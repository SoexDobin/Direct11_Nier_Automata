#include "pch.h"
#include "State2B.h"

State2B::State2B(const wstring& tag, const Shared<P10000>& owner)
	: State{ tag }, m_Owner{ owner }
{
}
