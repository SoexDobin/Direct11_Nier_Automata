#include "pch.h"
#include "State2B_AttackGround.h"

#include <SpdLogger.h>

#include "Pl0000.h"
#include "Pl0000StateMachine.h"
#include "WP0070Body.h"
#include "WP0220Body.h"

State2B_AttackGround::State2B_AttackGround(const wstring& tag, const Shared<Pl0000>& owner)
	: State2B{tag, owner}
{
}

HRESULT State2B_AttackGround::Initialize()
{
	Set_AnimationExitProgress();

	m_EnterAnim.emplace(ETOI(Pl0000::PL0000_STATE::LIGHT_GROUND_RUN));
	m_EnterAnim.emplace(ETOI(Pl0000::PL0000_STATE::HEAVY_GROUND_SPRINT));

	return State2B::Initialize();
}

Bool State2B_AttackGround::StateEnterInvoke()
{
	auto pl0000 = m_Body.lock();
	auto lightWeapon = m_LightWeapon.lock();
	auto heavyWeapon = m_HeavyWeapon.lock();
	auto prevState = m_States.lock()->Get_CurP10000State();
	
	m_ComboDelta = 0.f;
	m_ComboStep = 0;
	Bool clickLeft = m_Input.lock()->Is_MousePress(DIMB::LBUTTON) || m_Input.lock()->Is_MouseDown(DIMB::LBUTTON);

	switch (prevState)
	{
	case Pl0000::PL0000_STATE::IDLE:
		if (clickLeft)
		{
			lightWeapon->DrawWP0070();
			heavyWeapon->Set_Sheathing(m_Owner.lock()->Get_HeavySheathingMatrix());
			pl0000->Set_Animation(LIGHT_BODY[m_ComboStep], 0.15f, false);
			lightWeapon->Set_Animation(LIGHT_WP[m_ComboStep], 0.15f, false);

			++m_ComboStep;
			m_PrevComboType = COMBO_TYPE::LIGHT;
		}
		else
		{
			lightWeapon->Set_Sheathing(m_Owner.lock()->Get_LightSheathingMatrix());
			heavyWeapon->DrawWP0220();
			pl0000->Set_Animation(HEAVY_BODY[m_ComboStep], 0.15f, false);
			heavyWeapon->Set_Animation(HEAVY_WP[m_ComboStep], 0.15f, false);

			++m_ComboStep;
			m_PrevComboType = COMBO_TYPE::HEAVY;
		}
		return true;
	case Pl0000::PL0000_STATE::RUN: case Pl0000::PL0000_STATE::SPRINT: case Pl0000::PL0000_STATE::DASH:
		if (clickLeft)
		{
			lightWeapon->DrawWP0070();
			heavyWeapon->Set_Sheathing(m_Owner.lock()->Get_HeavySheathingMatrix());
			pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::LIGHT_GROUND_RUN), 0.15f, false);
			lightWeapon->Set_Animation(ETOI(WP0070Body::WP0070_STATE::LIGHT_GROUND_RUN), 0.15f, false);

			++m_ComboStep;
			m_PrevComboType = COMBO_TYPE::LIGHT;
		}
		else
		{
			lightWeapon->Set_Sheathing(m_Owner.lock()->Get_LightSheathingMatrix());
			heavyWeapon->DrawWP0220();
			pl0000->Set_Animation(HEAVY_BODY[m_ComboStep], 0.15f, false);
			heavyWeapon->Set_Animation(HEAVY_WP[m_ComboStep], 0.15f, false);

			m_ComboStep++;
			m_PrevComboType = COMBO_TYPE::HEAVY;
		}
		return true;
	default:
		LOG_ERROR(L"[ENTER ATTACK GROUND] : No Enter state PrevIndex {} ", ETOI(prevState));
		return false;
	}
}

