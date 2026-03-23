#pragma once
#include "State2B.h"

NS_BEGIN(Client)

class P10000Body;

class CLIENT_DLL State2B_Idle final : public State2B
{
public:
	explicit State2B_Idle(const wstring& tag, const Shared<P10000Body>& owner);
	~State2B_Idle() override = default;

private:
	HRESULT Initialize();

public:
	Bool StateEnterInvoke() override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void StateExitInvoke() override;

public:
	static Shared<State2B_Idle> Create(const wstring& tag, const Shared<P10000Body>& owner);
};

NS_END