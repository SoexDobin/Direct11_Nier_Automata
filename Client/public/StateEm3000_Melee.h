#pragma once
#include "StateEm3000.h"

NS_BEGIN(Client)

class CLIENT_DLL StateEm3000_Melee final : public StateEm3000
{
public:
	explicit StateEm3000_Melee(const wstring& tag, const Shared<Em3000>& owner);
	~StateEm3000_Melee() override = default;

private:
	HRESULT Initialize() override;

public:
	Bool StateEnterInvoke() override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void StateExitInvoke() override;

private:
	unordered_set<uint32> m_EnterAnim;
	unordered_set<uint32> m_AttackAnim;
	unordered_set<uint32> m_EndAnim;

public:
	static Shared<StateEm3000_Melee> Create(const wstring& tag, const Shared<Em3000>& owner);
};



NS_END
