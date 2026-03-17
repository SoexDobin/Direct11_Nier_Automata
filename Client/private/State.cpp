#include "pch.h"
#include "State.h"

State::State(const wstring& tag, const Shared<GameObject> owner)
	: m_StateTag{ tag }, m_Owner{ owner }
{
}

