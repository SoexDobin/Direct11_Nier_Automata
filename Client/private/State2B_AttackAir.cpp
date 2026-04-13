#include "pch.h"
#include "State2B_AttackAir.h"

#include <algorithm>

#include "Pl0000Movement.h"
#include "Pl0000StateMachine.h"
#include "WP0070Body.h"
#include "WP0220Body.h"
#include "SpdLogger.h"

State2B_AttackAir::State2B_AttackAir(const wstring& tag, const Shared<Pl0000>& owner)
	: State2B{ tag, owner }
{
}

HRESULT State2B_AttackAir::Initialize()
{
	m_EnterAnim.emplace(ETOI(Pl0000::PL0000_STATE::HEAVY_AIR_DOWN_ENTER));
	m_EnterAnim.emplace(ETOI(Pl0000::PL0000_STATE::LIGHT_AIR_DOWN_ENTER));

	m_EndAnim.emplace(ETOI(Pl0000::PL0000_STATE::HEAVY_AIR_DOWN_END));
	m_EndAnim.emplace(ETOI(Pl0000::PL0000_STATE::LIGHT_AIR_DOWN_END));

	return State2B::Initialize();
}

Bool State2B_AttackAir::StateEnterInvoke()
{
	auto pl0000 = m_Owner.lock();
	auto pl0000Body = m_Body.lock();
	auto prevState = m_States.lock()->Get_CurP10000State();
	if (prevState != Pl0000::PL0000_STATE::JUMP)
	{
		LOG_ERROR(L"[ ENTER BY WRONG STATE, PREV State is not JUMP! it's {} ]", Helper::To_wString(magic_enum::enum_name(prevState)));
		return false;
	}

	if (false == HasTarget())
	{
		if (auto target = pl0000->Get_ClosestTarget())
			pl0000->Set_LockOnTarget(target);
	}
	
	m_Movement.lock()->Reset_RootMotionStop();
	m_Movement.lock()->Set_Gravity(80.f);
	m_Movement.lock()->Add_Force(Vector3{ 0.f, 5.f, 0.f });

	pl0000Body->Set_Animation(ETOI(Pl0000::PL0000_STATE::HEAVY_AIR_DOWN_ENTER), 0.2f, false);
	m_LightWeapon.lock()->Set_Sheathing(m_Owner.lock()->Get_LightSheathingMatrix());
	m_HeavyWeapon.lock()->DrawWP0220();
	m_HeavyWeapon.lock()->Set_Animation(ETOI(WP0220Body::WP0220_STATE::HEAVY_AIR_DOWN_ENTER), 0.2f, false);

	return true;
}

