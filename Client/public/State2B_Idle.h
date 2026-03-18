#pragma once
#include "State2B.h"

NS_BEGIN(Client)

class P10000;

class State2B_Idle final : public State2B
{
	RTTR_ENABLE(State2B)
public:
	explicit State2B_Idle(const wstring& tag, const Shared<P10000>& owner);
	~State2B_Idle() override = default;

public:
	Bool StateEnterInvoke() override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void StateExitInvoke() override;


};

NS_END