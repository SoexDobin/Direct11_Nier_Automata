#pragma once
#include "State2B.h"

NS_BEGIN(Client)

class P10000;

class CLIENT_DLL State2B_Attack final : public State2B
{
public:
	explicit State2B_Attack(const wstring& tag, const Shared<P10000>& owner);
	~State2B_Attack() override = default;

private:
	HRESULT Initialize();

public:
	Bool StateEnterInvoke() override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void StateExitInvoke() override;

public:
	static Shared<State2B_Attack> Create(const wstring& tag, const Shared<P10000>& owner);
};

NS_END