void State2B_AttackGround::Update(Float timeDelta)
{
	auto pl0000 = m_Body.lock();
	auto input = m_Input.lock();
	Bool isAnimFinished = pl0000->Is_AnimationFinished();

	uint32 animIndex = pl0000->Get_CurrentAnimationIndex();

	// 항상 ATTACK보다 DASH 판정해서 EVADE로 빼기
	if (input->Is_WASD_DoubleClick())
	{
		m_States.lock()->Change_State(Pl0000::PL0000_STATE::DASH);
		return;
	}

	if (!m_EnterAnim.contains(animIndex))
		Execute_Attack();

	if (input->Is_MousePress(DIMB::LBUTTON) ||
		input->Is_MousePress(DIMB::RBUTTON) ||
		input->Is_MouseHold(DIMB::LBUTTON, 0.2f) ||
		input->Is_MouseHold(DIMB::RBUTTON, 0.2f) ||
		m_IsHeavyCharge)
	{
		return; // 이하 RUN 등 탈출 로직 완전 무시
	}

	// Attack 빠져나가기
	if (m_CanExitProgress[animIndex] >= pl0000->Get_AnimationProgress()) return;

	if (input->Is_WASD_Press())
	{
		m_States.lock()->Change_State(Pl0000::PL0000_STATE::RUN);
		return;
	}

	if (false == isAnimFinished) return;
	m_States.lock()->Change_State(Pl0000::PL0000_STATE::IDLE);
}

void State2B_AttackGround::Late_Update(Float timeDelta)
{
	m_ComboDelta += timeDelta;
	if (m_ComboDelta >= 2.0f)
	{
		m_ComboDelta = 0.f;
		m_ComboStep = 0;
	}

	if (m_IsHeavyCharge)
		m_HeavyChargeDelta += timeDelta;
}

void State2B_AttackGround::StateExitInvoke()
{
	if (auto lightWp = m_LightWeapon.lock())
	{
		lightWp->Set_Sheathing(m_Owner.lock()->Get_LightSheathingMatrix());
	}
		
	if (auto heavyWp = m_HeavyWeapon.lock())
	{
		heavyWp->Set_Sheathing(m_Owner.lock()->Get_HeavySheathingMatrix());
	}
		
}

