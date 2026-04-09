#include "pch.h"
#include "StateEm0010_Hit.h"

StateEm0010_Hit::StateEm0010_Hit(const wstring& tag, const Shared<Em0010>& owner)
	: StateEm0010{tag, owner} {}
StateEm0010_Hit::~StateEm0010_Hit() {}


HRESULT StateEm0010_Hit::Initialize()
{
	return StateEm0010::Initialize();
}


Bool StateEm0010_Hit::StateEnterInvoke()
{
	return StateEm0010::StateEnterInvoke();
}

void StateEm0010_Hit::Update(Float timeDelta)
{
	
}

void StateEm0010_Hit::Late_Update(Float timeDelta)
{
	
}

void StateEm0010_Hit::StateExitInvoke()
{
	
}

Shared<StateEm0010_Hit> StateEm0010_Hit::Create(const wstring& tag, const Shared<Em0010>& owner)
{
	auto instance = make_shared<StateEm0010_Hit>(tag, owner);

	if (FAILED(instance->Initialize()))
	{
		MSG_BOX("Failed to Clone : StateEm0010_Hit");
		return nullptr;
	}

	return instance;
}

