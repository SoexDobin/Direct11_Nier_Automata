#pragma once
#include "State2B.h"

NS_BEGIN(Client)

class Pl0000;

class CLIENT_DLL State2B_AttackGround final : public State2B
{
public:
	explicit State2B_AttackGround(const wstring& tag, const Shared<Pl0000>& owner);
	~State2B_AttackGround() override = default;

private:
	HRESULT Initialize() override;

public:
	Bool StateEnterInvoke() override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void StateExitInvoke() override;

private:
	unordered_set<uint32> m_EnterAnim;

public:
	static Shared<State2B_AttackGround> Create(const wstring& tag, const Shared<Pl0000>& owner);
};

NS_END