void State2B_AttackGround::Execute_Attack()
{
	auto pl0000 = m_Body.lock();
	auto input = m_Input.lock();
	auto lightWeapon = m_LightWeapon.lock();
	auto heavyWeapon = m_HeavyWeapon.lock();

	uint32 animIndex = pl0000->Get_CurrentAnimationIndex();
	Bool isExitProgress = pl0000->Get_AnimationProgress() >= m_CanComboProgress[animIndex];
	Float blendDuration = 0.15f;

	if (false == isExitProgress) return;

	if (m_IsHeavyCharge)
	{
		if (input->Is_MouseUp(DIMB::RBUTTON))
		{
			if (m_HeavyChargeDelta >= 2.f)
			{
				pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::HEAVY_GROUND_HOLD_NO_CONTACT), blendDuration, false);
				heavyWeapon->Set_Animation(ETOI(WP0220Body::WP0220_STATE::HEAVY_GROUND_HOLD_NO_CONTACT), blendDuration, false);
			}
			else
			{
				pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::HEAVY_GROUND_HOLD_UNFULL), blendDuration, false);
				heavyWeapon->Set_Animation(ETOI(WP0220Body::WP0220_STATE::HEAVY_GROUND_HOLD_UNFULL), blendDuration, false);
			}
			m_ComboStep = 0;
			m_PrevComboType = COMBO_TYPE::HEAVY;
			m_IsHeavyCharge = false;
		}
		return; // 단일 패턴이므로 돌아감
	}

	if (false == m_IsHeavyCharge && input->Is_MousePress(DIMB::LBUTTON))
	{
		m_ComboDelta = 0.f;
		if (m_PrevComboType == COMBO_TYPE::LIGHT)
		{
			Bool isNotLightRootCombo = m_ComboStep != 0;

			if (false == isNotLightRootCombo)
			{
				pl0000->Set_Animation(LIGHT_BODY[m_ComboStep], blendDuration, false);
				if (lightWeapon->Is_Sheathing())
					lightWeapon->DrawWP0070();
				lightWeapon->Set_Animation(LIGHT_WP[m_ComboStep], blendDuration, false);
				heavyWeapon->Set_Sheathing(m_Owner.lock()->Get_HeavySheathingMatrix());

				++m_ComboStep;
				m_PrevComboType = COMBO_TYPE::LIGHT;
				return; // 루트 평타는 바로 재끼기
			}

			
			if (isNotLightRootCombo &&
				input->Is_MousePress(DIMB::RBUTTON))
			{
				pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::LIGHT_HEAVY_COMBO), blendDuration, false);
				lightWeapon->DrawWP0070();
				lightWeapon->Set_Animation(ETOI(WP0070Body::WP0070_STATE::LIGHT_COMBO), blendDuration, false);
				heavyWeapon->DrawWP0220();
				heavyWeapon->Set_Animation(ETOI(WP0220Body::WP0220_STATE::HEAVY_COMBO), blendDuration, false);

				m_ComboStep = 0;
				m_PrevComboType = COMBO_TYPE::LIGHT_HEAVY;
			}
			else if (animIndex != ETOI(Pl0000::PL0000_STATE::LIGHT_GROUND_HOLD) &&
				isNotLightRootCombo && 
				input->Is_MouseHold(DIMB::LBUTTON, 0.3f))
			{
				pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::LIGHT_GROUND_HOLD), blendDuration, false);
				lightWeapon->Set_Animation(ETOI(WP0070Body::WP0070_STATE::LIGHT_GROUND_HOLD), blendDuration, false);
				lightWeapon->DrawWP0070();

				m_ComboStep = 0;
				m_PrevComboType = COMBO_TYPE::LIGHT;
			}
			else if (input->Is_MousePress(DIMB::LBUTTON))
			{
				pl0000->Set_Animation(LIGHT_BODY[m_ComboStep], blendDuration, false);
				lightWeapon->Set_Animation(LIGHT_WP[m_ComboStep], blendDuration, false);
				lightWeapon->DrawWP0070();

				++m_ComboStep;
				m_PrevComboType = COMBO_TYPE::LIGHT;
			}
		}
		else
		{
			m_ComboStep = 0;

			pl0000->Set_Animation(LIGHT_BODY[m_ComboStep], blendDuration, false);
			lightWeapon->Set_Animation(LIGHT_WP[m_ComboStep], blendDuration, false);
			heavyWeapon->Set_Sheathing(m_Owner.lock()->Get_HeavySheathingMatrix());
			lightWeapon->DrawWP0070();

			m_PrevComboType = COMBO_TYPE::LIGHT;
			++m_ComboStep;
		}
	}
	else if (input->Is_MousePress(DIMB::RBUTTON))
	{
		m_ComboDelta = 0.f;
		if (m_PrevComboType == COMBO_TYPE::HEAVY)
		{
			Bool isNotLightRootCombo = m_ComboStep != 0;

			if (m_IsHeavyCharge) return;

			if (false == isNotLightRootCombo)
			{
				pl0000->Set_Animation(HEAVY_BODY[m_ComboStep], blendDuration, false);
				if (heavyWeapon->Is_Sheathing())
					heavyWeapon->DrawWP0220();
				lightWeapon->Set_Sheathing(m_Owner.lock()->Get_LightSheathingMatrix());
				heavyWeapon->Set_Animation(HEAVY_WP[m_ComboStep], blendDuration, false);

				++m_ComboStep;
				m_PrevComboType = COMBO_TYPE::HEAVY;
				return; // 루트 평타는 바로 재끼기
			}

			//if (animIndex != ETOI(Pl0000::PL0000_STATE::HEAVY_GROUND_HOLD_CYCLE) &&
			//	input->Is_MouseHold(DIMB::RBUTTON, 0.25f))
			//{
			//	m_IsHeavyCharge = true;
			//	m_HeavyChargeDelta = 0.f;
			//	pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::HEAVY_GROUND_HOLD_CYCLE), 0.3f, true);
			//	lightWeapon->Set_Sheathing(m_Owner.lock()->Get_LightSheathingMatrix());
			//	heavyWeapon->DrawWP0220();
			//	heavyWeapon->Set_Animation(ETOI(WP0220Body::WP0220_STATE::HEAVY_GROUND_HOLD_CYCLE), 0.3f, true);
			//
			//	m_ComboStep = 0;
			//	m_PrevComboType = COMBO_TYPE::HEAVY;
			//}
			//else 
			if (input->Is_MousePress(DIMB::RBUTTON))
			{
				pl0000->Set_Animation(HEAVY_BODY[m_ComboStep], blendDuration, false);
				lightWeapon->Set_Sheathing(m_Owner.lock()->Get_LightSheathingMatrix());
				heavyWeapon->DrawWP0220();
				heavyWeapon->Set_Animation(HEAVY_WP[m_ComboStep], blendDuration, false);
				
				++m_ComboStep;
				m_PrevComboType = COMBO_TYPE::HEAVY;
			}
		}
		else
		{
			m_ComboStep = 0;

			pl0000->Set_Animation(HEAVY_BODY[m_ComboStep], blendDuration, false);
			lightWeapon->Set_Sheathing(m_Owner.lock()->Get_LightSheathingMatrix());
			heavyWeapon->Set_Animation(HEAVY_WP[m_ComboStep], blendDuration, false);
			heavyWeapon->DrawWP0220();

			m_PrevComboType = COMBO_TYPE::HEAVY;
			++m_ComboStep;
		}
	}

	

}

