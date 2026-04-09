#include "pch.h"
#include "Em0010.h"

#include <Game.h>
#include <SpdLogger.h>
#include <SphereCollider.h>

#include "HpBarWorldUI.h"
#include "Em0010Body.h"
#include "Em0010Movement.h"
#include "MonsterSight.h"
#include "MonsterStateMachine.h"
#include "State2B_Run.h"
#include "StateEm0010_Hit.h"
#include "StateEm0010_Idle.h"

Em0010::Em0010() : Monster{} {}
Em0010::Em0010(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Monster{device, context} {}
Em0010::Em0010(const Em0010& rhs) 
	: Monster{rhs} {}

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
	Monster::On_Destroy();
}

void Em0010::Priority_Update(Float timeDelta)
{
	
}

void Em0010::Update(Float timeDelta)
{
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
	Play_HitSFX(dmgInfo);
	DisplaySparkEffect(dmgInfo.attackType, dmgInfo.hitPosition, dmgInfo.hitRotation);

	Entity::TakeDamage(dmgInfo);
}

void Em0010::OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	
}

void Em0010::OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	
}

void Em0010::OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{

}

HRESULT Em0010::Ready_PartObjects()
{
	Em0010Body::EM0010BODY_DESC em0010BodyDesc{};
	em0010BodyDesc.parentMatrix = m_Transform->Get_WorldMatrixPtr();
	em0010BodyDesc.Owner = static_pointer_cast<ContainerObject>(shared_from_this());
	if (FAILED(Add_PartObject(ETOI(LEVEL::GAMEPLAY), L"Em0010Body", L"Em0010Body", &em0010BodyDesc)))
		return E_FAIL;

	MonsterSight::MONSTER_SIGHT_DESC em0010SightDesc{};
	em0010SightDesc.parentMatrix = m_Transform->Get_WorldMatrixPtr();
	em0010SightDesc.Owner = static_pointer_cast<ContainerObject>(shared_from_this());
	em0010SightDesc.radius = 3.f;
	em0010SightDesc.offset = Vector3::Zero;
	if (FAILED(Add_PartObject(ETOI(LEVEL::GAMEPLAY), L"MonsterSight", L"Em0010Sight", &em0010SightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT Em0010::Ready_Components()
{
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


