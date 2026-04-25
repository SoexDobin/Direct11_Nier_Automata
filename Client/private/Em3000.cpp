#include "pch.h"
#include "Em3000.h"

#include <Game.h>
#include <SpdLogger.h>

#include "Em3000Body.h"
#include "Em3000MeleeSight.h"
#include "Em3000Movement.h"
#include "Em3001.h"
#include "SphereCollider.h"
#include "Navigation.h"
#include "HpBarWorldUI.h"
#include "Em3000StateMachine.h"
#include "MonsterAOE.h"
#include "MonsterSight.h"
#include "StateEm3000_Idle.h"
#include "StateEm3000_Range.h"
#include "StateEm3000_Melee.h"

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

	uint32 levIndex = GAME_INSTANCE->Get_TargetLevelIndex();
	GAME_INSTANCE->Add_Instance_Event(levIndex, L"Add_HpBar", [this, levIndex]()
		{
			HpBarWorldUI::HP_BAR_WORLD_UI_DESC UI_hpDesc{};
			UI_hpDesc.target = static_pointer_cast<Entity>(shared_from_this());
			UI_hpDesc.worldOffset = Vector3{ 0.f, 4.f, 0.f };
			UI_hpDesc.anchor = UI_ANCHOR::TOP_LEFT;
			UI_hpDesc.sizeX = 200.f;
			UI_hpDesc.sizeY = 10.f;
			UI_hpDesc.x = 0.f;
			UI_hpDesc.y = 0.f;
			m_HpBarUI = GAME_INSTANCE->Instantiate<HpBarWorldUI>(L"HpBarWorldUI", levIndex, &UI_hpDesc);
		});

	m_IsStatic = true;
	m_MaxHp = 10000.f;
	m_Hp = 10000.f;

	return S_OK;
}

void Em3000::On_Destroy()
{
	Monster::On_Destroy();
}

