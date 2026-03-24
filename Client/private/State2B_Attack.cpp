#include "pch.h"
#include "State2B_Attack.h"

#include <SpdLogger.h>

State2B_Attack::State2B_Attack(const wstring& tag, const Shared<P10000>& owner)
	: State2B{tag, owner}
{
}

HRESULT State2B_Attack::Initialize()
{
	return S_OK;
}

Bool State2B_Attack::StateEnterInvoke()
{
	return State2B::StateEnterInvoke();
}

void State2B_Attack::Update(Float timeDelta)
{
	State2B::Update(timeDelta);
}

void State2B_Attack::Late_Update(Float timeDelta)
{
	State2B::Late_Update(timeDelta);
}

void State2B_Attack::StateExitInvoke()
{
	State2B::StateExitInvoke();
}

Shared<State2B_Attack> State2B_Attack::Create(const wstring& tag, const Shared<P10000>& owner)
{
	auto instance = make_shared<State2B_Attack>(tag, owner);

	if (FAILED(instance->Initialize()))
	{
		LOG_ERROR(L"Failed to Create State2B_Attack");
		return nullptr;
	}

	return instance;
}

