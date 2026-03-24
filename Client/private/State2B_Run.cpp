#include "pch.h"
#include "State2B_Run.h"

#include <SpdLogger.h>

State2B_Run::State2B_Run(const wstring& tag, const Shared<P10000>& owner)
	: State2B{tag, owner}
{
}

HRESULT State2B_Run::Initialize()
{
	return S_OK;
}

Bool State2B_Run::StateEnterInvoke()
{
	return State2B::StateEnterInvoke();
}

void State2B_Run::Update(Float timeDelta)
{
	State2B::Update(timeDelta);
}

void State2B_Run::Late_Update(Float timeDelta)
{
	State2B::Late_Update(timeDelta);
}

void State2B_Run::StateExitInvoke()
{
	State2B::StateExitInvoke();
}

Shared<State2B_Run> State2B_Run::Create(const wstring& tag, const Shared<P10000>& owner)
{
	auto instance = make_shared<State2B_Run>(tag, owner);

	if (FAILED(instance->Initialize()))
	{
		LOG_ERROR(L"Failed to Create State2B_Run");
		return nullptr;
	}

	return instance;
}
