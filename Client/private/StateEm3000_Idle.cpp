#include "pch.h"
#include "StateEm3000_Idle.h"

StateEm3000_Idle::StateEm3000_Idle(const wstring& tag, const Shared<Em3000>& owner)
	: StateEm3000{tag, owner} {}

HRESULT StateEm3000_Idle::Initialize()
{
	return StateEm3000::Initialize();
}

Bool StateEm3000_Idle::StateEnterInvoke()
{
	return StateEm3000::StateEnterInvoke();
}

void StateEm3000_Idle::Update(Float timeDelta)
{
	StateEm3000::Update(timeDelta);
}

void StateEm3000_Idle::Late_Update(Float timeDelta)
{
	StateEm3000::Late_Update(timeDelta);
}

void StateEm3000_Idle::StateExitInvoke()
{
	StateEm3000::StateExitInvoke();
}

Shared<StateEm3000_Idle> StateEm3000_Idle::Create(const wstring& tag, const Shared<Em3000>& owner)
{
	auto prototype = make_shared<StateEm3000_Idle>(tag, owner);

	if (FAILED(prototype->Initialize()))
	{
		MSG_BOX("Failed to Created : StateEm3000_Idle");
		return nullptr;
	}

	return prototype;
}