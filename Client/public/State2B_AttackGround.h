#pragma once
#include "Pl0000.h"
#include "WP0070Body.h"
#include "WP0220Body.h"
#include "State2B.h"

NS_BEGIN(Client)
	class Pl0000;

class CLIENT_DLL State2B_AttackGround final : public State2B
{
public:
	enum class COMBO_TYPE { LIGHT, HEAVY, LIGHT_HEAVY, END};
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
	void Execute_Attack();
	void Set_AnimationExitProgress();
	void SpawnGhost();

private:
	uint32 m_ComboStep{};
	COMBO_TYPE m_PrevComboType{};
	Float m_ComboDelta{};
	uint32 m_LastOrderedAnimIndex{ };

	unordered_map<uint32, Float> m_CanComboProgress;
	unordered_map<uint32, Float> m_CanExitProgress;
	unordered_set<uint32> m_EnterAnim;

	Bool m_IsHeavyCharge{};
	Bool m_IsChargeEnd{};
	Float m_HeavyChargeDelta{};

	const uint32 LIGHT_BODY[7] = {
		ETOI(Pl0000::PL0000_STATE::LIGHT_GROUND1),
		ETOI(Pl0000::PL0000_STATE::LIGHT_GROUND2),
		ETOI(Pl0000::PL0000_STATE::LIGHT_GROUND3),
		ETOI(Pl0000::PL0000_STATE::LIGHT_GROUND4),
		ETOI(Pl0000::PL0000_STATE::LIGHT_GROUND5),
		ETOI(Pl0000::PL0000_STATE::LIGHT_GROUND6),
		ETOI(Pl0000::PL0000_STATE::LIGHT_GROUND7),
	};
	const uint32 LIGHT_WP[7] = {
		ETOI(WP0070Body::WP0070_STATE::LIGHT_GROUND1),
		ETOI(WP0070Body::WP0070_STATE::LIGHT_GROUND2),
		ETOI(WP0070Body::WP0070_STATE::LIGHT_GROUND3),
		ETOI(WP0070Body::WP0070_STATE::LIGHT_GROUND4),
		ETOI(WP0070Body::WP0070_STATE::LIGHT_GROUND5),
		ETOI(WP0070Body::WP0070_STATE::LIGHT_GROUND6),
		ETOI(WP0070Body::WP0070_STATE::LIGHT_GROUND7),
	};

	const uint32 HEAVY_BODY[3] = {
		ETOI(Pl0000::PL0000_STATE::HEAVY_GROUND1),
		ETOI(Pl0000::PL0000_STATE::HEAVY_GROUND2),
		ETOI(Pl0000::PL0000_STATE::HEAVY_GROUND3),
	};
	const uint32 HEAVY_WP[3] = {
		ETOI(WP0220Body::WP0220_STATE::HEAVY_GROUND1),
		ETOI(WP0220Body::WP0220_STATE::HEAVY_GROUND2),
		ETOI(WP0220Body::WP0220_STATE::HEAVY_GROUND3),
	};


public:
	static Shared<State2B_AttackGround> Create(const wstring& tag, const Shared<Pl0000>& owner);
};

NS_END

