#include "pch.h"
#include "Em3000.h"

#include <Game.h>
#include <SpdLogger.h>

#include "Em3000Body.h"
#include "Em3000Movement.h"
#include "Em3001.h"
#include "SphereCollider.h"
#include "Navigation.h"
#include "HpBarWorldUI.h"
#include "MonsterStateMachine.h"

Em3000::Em3000(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Monster{device, context} {}
Em3000::Em3000(const Em3000& rhs)
	: Monster{ rhs } { }

HRESULT Em3000::Initialize_Prototype()
{
	return Monster::Initialize_Prototype();
}

HRESULT Em3000::Initialize(void* arg)
{
	if (FAILED(Monster::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize Em3000");
		return E_FAIL;
	}

	if (FAILED(Ready_PartObjects()))
	{
		LOG_ERROR(L"Failed to Ready_PartObjects Em3000");
		return E_FAIL;
	}

	if (FAILED(Ready_Components()))
	{
		LOG_ERROR(L"Failed to Ready_Components Em3000");
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

	m_IsStatic = true;
	m_MaxHp = 100000.f;
	m_Hp = 100000.f;

	return S_OK;
}

void Em3000::On_Destroy()
{
	Monster::On_Destroy();
}

void Em3000::Priority_Update(Float timeDelta)
{
	
}

void Em3000::Update(Float timeDelta)
{
	if (m_LagDuration > 0.f)
	{
		m_LagDuration -= timeDelta;
		timeDelta *= 0.05f;
	}

	m_Transform->Update_WorldMatrix();
	m_Em3000Movement->Update_Movement(timeDelta);
	m_PhysicalZone->Update(*m_Transform->Get_WorldMatrixPtr());
}

void Em3000::Late_Update(Float timeDelta)
{

}

void Em3000::Fixed_Update(Float fixedDelta)
{
	
}

HRESULT Em3000::Render()
{
	return S_OK;
}

void Em3000::Submit_RenderGroup()
{

}

const TRANSFORM_FRAME& Em3000::Get_BodyModelTransform() const
{
	return m_MainBody->Get_ModelTransform();
}

void Em3000::TakeDamage(const DAMAGE_INFO& dmgInfo)
{
	if (Is_Dead()) return;

	Play_HitSFX(dmgInfo);
	DisplaySparkEffect(dmgInfo.attackType, dmgInfo.hitPosition, dmgInfo.hitRotation);

	// TODO : 스테이트 구성하면 키기
	if (dmgInfo.attackType != ATK_TYPE::POD)
		//m_States->Change_State(MonsterStateMachine::MONSTER_STATE::Hit);

	Monster::TakeDamage(dmgInfo);
}

void Em3000::OnDeath()
{
	//m_MainBody->OffHitBox();
	// TODO : 콜라이더 끄기
	//m_States->Change_State(MonsterStateMachine::MONSTER_STATE::DEAD);
}

void Em3000::OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	
}

void Em3000::OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	Monster::OnCollisionStay(ownCollider, targetCollider);
}

void Em3000::OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	
}

