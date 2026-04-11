#pragma once
#include "State2B.h"

NS_BEGIN(Client)

class Pl0000;

class CLIENT_DLL State2B_Evade final : public State2B
{
private:
	enum class DASH_DIR { FRONT, BACK, RIGHT, LEFT, NONE };
	enum class EVADE_PHASE { ENTER, DASH, EVADE, END };

public:
	explicit State2B_Evade(const wstring& tag, const Shared<Pl0000>& owner);
	~State2B_Evade() override = default;

private:
	HRESULT Initialize() override;

public:
	Bool StateEnterInvoke() override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void StateExitInvoke() override;

private:
	void PlayEvadeSFX(EVADE_PHASE evadePhase) const;

public:
	Bool TryEvade_FromDash();

private:
	unordered_set<uint32> m_EnterAnim;
	unordered_set<uint32> m_DashAnim;
	unordered_set<uint32> m_EvadeAnim;

	DASH_DIR m_DashDir{ DASH_DIR::NONE };
	Float m_OriginalFov{ 0.f };
	EVADE_PHASE m_EvadePhase{ EVADE_PHASE::ENTER };

public:
	static Shared<State2B_Evade> Create(const wstring& tag, const Shared<Pl0000>& owner);
};

NS_END

