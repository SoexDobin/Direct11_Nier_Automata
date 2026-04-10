#pragma once
#include "StateEm0010.h"

NS_BEGIN(Client)

class CLIENT_DLL StateEm0010_Attack final : public StateEm0010
{
public:
	explicit StateEm0010_Attack(const wstring& tag, const Shared<Em0010>& owner);
	~StateEm0010_Attack() override = default;

private:
	HRESULT Initialize() override;

public:
	Bool StateEnterInvoke() override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void StateExitInvoke() override;

private:
	void Em0010Attack(); 

private:
	unordered_set<uint32> m_EnterAnim;
	unordered_set<uint32> m_AttackAnim;

public:
	static Shared<StateEm0010_Attack> Create(const wstring& tag, const Shared<Em0010>& owner);
};

NS_END