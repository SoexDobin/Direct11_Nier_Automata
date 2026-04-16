#pragma once
#include "StateEm0010.h"

NS_BEGIN(Client)

class CLIENT_DLL StateEm0010_Idle final : public StateEm0010
{
public:
	explicit StateEm0010_Idle(const wstring& tag, const Shared<Em0010>& owner);
	~StateEm0010_Idle() override = default;

private:
	HRESULT Initialize() override;

public:
	Bool StateEnterInvoke() override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void StateExitInvoke() override;

private:
	void Update_Wander(Float timeDelta);

private:
	Bool m_InitializeState{ true };
	unordered_set<uint32> m_EnterAnim;

	Vector3 m_RootPosition{};
	Float m_WanderTimer{};

public:
	static Shared<StateEm0010_Idle> Create(const wstring& tag, const Shared<Em0010>& owner);
};

NS_END