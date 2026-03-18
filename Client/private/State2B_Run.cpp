#include "pch.h"
#include "State2B_Run.h"

State2B_Run::State2B_Run(const wstring& tag, const Shared<P10000> owner)
	: State2B{tag, owner}
{
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
