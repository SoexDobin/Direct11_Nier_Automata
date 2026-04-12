#include "pch.h"
#include "StateEm0010_Hit.h"

#include <SpdLogger.h>

#include "Em0010.h"
#include "Em0010Body.h"
#include "Em0010Movement.h"
#include "MonsterStateMachine.h"
#include "Random_Helper.h"

StateEm0010_Hit::StateEm0010_Hit(const wstring& tag, const Shared<Em0010>& owner)
	: StateEm0010{tag, owner} {}
StateEm0010_Hit::~StateEm0010_Hit() {}


HRESULT StateEm0010_Hit::Initialize()
{

	return StateEm0010::Initialize();
}


Bool StateEm0010_Hit::StateEnterInvoke()
{
	if (m_Owner.lock()->Is_Dead()) return false;
	using msState = MonsterStateMachine::MONSTER_STATE;
	using em0010Anim = Em0010::EM0010_STATE;

	uint32 randIndex{};
	if (Is_BackAttack())
	{
		randIndex = Helper::Random_Int(ETOI(em0010Anim::HIT_BACK_1), ETOI(em0010Anim::HIT_BACK_3));
	}
	else
	{
		randIndex = Helper::Random_Int(ETOI(em0010Anim::HIT_FRONT_1), ETOI(em0010Anim::HIT_FRONT_3));
	}

	m_Body.lock()->Set_Animation(randIndex, 0.2f, false);

	return true;
}

void StateEm0010_Hit::Update(Float timeDelta)
{
	auto em0010 = m_Body.lock();
	Bool isFinished = em0010->Is_AnimationFinished();

	const Entity::DAMAGE_INFO& dmgInfo = m_Owner.lock()->Get_LastDamageInfo();
	Em0010Movement::EM0010_MOVEMENT_DATA moveData{};
	moveData.isMove = false;
	moveData.isAttack = true;         // 루트모션 넉백 위치 적용
	moveData.canRotation = false;
	moveData.useRootMotionDir = true;
	moveData.rootMotionScale = dmgInfo.knockbackForce;
	m_Movement.lock()->Set_MovementData(moveData);

	if (isFinished)
	{
		if (m_Owner.lock()->Is_Dead())
		{
			m_States.lock()->Change_State(MonsterStateMachine::MONSTER_STATE::DEAD);
			return;
		}

		if (Has_Target())
		{
			m_States.lock()->Change_State(MonsterStateMachine::MONSTER_STATE::CHASE);
		}
		else
		{
			m_States.lock()->Change_State(MonsterStateMachine::MONSTER_STATE::IDLE);
		}
	}
}

void StateEm0010_Hit::Late_Update(Float timeDelta)
{
	
}

void StateEm0010_Hit::StateExitInvoke()
{
		
}

Bool StateEm0010_Hit::Is_BackAttack()
{
	auto& dmgInfo = m_Owner.lock()->Get_LastDamageInfo();
	Vector3 monsterLook = m_Owner.lock()->Get_Transform()->Get_Look();
	Vector3 toAttackPos = dmgInfo.hitPosition - m_Owner.lock()->Get_Transform()->Get_Position();
	
	Float scalar = monsterLook.Dot(toAttackPos);
	return scalar < 0.f;
}

Shared<StateEm0010_Hit> StateEm0010_Hit::Create(const wstring& tag, const Shared<Em0010>& owner)
{
	auto instance = make_shared<StateEm0010_Hit>(tag, owner);

	if (FAILED(instance->Initialize()))
	{
		MSG_BOX("Failed to Clone : StateEm0010_Hit");
		return nullptr;
	}

	return instance;
}