void Em3000::Priority_Update(Float timeDelta)
{
	Float ratio = m_Hp / m_MaxHp;
	if (ratio <= 0.4f)
		static_pointer_cast<Em3000StateMachine>(m_States)->Set_SecondPhase();
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
	m_States->Update_State(timeDelta);
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

void Em3000::Set_Navigation(const Shared<Navigation>& navigation)
{
	Entity::Set_Navigation(navigation);
	m_Em3000Movement->Set_TargetNavigation(m_Navigation);
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
	uint32 levIndex = GAME_INSTANCE->Get_TargetLevelIndex();
	auto em3000 = static_pointer_cast<Em3000>(shared_from_this());

	PartObject::PARTOBJECT_DESC bodyDesc{};
	bodyDesc.parentMatrix = m_Transform->Get_WorldMatrixPtr();
	bodyDesc.Owner = em3000;
	if (FAILED(Add_PartObject(levIndex, L"Em3000Body", L"Em3000Body", &bodyDesc)))
		return E_FAIL;
	m_MainBody = static_pointer_cast<Em3000Body>(Find_PartObject(L"Em3000Body"));

	auto animMesh = Shader::SHADER_DESC{ VTXANIMMESH::Tag,  VTXANIMMESH::Elements, VTXANIMMESH::numElements };
	auto staticMesh = Shader::SHADER_DESC{ VTXMESH::Tag,  VTXMESH::Elements, VTXMESH::numElements };

	Em3000Parts::EM3000PART_DESC partsDesc{};
	partsDesc.parentMatrix = m_Transform->Get_WorldMatrixPtr();
	partsDesc.Owner = em3000;
	partsDesc.bodyModel = m_MainBody->Get_ModelComponent();

	partsDesc.shaderDesc = staticMesh;
	partsDesc.modelResourceTag = L"em3001";
	if (FAILED(Add_PartObject(levIndex, L"Em3001", L"Em3001", &partsDesc))) 
		return E_FAIL;
	
	partsDesc.shaderDesc = staticMesh;
	partsDesc.modelResourceTag = L"em3002";
	if (FAILED(Add_PartObject(levIndex, L"Em3002", L"Em3002", &partsDesc))) 
		return E_FAIL;

	partsDesc.shaderDesc = staticMesh;
	partsDesc.modelResourceTag = L"em3003";
	if (FAILED(Add_PartObject(levIndex, L"Em3003", L"Em3003", &partsDesc))) 
		return E_FAIL;

	MonsterSight::MONSTER_SIGHT_DESC sightDesc{};
	sightDesc.Owner = em3000;
	sightDesc.parentMatrix = m_Transform->Get_WorldMatrixPtr();

	sightDesc.offset = Vector3::Zero;
	sightDesc.radius = 6.f;
	if (FAILED(Add_PartObject(levIndex, L"Em3000MeleeSight", L"Em3000MeleeSight", &sightDesc)))
		return E_FAIL;
	m_MeleeSight = static_pointer_cast<Em3000MeleeSight>(Find_PartObject(L"Em3000MeleeSight"));

	sightDesc.offset = Vector3::Zero;
	sightDesc.radius = 20.f;
	if (FAILED(Add_PartObject(levIndex, L"MonsterSight", L"Sight", &sightDesc)))
		return E_FAIL;
	m_Sight = static_pointer_cast<MonsterSight>(Find_PartObject(L"Sight"));

	DAMAGE_INFO em3000DamageInfo{};
	em3000DamageInfo.attacker = shared_from_this();
	em3000DamageInfo.attackType = ATK_TYPE::HEAVY;
	em3000DamageInfo.damage = 30.f;
	em3000DamageInfo.groggyWeight = 10.f;
	em3000DamageInfo.knockbackForce = 1.f;

	auto thisObject = static_pointer_cast<ContainerObject>(shared_from_this());
	MonsterAOE::MONSTER_AOE_DESC centerDesc{};
	centerDesc.parentMatrix = m_Transform->Get_WorldMatrixPtr();
	centerDesc.Owner = thisObject;
	centerDesc.model = m_MainBody->Get_ModelComponent();
	centerDesc.offset = Vector3::Zero;
	centerDesc.radius = 3.f;
	centerDesc.targetBoneName = "bone774";
	centerDesc.dmgInfo = em3000DamageInfo;
	if (FAILED(Add_PartObject(levIndex, L"MonsterAOE", L"Em3000CenterAoe", &centerDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT Em3000::Ready_Components()
{
	uint32 levIndex = GAME_INSTANCE->Get_TargetLevelIndex();

	Navigation::NAVIGATION_DESC navDesc;
	navDesc.startCellIndex = 0;
	m_Navigation = Add_Component_Tag<Navigation>(ETOI(LEVEL::STATIC), L"AmusementParkDome", &navDesc);

	Em3000Movement::EM3000_MOVEMENT_DESC movementDesc{};
	movementDesc.velocity = Vector3{ 0.f, 0.f, 0.f };
	movementDesc.moveSpeed = 0.f;
	movementDesc.targetDirection = Vector3::Zero;
	movementDesc.turnSpeed = 0.f;
	m_Em3000Movement = Add_Component<Em3000Movement>(levIndex, &movementDesc);
	if (nullptr == m_Em3000Movement)
		return E_FAIL;


	auto em3000 = static_pointer_cast<Em3000>(shared_from_this());
	if ((m_States = Add_Component<Em3000StateMachine>(levIndex)))
	{
		auto stateMachine = static_pointer_cast<Em3000StateMachine>(m_States);
		if (FAILED(stateMachine->Add_State(StateEm3000_Idle::Create(Helper::To_wString(magic_enum::enum_name(EM3000_STATE::IDLE)), em3000))))
			return E_FAIL;
		if (FAILED(stateMachine->Add_State(StateEm3000_Range::Create(Helper::To_wString(magic_enum::enum_name(EM3000_STATE::PHASE1_RANGE)), em3000))))
			return E_FAIL;
		if (FAILED(stateMachine->Add_State(StateEm3000_Melee::Create(Helper::To_wString(magic_enum::enum_name(EM3000_STATE::PHASE1_MELEE)), em3000))))
			return E_FAIL;
		if (FAILED(stateMachine->Add_State(StateEm3000_Melee::Create(Helper::To_wString(magic_enum::enum_name(EM3000_STATE::GROGGY)), em3000))))
			return E_FAIL;
		if (FAILED(stateMachine->Add_State(StateEm3000_Melee::Create(Helper::To_wString(magic_enum::enum_name(EM3000_STATE::PHASE2_TRANSFORM)), em3000))))
			return E_FAIL;

		stateMachine->Change_State(EM3000_STATE::IDLE);
	}
	else
		return E_FAIL;

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


