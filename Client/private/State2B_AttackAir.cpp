#include "pch.h"
#include "State2B_AttackAir.h"

State2B_AttackAir::State2B_AttackAir(const wstring& tag, const Shared<Pl0000>& owner)
	: State2B{ tag, owner }
{
}

HRESULT State2B_AttackAir::Initialize()
{
	return State2B::Initialize();
}

Bool State2B_AttackAir::StateEnterInvoke()
{
	return State2B::StateEnterInvoke();
}

void State2B_AttackAir::Update(Float timeDelta)
{
	
}

void State2B_AttackAir::Late_Update(Float timeDelta)
{
	
}

void State2B_AttackAir::StateExitInvoke()
{
	
}

Shared<State2B_AttackAir> State2B_AttackAir::Create(const wstring& tag, const Shared<Pl0000>& owner)
{
	auto instance = make_shared<State2B_AttackAir>(tag, owner);

	if (FAILED(instance->Initialize()))
	{
		MSG_BOX("Failed to Created : State2B_AttackAir");
		return nullptr;
	}

	return instance;
}
