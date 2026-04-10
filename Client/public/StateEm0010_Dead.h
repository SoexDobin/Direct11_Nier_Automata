#pragma once
#include "StateEm0010.h"

NS_BEGIN(Client)

class CLIENT_DLL StateEm0010_Dead final : public StateEm0010
{
public:
	explicit StateEm0010_Dead(const wstring& tag, const Shared<Em0010>& owner);
	~StateEm0010_Dead() override;

private:
	HRESULT Initialize() override;

public:
	Bool StateEnterInvoke() override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void StateExitInvoke() override;

private:
	unordered_set<uint32> m_DeadAnim;

public:
	static Shared<StateEm0010_Dead> Create(const wstring& tag, const Shared<Em0010>& owner);
};

NS_END