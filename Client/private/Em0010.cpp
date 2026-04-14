#include "pch.h"
#include "Em0010.h"

#include <Game.h>
#include <Navigation.h>
#include <SpdLogger.h>
#include <SphereCollider.h>

#include "HpBarWorldUI.h"
#include "Em0010Body.h"
#include "Em0010Movement.h"
#include "MonsterAOE.h"
#include "MonsterSight.h"
#include "MonsterStateMachine.h"
#include "State2B_Run.h"
#include "StateEm0010_Attack.h"
#include "StateEm0010_Chase.h"
#include "StateEm0010_Dead.h"
#include "StateEm0010_Hit.h"
#include "StateEm0010_Idle.h"
#include "StateEm0010_Walk.h"

Em0010::Em0010() : Monster{} {}
Em0010::Em0010(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Monster{device, context} {}
Em0010::Em0010(const Em0010& rhs) 
	: Monster{rhs} {}

Em0010::~Em0010()
{
	m_MainBody.reset();
}

HRESULT Em0010::Initialize_Prototype()
{
	if (FAILED(Monster::Initialize_Prototype()))
	{
		LOG_ERROR(L"Failed to Create Prototype Monster");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT Em0010::Initialize(void* arg)
{
	if (FAILED(Monster::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize em0010");
		return E_FAIL;
	}

	if (FAILED(Ready_PartObjects()))
	{
		LOG_ERROR(L"Failed to Ready PartObjects em0010 ");
		return E_FAIL;
	}

	if (FAILED(Ready_Components()))
	{
		LOG_ERROR(L"Failed to Ready Components em0010 ");
		return E_FAIL;
	}

	GAME_INSTANCE->Add_Instance_Event(ETOI(LEVEL::GAMEPLAY), L"Add_HpBar", [this]()
		{
			HpBarWorldUI::HP_BAR_WORLD_UI_DESC UI_hpDesc{};
			UI_hpDesc.target = static_pointer_cast<Entity>(shared_from_this());
			UI_hpDesc.worldOffset = Vector3{ 0.f, 1.f, 0.f };
			UI_hpDesc.anchor = UI_ANCHOR::TOP_LEFT;
			UI_hpDesc.sizeX = 200.f;
			UI_hpDesc.sizeY = 10.f;
			UI_hpDesc.x = 0.f;
			UI_hpDesc.y = 0.f;
			m_HpBarUI = GAME_INSTANCE->Instantiate<HpBarWorldUI>(L"HpBarWorldUI", ETOI(LEVEL::GAMEPLAY), &UI_hpDesc);
		});


	return S_OK;
}

void Em0010::On_Destroy()
{
	m_MainBody.reset();
	Monster::On_Destroy();
}

void Em0010::Priority_Update(Float timeDelta)
{
	
}

void Em0010::Update(Float timeDelta)
{
	if (m_LagDuration > 0.f)
	{
		m_LagDuration -= timeDelta;
		timeDelta *= 0.05f;
	}

	m_States->Update_State(timeDelta);
	m_Em0010Movement->Update_Movement(timeDelta);
	m_Transform->Update_WorldMatrix();
	m_PhysicalZone->Update(m_Transform->Get_WorldMatrix());
}

void Em0010::Late_Update(Float timeDelta)
{

}

void Em0010::Fixed_Update(Float fixedDelta)
{

}

HRESULT Em0010::Render()
{
	return S_OK;
}

void Em0010::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

void Em0010::TakeDamage(const DAMAGE_INFO& dmgInfo)
{
	if (Is_Dead()) return;

	Play_HitSFX(dmgInfo);
	DisplaySparkEffect(dmgInfo.attackType, dmgInfo.hitPosition, dmgInfo.hitRotation);
		
	if (dmgInfo.attackType != ATK_TYPE::POD)
		m_States->Change_State(MonsterStateMachine::MONSTER_STATE::Hit);

	Monster::TakeDamage(dmgInfo);
}

void Em0010::OnDeath()
{
	m_MainBody->OffHitBox();
	m_States->Change_State(MonsterStateMachine::MONSTER_STATE::DEAD);
}

void Em0010::OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	
}

void Em0010::OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	Monster::OnCollisionStay(ownCollider, targetCollider);
}

void Em0010::OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{

}

void Em0010::OnAttackHit(const Shared<GameObject>& target)
{
	if (m_Em0010Movement)
	{
		m_Em0010Movement->Reduce_RootMotion();
	}
}

HRESULT Em0010::Ready_PartObjects()
{
	auto thisObject = static_pointer_cast<ContainerObject>(shared_from_this());

	Em0010Body::EM0010BODY_DESC em0010BodyDesc{};
	em0010BodyDesc.parentMatrix = m_Transform->Get_WorldMatrixPtr();
	em0010BodyDesc.Owner = thisObject;
	if (FAILED(Add_PartObject(ETOI(LEVEL::GAMEPLAY), L"Em0010Body", L"Em0010Body", &em0010BodyDesc)))
		return E_FAIL;

	MonsterSight::MONSTER_SIGHT_DESC em0010SightDesc{};
	em0010SightDesc.parentMatrix = m_Transform->Get_WorldMatrixPtr();
	em0010SightDesc.Owner = thisObject;
	em0010SightDesc.radius = 8.f;
	em0010SightDesc.offset = Vector3::Zero;
	if (FAILED(Add_PartObject(ETOI(LEVEL::GAMEPLAY), L"MonsterSight", L"Em0010Sight", &em0010SightDesc)))
		return E_FAIL;

	m_MainBody = static_pointer_cast<Em0010Body>(Find_PartObject(L"Em0010Body"));
	if (nullptr == m_MainBody)
		return E_FAIL;

	DAMAGE_INFO em0010DamageInfo{};
	em0010DamageInfo.attacker = shared_from_this();
	em0010DamageInfo.attackType = ATK_TYPE::HEAVY;
	em0010DamageInfo.damage = 10.f;
	em0010DamageInfo.groggyWeight = 10.f;
	em0010DamageInfo.knockbackForce = 1.f;

	MonsterAOE::MONSTER_AOE_DESC leftArmDesc{};
	leftArmDesc.parentMatrix = m_Transform->Get_WorldMatrixPtr();
	leftArmDesc.Owner = thisObject;
	leftArmDesc.model = m_MainBody->Get_ModelComponent();
	leftArmDesc.offset = Vector3::Zero;
	leftArmDesc.radius = 0.75f;
	leftArmDesc.targetBoneName = "bone304";
	leftArmDesc.dmgInfo = em0010DamageInfo;
	if (FAILED(Add_PartObject(ETOI(LEVEL::GAMEPLAY), L"MonsterAOE", L"Em0010LeftArm", &leftArmDesc)))
		return E_FAIL;
	
	MonsterAOE::MONSTER_AOE_DESC rightArmDesc{};
	rightArmDesc.parentMatrix = m_Transform->Get_WorldMatrixPtr();
	rightArmDesc.Owner = thisObject;
	rightArmDesc.model = m_MainBody->Get_ModelComponent();
	rightArmDesc.offset = Vector3::Zero;
	rightArmDesc.radius = 0.75f;
	rightArmDesc.targetBoneName = "bone560";
	rightArmDesc.dmgInfo = em0010DamageInfo;
	if (FAILED(Add_PartObject(ETOI(LEVEL::GAMEPLAY), L"MonsterAOE", L"Em0010RightArm", &rightArmDesc)))
		return E_FAIL;
	
	MonsterAOE::MONSTER_AOE_DESC footDesc{};
	footDesc.parentMatrix = m_Transform->Get_WorldMatrixPtr();
	footDesc.Owner = thisObject;
	footDesc.model = m_MainBody->Get_ModelComponent();
	footDesc.offset = Vector3::Zero;
	footDesc.radius = 2.5f;
	footDesc.targetBoneName = "bone4094";
	footDesc.dmgInfo = em0010DamageInfo;
	if (FAILED(Add_PartObject(ETOI(LEVEL::GAMEPLAY), L"MonsterAOE", L"Em0010Foot", &footDesc)))
		return E_FAIL;

	if (FAILED(m_MainBody->Begin()))
	{
		LOG_ERROR(L"Failed to Begin Main Body");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT Em0010::Ready_Components()
{
	Navigation::NAVIGATION_DESC navDesc;
	navDesc.startCellIndex = 0;
	m_Navigation = Add_Component_Tag<Navigation>(ETOI(LEVEL::STATIC), L"CityOfRuinEntry", &navDesc);

	Em0010Movement::EM0010_MOVEMENT_DESC movementDesc{};
	movementDesc.velocity = Vector3{ 0.f, 0.f, 0.f };
	movementDesc.moveSpeed = 0.f;
	movementDesc.targetDirection = Vector3::Zero;
	movementDesc.turnSpeed = 7.5f;
	m_Em0010Movement = Add_Component<Em0010Movement>(ETOI(LEVEL::GAMEPLAY), &movementDesc);
	if (nullptr == m_Em0010Movement)
		return E_FAIL;

	// StateMachine은 마지막에 처리
	if ((m_States = Add_Component<MonsterStateMachine>(ETOI(LEVEL::GAMEPLAY))))
	{
		auto em0010 = static_pointer_cast<Em0010>(shared_from_this());

		if (FAILED(m_States->Add_State(StateEm0010_Idle::Create(
		Helper::To_wString(magic_enum::enum_name(MonsterStateMachine::MONSTER_STATE::IDLE)), em0010))))
			return E_FAIL;
		if (FAILED(m_States->Add_State(StateEm0010_Hit::Create(
			Helper::To_wString(magic_enum::enum_name(MonsterStateMachine::MONSTER_STATE::Hit)), em0010))))
			return E_FAIL;
		if (FAILED(m_States->Add_State(StateEm0010_Chase::Create(
			Helper::To_wString(magic_enum::enum_name(MonsterStateMachine::MONSTER_STATE::CHASE)), em0010))))
			return E_FAIL;
		if (FAILED(m_States->Add_State(StateEm0010_Attack::Create(
			Helper::To_wString(magic_enum::enum_name(MonsterStateMachine::MONSTER_STATE::ATTACK)), em0010))))
			return E_FAIL;
		if (FAILED(m_States->Add_State(StateEm0010_Walk::Create(
			Helper::To_wString(magic_enum::enum_name(MonsterStateMachine::MONSTER_STATE::WALK)), em0010))))
			return E_FAIL;
		if (FAILED(m_States->Add_State(StateEm0010_Dead::Create(
			Helper::To_wString(magic_enum::enum_name(MonsterStateMachine::MONSTER_STATE::DEAD)), em0010))))
			return E_FAIL;

		m_States->Change_State(MonsterStateMachine::MONSTER_STATE::IDLE);
	}
	else
		return E_FAIL;

	m_Em0010Movement->Begin();

	SphereCollider::SPHERE_COLLIDER_DESC physicalZoneDesc{};
	physicalZoneDesc.offset = Vector3::UnitY;
	physicalZoneDesc.radius = 0.75f;
	m_PhysicalZone = Add_Component<SphereCollider>(ETOI(LEVEL::STATIC), &physicalZoneDesc);
	if (nullptr == m_PhysicalZone)
		return E_FAIL;

	return S_OK;
}

Shared<Em0010> Em0010::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<Em0010>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Em0010");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> Em0010::Clone(void* arg)
{
	auto instance = make_shared<Em0010>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : Em0010");
		return nullptr;
	}

	return instance;
}

void Em0010::Apply_PushoutCorrection(const Vector3& correction)
{
	if (m_Em0010Movement)
	{
		m_Em0010Movement->Add_Correction(correction);
	}
}


