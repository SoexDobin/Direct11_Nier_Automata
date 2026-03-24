#pragma once
#include "State2B.h"

NS_BEGIN(Client)

class P10000;

class CLIENT_DLL State2B_Jump final : public State2B
{
public:
	explicit State2B_Jump(const wstring& tag, const Shared<P10000>& owner);
	~State2B_Jump() override = default;

private:
	HRESULT Initialize();

public:
	Bool StateEnterInvoke() override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void StateExitInvoke() override;

private:

public:
	static Shared<State2B_Jump> Create(const wstring& tag, const Shared<P10000>& owner);
};

NS_END