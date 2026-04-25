#pragma once
#include "StateEm3000.h"


NS_BEGIN(Client)

class CLIENT_DLL StateEm3000_Idle final : public StateEm3000
{
public:
	explicit StateEm3000_Idle(const wstring& tag, const Shared<Em3000>& owner);
	~StateEm3000_Idle() override = default;

private:
	HRESULT Initialize() override;

public:
	Bool StateEnterInvoke() override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void StateExitInvoke() override;

private:
	Bool m_InitializeState{ true };
	unordered_set<uint32> m_EnterAnim;

	Float m_delayAcc{};

public:
	static Shared<StateEm3000_Idle> Create(const wstring& tag, const Shared<Em3000>& owner);
};



NS_END
