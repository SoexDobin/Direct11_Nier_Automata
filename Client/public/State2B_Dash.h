#pragma once
#include "State2B.h"

NS_BEGIN(Client)

class Pl0000;

class CLIENT_DLL State2B_Dash final : public State2B
{
private:
	enum class DASH_DIR { FRONT, BACK, RIGHT, LEFT, NONE };

public:
	explicit State2B_Dash(const wstring& tag, const Shared<Pl0000>& owner);
	~State2B_Dash() override = default;

private:
	HRESULT Initialize() override;

public:
	Bool StateEnterInvoke() override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void StateExitInvoke() override;

private:
	unordered_set<uint32> m_EnterAnim;
	unordered_set<uint32> m_DashAnim;

	DASH_DIR m_DashDir{ DASH_DIR::NONE };
	Float m_OriginalFov{ 0.f };

public:
	static Shared<State2B_Dash> Create(const wstring& tag, const Shared<Pl0000>& owner);
};

NS_END

