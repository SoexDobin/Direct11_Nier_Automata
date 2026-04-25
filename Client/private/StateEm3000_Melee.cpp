#include "pch.h"
#include "StateEm3000_Melee.h"

StateEm3000_Melee::StateEm3000_Melee(const wstring& tag, const Shared<Em3000>& owner)
	: StateEm3000(tag, owner) {}

HRESULT StateEm3000_Melee::Initialize()
{
	return StateEm3000::Initialize();
}

Bool StateEm3000_Melee::StateEnterInvoke()
{
	return StateEm3000::StateEnterInvoke();
}

void StateEm3000_Melee::Update(Float timeDelta)
{
	
}

void StateEm3000_Melee::Late_Update(Float timeDelta)
{
	
}

void StateEm3000_Melee::StateExitInvoke()
{
	
}

Shared<StateEm3000_Melee> StateEm3000_Melee::Create(const wstring& tag, const Shared<Em3000>& owner)
{
	auto prototype = make_shared<StateEm3000_Melee>(tag, owner);

	if (FAILED(prototype->Initialize()))
	{
		MSG_BOX("Failed to Created : StateEm3000_Melee");
		return nullptr;
	}

	return prototype;
}