void State2B_AttackGround::Set_AnimationExitProgress()
{
	using pl = Pl0000::PL0000_STATE;
	m_CanComboProgress.emplace(ETOI(pl::LIGHT_GROUND1), 0.1f); m_CanExitProgress.emplace(ETOI(pl::LIGHT_GROUND1), 0.1f);
	m_CanComboProgress.emplace(ETOI(pl::LIGHT_GROUND2), 0.1f); m_CanExitProgress.emplace(ETOI(pl::LIGHT_GROUND2), 0.1f);
	m_CanComboProgress.emplace(ETOI(pl::LIGHT_GROUND3), 0.1f); m_CanExitProgress.emplace(ETOI(pl::LIGHT_GROUND3), 0.1f);
	m_CanComboProgress.emplace(ETOI(pl::LIGHT_GROUND4), 0.1f); m_CanExitProgress.emplace(ETOI(pl::LIGHT_GROUND4), 0.1f);
	m_CanComboProgress.emplace(ETOI(pl::LIGHT_GROUND5), 0.1f); m_CanExitProgress.emplace(ETOI(pl::LIGHT_GROUND5), 0.1f);
	m_CanComboProgress.emplace(ETOI(pl::LIGHT_GROUND6), 0.1f); m_CanExitProgress.emplace(ETOI(pl::LIGHT_GROUND6), 0.1f);
	m_CanComboProgress.emplace(ETOI(pl::LIGHT_GROUND7), 0.5f); m_CanExitProgress.emplace(ETOI(pl::LIGHT_GROUND7), 0.3f);

	m_CanComboProgress.emplace(ETOI(pl::LIGHT_GROUND_HOLD), 0.8f); m_CanExitProgress.emplace(ETOI(pl::LIGHT_GROUND_HOLD), 0.75f);
	m_CanComboProgress.emplace(ETOI(pl::LIGHT_GROUND_RUN), 0.25f); m_CanExitProgress.emplace(ETOI(pl::LIGHT_GROUND_RUN), 0.15f);

	m_CanComboProgress.emplace(ETOI(pl::HEAVY_GROUND1), 0.15f); m_CanExitProgress.emplace(ETOI(pl::HEAVY_GROUND1), 0.25f);
	m_CanComboProgress.emplace(ETOI(pl::HEAVY_GROUND2), 0.15f); m_CanExitProgress.emplace(ETOI(pl::HEAVY_GROUND2), 0.25f);
	m_CanComboProgress.emplace(ETOI(pl::HEAVY_GROUND3), 0.3f); m_CanExitProgress.emplace(ETOI(pl::HEAVY_GROUND3), 0.3f);

	m_CanComboProgress.emplace(ETOI(pl::HEAVY_GROUND_HOLD_CYCLE), 0.1f); m_CanExitProgress.emplace(ETOI(pl::HEAVY_GROUND_HOLD_CYCLE), 0.1f);
	m_CanComboProgress.emplace(ETOI(pl::HEAVY_GROUND_HOLD_UNFULL), 0.5f); m_CanExitProgress.emplace(ETOI(pl::HEAVY_GROUND_HOLD_UNFULL), 0.25f);
	m_CanComboProgress.emplace(ETOI(pl::HEAVY_GROUND_HOLD_NO_CONTACT), 0.5f); m_CanExitProgress.emplace(ETOI(pl::HEAVY_GROUND_HOLD_NO_CONTACT), 0.25f);

	m_CanComboProgress.emplace(ETOI(pl::LIGHT_HEAVY_COMBO), 0.75f);
}

Shared<State2B_AttackGround> State2B_AttackGround::Create(const wstring& tag, const Shared<Pl0000>& owner)
{
	auto instance = make_shared<State2B_AttackGround>(tag, owner);

	if (FAILED(instance->Initialize()))
	{
		LOG_ERROR(L"Failed to Create State2B_AttackGround");
		return nullptr;
	}

	return instance;
}

