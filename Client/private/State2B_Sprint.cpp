#include "pch.h"
#include "State2B_Sprint.h"

#include <SpdLogger.h>

State2B_Sprint::State2B_Sprint(const wstring& tag, const Shared<P10000Body>& owner)
	: State2B{tag, owner}
{
}

HRESULT State2B_Sprint::Initialize()
{
	return S_OK;
}

Bool State2B_Sprint::StateEnterInvoke()
{
	return State2B::StateEnterInvoke();
}

void State2B_Sprint::Update(Float timeDelta)
{
	State2B::Update(timeDelta);
}

void State2B_Sprint::Late_Update(Float timeDelta)
{
	State2B::Late_Update(timeDelta);
}

void State2B_Sprint::StateExitInvoke()
{
	State2B::StateExitInvoke();
}

Shared<State2B_Sprint> State2B_Sprint::Create(const wstring& tag, const Shared<P10000Body>& owner)
{
	auto instance = make_shared<State2B_Sprint>(tag, owner);

	if (FAILED(instance->Initialize()))
	{
		LOG_ERROR(L"Failed to Create State2B_Sprint");
		return nullptr;
	}

	return instance;
}