void State2B_AttackAir::Update(Float timeDelta)
{
	auto pl0000 = m_Owner.lock();
	auto pl0000Body = m_Body.lock();
	auto target = pl0000->Get_LockOnTarget();
	auto animIndex = pl0000Body->Get_CurrentAnimationIndex();
	auto isFinished = pl0000Body->Is_AnimationFinished();
	Float progress = pl0000Body->Get_AnimationProgress();

	Pl0000Movement::PL0000_MOVEMENT_DATA movementData{};
	movementData.isMove = false;
	movementData.isJump = true;
	movementData.isAttack = true;
	movementData.canRotation = false;
	m_Movement.lock()->Set_MovementData(movementData);

	// 만약 땅에 먼저 닿으면 LIGHT, HEAVY END 애니메이션 되면서 끝
	if (m_Movement.lock()->Is_Grounded())
	{
		if (false == m_EndAnim.contains(animIndex))
		{
			if (animIndex == ETOI(Pl0000::PL0000_STATE::HEAVY_AIR_DOWN_HOLD) || animIndex == ETOI(Pl0000::PL0000_STATE::HEAVY_AIR_DOWN_ENTER))
			{
				pl0000Body->Set_Animation(ETOI(Pl0000::PL0000_STATE::HEAVY_AIR_DOWN_END), 0.05f, false);
				m_HeavyWeapon.lock()->Set_Animation(ETOI(WP0220Body::WP0220_STATE::HEAVY_AIR_DOWN_END), 0.05f, false);
			}
			else if (animIndex == ETOI(Pl0000::PL0000_STATE::LIGHT_AIR_DOWN_HOLD) || animIndex == ETOI(Pl0000::PL0000_STATE::LIGHT_AIR_DOWN_ENTER))
			{
				pl0000Body->Set_Animation(ETOI(Pl0000::PL0000_STATE::LIGHT_AIR_DOWN_END), 0.05f, false);
				m_LightWeapon.lock()->Set_Animation(ETOI(WP0070Body::WP0070_STATE::LIGHT_AIR_DOWN_END), 0.05f, false);
			}

			return;
		}

		if (false == m_EndAnim.contains(animIndex) || progress <= 0.1f) return;

		if (m_Input.lock()->Is_MousePress(DIMB::LBUTTON) || m_Input.lock()->Is_MousePress(DIMB::RBUTTON))
		{
			m_States.lock()->Change_State(Pl0000::PL0000_STATE::ATTACK_GROUND);
			return;
		}

		if (m_Input.lock()->Is_WASD_SingleClickHold(0.5f) || m_Input.lock()->Is_WASD_Press())
		{
			m_States.lock()->Change_State(Pl0000::PL0000_STATE::RUN);
			return;
		}

		if (m_EndAnim.contains(animIndex) && isFinished)
		{
			m_States.lock()->Change_State(Pl0000::PL0000_STATE::IDLE);
			return;
		}
	}
	else
	{
		if (!target.expired() && !target.lock()->Is_Destroy() && target.lock()->Is_Active())
		{
			Vector3 pos = pl0000->Get_Transform()->Get_Position();
			Vector3 targetPos = target.lock()->Get_Transform()->Get_Position();
			Vector3 lookPos = targetPos;
			lookPos.y = pos.y;
			pl0000->Get_Transform()->LookAt(lookPos);


			Vector3 dirToTarget = targetPos - pos;
			dirToTarget.y = 0.f;
			Float distance = dirToTarget.Length();

			if (distance > 1.5f)
			{
				dirToTarget.Normalize();
				Float homingSpeed = 30.f;

				m_Movement.lock()->Add_Correction(dirToTarget * homingSpeed * timeDelta);
			}
		}
		else
		{
			Vector3 myLook = pl0000->Get_Transform()->Get_Look();
			myLook.y = 0.f;
			if (myLook.Length() > 0.001f) myLook.Normalize();
			Float dashSpeed = 8.f;
			m_Movement.lock()->Add_Correction(myLook * dashSpeed * timeDelta);
		}
	}


	if (m_EnterAnim.contains(animIndex) && isFinished)
	{
		if (animIndex == ETOI(Pl0000::PL0000_STATE::HEAVY_AIR_DOWN_ENTER))
		{
			pl0000Body->Set_Animation(ETOI(Pl0000::PL0000_STATE::HEAVY_AIR_DOWN_HOLD), 0.1f, true);
			m_HeavyWeapon.lock()->Set_Animation(ETOI(WP0220Body::WP0220_STATE::HEAVY_AIR_DOWN_HOLD), 0.1f, true);
		}
			
		else
		{
			//pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::LIGHT_AIR_DOWN_HOLD), 0.2f, true);
			//m_LightWeapon.lock()->Set_Animation(ETOI(WP0070Body::WP0070_STATE::LIGHT_AIR_DOWN_HOLD), 0.2f, true);
		}
	}
}

void State2B_AttackAir::Late_Update(Float timeDelta)
{
	
}

void State2B_AttackAir::StateExitInvoke()
{
	m_LightWeapon.lock()->Set_Sheathing(m_Owner.lock()->Get_LightSheathingMatrix());
	m_HeavyWeapon.lock()->Set_Sheathing(m_Owner.lock()->Get_HeavySheathingMatrix());
	m_Movement.lock()->Reset_RootMotionStop();
	m_Movement.lock()->Set_Gravity(30.f);
}

Shared<State2B_AttackAir> State2B_AttackAir::Create(const wstring& tag, const Shared<Pl0000>& owner)
{
	auto instance = make_shared<State2B_AttackAir>(tag, owner);

	if (FAILED(instance->Initialize()))
	{
		MSG_BOX("Failed to Created : State2B_AttackAir");
		return nullptr;
	}

	return instance;
}