HRESULT Em3000::Ready_PartObjects()
{
	PartObject::PARTOBJECT_DESC bodyDesc{};
	if (FAILED(Add_Child(ETOI(LEVEL::GAMEPLAY), L"Em3000Body", L"Em3000Body", &bodyDesc)))
		return E_FAIL;
	m_MainBody = static_pointer_cast<Em3000Body>(Find_Child(L"Em3000Body"));

	auto animMesh = Shader::SHADER_DESC{ VTXANIMMESH::Tag,  VTXANIMMESH::Elements, VTXANIMMESH::numElements };
	auto staticMesh = Shader::SHADER_DESC{ VTXMESH::Tag,  VTXMESH::Elements, VTXMESH::numElements };

	Em3000Parts::EM3000PART_DESC partsDesc{};
	partsDesc.bodyModel = m_MainBody->Get_ModelComponent();

	partsDesc.shaderDesc = staticMesh;
	partsDesc.modelResourceTag = L"em3001";
	partsDesc.targetBoneName = "bone0";
	if (FAILED(Add_Child(ETOI(LEVEL::GAMEPLAY), L"Em3001", L"Em3001", &partsDesc)))
		return E_FAIL;
	
	partsDesc.shaderDesc = staticMesh;
	partsDesc.modelResourceTag = L"em3002";
	partsDesc.targetBoneName = "bone0";
	if (FAILED(Add_Child(ETOI(LEVEL::GAMEPLAY), L"Em3002", L"Em3002", &partsDesc)))
		return E_FAIL;

	partsDesc.shaderDesc = animMesh;
	partsDesc.modelResourceTag = L"em3003";
	partsDesc.targetBoneName = "bone0";
	if (FAILED(Add_Child(ETOI(LEVEL::GAMEPLAY), L"Em3003", L"Em3003", &partsDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT Em3000::Ready_Components()
{
	Navigation::NAVIGATION_DESC navDesc;
	navDesc.startCellIndex = 0;
	m_Navigation = Add_Component_Tag<Navigation>(ETOI(LEVEL::STATIC), L"CityOfRuinEntry", &navDesc);

	Em3000Movement::EM3000_MOVEMENT_DESC movementDesc{};
	movementDesc.velocity = Vector3{ 0.f, 0.f, 0.f };
	movementDesc.moveSpeed = 0.f;
	movementDesc.targetDirection = Vector3::Zero;
	movementDesc.turnSpeed = 7.5f;
	m_Em3000Movement = Add_Component<Em3000Movement>(ETOI(LEVEL::GAMEPLAY), &movementDesc);
	if (nullptr == m_Em3000Movement)
		return E_FAIL;

	// StateMachine은 마지막에 처리
	//if ((m_States = Add_Component<MonsterStateMachine>()))
	//{
	//	//if (FAILED(m_States->Add_State(StateEm0010_Idle::Create(
	//	//	Helper::To_wString(magic_enum::enum_name(MonsterStateMachine::MONSTER_STATE::IDLE)), em0010))))
	//	//	return E_FAIL;
	//	//if (FAILED(m_States->Add_State(StateEm0010_Hit::Create(
	//	//	Helper::To_wString(magic_enum::enum_name(MonsterStateMachine::MONSTER_STATE::Hit)), em0010))))
	//	//	return E_FAIL;
	//	//if (FAILED(m_States->Add_State(StateEm0010_Chase::Create(
	//	//	Helper::To_wString(magic_enum::enum_name(MonsterStateMachine::MONSTER_STATE::CHASE)), em0010))))
	//	//	return E_FAIL;
	//	//if (FAILED(m_States->Add_State(StateEm0010_Attack::Create(
	//	//	Helper::To_wString(magic_enum::enum_name(MonsterStateMachine::MONSTER_STATE::ATTACK)), em0010))))
	//	//	return E_FAIL;
	//	//if (FAILED(m_States->Add_State(StateEm0010_Walk::Create(
	//	//	Helper::To_wString(magic_enum::enum_name(MonsterStateMachine::MONSTER_STATE::WALK)), em0010))))
	//	//	return E_FAIL;
	//	//if (FAILED(m_States->Add_State(StateEm0010_Dead::Create(
	//	//	Helper::To_wString(magic_enum::enum_name(MonsterStateMachine::MONSTER_STATE::DEAD)), em0010))))
	//	//	return E_FAIL;
	//	//
	//	//m_States->Change_State(MonsterStateMachine::MONSTER_STATE::IDLE);
	//}
	//else
	//	return E_FAIL;

	if (FAILED(m_Em3000Movement->Begin()))
	{
		LOG_ERROR(L"Failed to Begin EM3000 Movement");
		return E_FAIL;
	}

	SphereCollider::SPHERE_COLLIDER_DESC physicalZoneDesc{};
	physicalZoneDesc.offset = Vector3::UnitY;
	physicalZoneDesc.radius = 3.f;
	m_PhysicalZone = Add_Component<SphereCollider>(ETOI(LEVEL::STATIC), &physicalZoneDesc);
	if (nullptr == m_PhysicalZone)
		return E_FAIL;

	return S_OK;
}

Shared<Em3000> Em3000::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<Em3000>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Em3000");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> Em3000::Clone(void* arg)
{
	auto instance = make_shared<Em3000>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : Em3000");
		return nullptr;
	}

	return instance;
}


