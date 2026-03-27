#include "pch.h"
#include "State2B_Jump.h"

#include <SpdLogger.h>

State2B_Jump::State2B_Jump(const wstring& tag, const Shared<Pl0000>& owner)
	: State2B{tag, owner}
{
}

HRESULT State2B_Jump::Initialize()
{

	return S_OK;
}

Bool State2B_Jump::StateEnterInvoke()
{
	return State2B::StateEnterInvoke();
}

void State2B_Jump::Update(Float timeDelta)
{
	State2B::Update(timeDelta);
}

void State2B_Jump::Late_Update(Float timeDelta)
{
	State2B::Late_Update(timeDelta);
}

void State2B_Jump::StateExitInvoke()
{
	State2B::StateExitInvoke();
}

Shared<State2B_Jump> State2B_Jump::Create(const wstring& tag, const Shared<Pl0000>& owner)
{
	auto instance = make_shared<State2B_Jump>(tag, owner);

	if (FAILED(instance->Initialize()))
	{
		LOG_ERROR(L"Failed to Create State2B_Jump");
		return nullptr;
	}

	return instance;
}


