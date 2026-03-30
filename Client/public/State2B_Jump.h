#pragma once
#include "Pl0000.h"
#include "State2B.h"

NS_BEGIN(Client)

class Pl0000;

class CLIENT_DLL State2B_Jump final : public State2B
{
public:
	explicit State2B_Jump(const wstring& tag, const Shared<Pl0000>& owner);
	~State2B_Jump() override = default;

private:
	HRESULT Initialize();

public:
	Bool StateEnterInvoke() override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void StateExitInvoke() override;

private:
	Pl0000::PL0000_STATE m_PrevMoveState{};
	Bool m_CanDoubleJump{ true };
	Float JumpScalar{};
	unordered_set<uint32> m_EnterAnim;

public:
	static Shared<State2B_Jump> Create(const wstring& tag, const Shared<Pl0000>& owner);
};

NS_END