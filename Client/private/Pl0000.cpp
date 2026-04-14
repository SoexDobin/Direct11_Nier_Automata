#include "pch.h"
#include "Pl0000.h"

#include <Game.h>
#include <SpdLogger.h>
#include <SphereCollider.h>
#include <Navigation.h>

#include "Bullet.h"
#include "Monster.h"
#include "Pl0000Body.h"
#include "Pl0000Movement.h"
#include "Pl0000StateMachine.h"
#include "State2B_AttackAir.h"
#include "State2B_AttackGround.h"
#include "WP3000Body.h"

#include "StateMachine.h"
#include "State2B_Idle.h"
#include "State2B_Jump.h"
#include "State2B_Run.h"
#include "State2B_Sprint.h"
#include "State2B_Walk.h"
#include "State2B_Evade.h"

namespace Client {

Pl0000::Pl0000() : Entity{} {}
Pl0000::Pl0000(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Entity{ device, context } {}
Pl0000::Pl0000(const Pl0000& rhs)
	: Entity{ rhs } {}

HRESULT Pl0000::Initialize_Prototype()
{
	m_LayerMask.Set_Layer(L"PlayerPhysical");

	return ContainerObject::Initialize_Prototype();
}

HRESULT Pl0000::Initialize(void* arg)
{
	GAME_INSTANCE->Add_Instance_Event(ETOI(LEVEL::GAMEPLAY), L"Add_Owner_To_Camera", [&]()
		{
			auto camera = GAME_INSTANCE->Get_MainCamera();
			
			if (camera != nullptr)
			{
				camera->Set_Target(shared_from_this());
			}

#ifdef _DEBUG
			//Entity::DAMAGE_INFO dmgInfo{};
			//dmgInfo.attackType = ATK_TYPE::LIGHT;
			//dmgInfo.attackerPos = m_Transform->Get_Position() + Vector3{ 0.f, 0.f, -10.f };
			//dmgInfo.damage = 25.f;
			//dmgInfo.groggyWeight = 10;
			//dmgInfo.knockbackForce = 0.5f;
			//// 2. 총알 공통 속성 세팅 (정지 상태 고정)
			//Bullet::BULLET_DESC desc{};
			//desc.isPermanent = false;
			//desc.damageInfo = dmgInfo;
			//desc.resourceTag = L"candy";         // 💡 테스트용 모델이 잘 뜨는지 확인
			//desc.targetLayer = L"Player";
			//desc.speed = 0.0f;                   // 스피드 0
			//desc.maxDistance = 9999.f;
			//desc.direction = m_Transform->Get_Look();
			//
			//Float spawnHeight = m_Transform->Get_Position().y + 1.f; // 공중에 깔려면 y값을 적절히 조절하세요 (ex: 1.0f)
			//for (int x = -1; x <= 2; ++x)
			//{
			//	for (int z = -1; z <= 2; ++z)
			//	{
			//		desc.initialPosition = Vector3(static_cast<Float>(x), spawnHeight, static_cast<Float>(z));
			//		GAME_INSTANCE->Instantiate<Bullet>(L"Bullet", ETOI(LEVEL::GAMEPLAY), &desc);
			//	}
			//}
#endif

		});

	if (FAILED(ContainerObject::Initialize(arg))) {
		LOG_ERROR(L"Failed to Initialize GameObject {}", m_ObjectName);
		return E_FAIL;
	}
	if (FAILED(Ready_PartObjects())) {
		LOG_ERROR(L"Failed to Ready PartObjects {}", m_ObjectName);
		return E_FAIL;
	}
	if (FAILED(Ready_Components())) {
		LOG_ERROR(L"Failed to Ready Components {}", m_ObjectName);
		return E_FAIL;
	}

	return S_OK;
}

void Pl0000::On_Destroy()
{
	
	ContainerObject::On_Destroy();
}

void Pl0000::Priority_Update(Float timeDelta)
{
	m_Pl0000Input->Update_P10000_InputState(timeDelta);
}

void Pl0000::Update(Float timeDelta)
{
	if (m_LagDuration > 0.f)
	{
		m_LagDuration -= timeDelta;
		timeDelta *= 0.05f; 
	}

	if (auto target = m_LockOnTarget.lock())
	{
		if (target->Is_Destroy() || !target->Is_Active())
		{
			m_LockOnTarget.reset(); // 대상 파괴 시 파기
		}
		else
		{
			Vector3 myPos = Get_Transform()->Get_Position();
			Vector3 targetPos = target->Get_Transform()->Get_Position();
			
			Vector3 myLook = Get_Transform()->Get_Look();
			myLook.y = 0.f;
			if (myLook.Length() > 0.001f) myLook.Normalize();
			Vector3 dirToTarget = targetPos - myPos;
			dirToTarget.y = 0.f;
			if (dirToTarget.Length() > 0.001f) dirToTarget.Normalize();
			
			if (myLook.Dot(dirToTarget) <= 0.f)
			{
				m_LockOnTarget.reset();
			}
		}
	}

	m_Pl0000States->Update_State(timeDelta);
}

void Pl0000::Late_Update(Float timeDelta)
{
	if (m_LagDuration > 0.f)
		timeDelta *= 0.05f;

	m_Pl0000Movement->Update_Movement(timeDelta);
	m_Transform->Update_WorldMatrix();
	m_PhysicalZone->Update(m_Transform->Get_WorldMatrix());
}

void Pl0000::Fixed_Update(Float fixedDelta)
{
	
}

HRESULT Pl0000::Render()
{
	return S_OK;
}

void Pl0000::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

void Pl0000::OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	
}

void Pl0000::OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	auto target = targetCollider->Get_Owner();
	auto targetLayerName = target->Get_LayerMask().Get_LayerName();

	if (target->Get_GameObjectType() != GAMEOBJECTTYPE::CONTAINER ||
		targetLayerName != L"MonsterPhysical")
		return;
	
	if (static_pointer_cast<Monster>(target)->Is_Static())
	{
		Vector3 pushOutDelta = PushoutDelta(ownCollider, targetCollider, 1.f);
		m_Pl0000Movement->Add_Correction(pushOutDelta);
	}
}

void Pl0000::OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	
}

void Pl0000::TakeDamage(const DAMAGE_INFO& dmgInfo)
{
	if (TryEvade(dmgInfo.attacker.lock())) return;

	LOG_INFO(L"Hit ---------------------");

	Entity::TakeDamage(dmgInfo);
}

void Pl0000::OnAttackHit(const Shared<GameObject>& target)
{
	if (m_Pl0000Movement)
	{
		m_Pl0000Movement->Reduce_RootMotion();
	}
}

Bool Pl0000::TryEvade(const Shared<GameObject>& attacker)
{
	if (m_Pl0000States->Get_CurP10000State() != PL0000_STATE::EVADE)
		return false;

	auto dashState = static_pointer_cast<State2B_Evade>(
		m_Pl0000States->Find_2BState(PL0000_STATE::EVADE));
	if (!dashState)
		return false;

	Bool isEvaded = dashState->TryEvade_FromDash();

	if (isEvaded && attacker)
	{
		if (attacker->Get_GameObjectType() == GAMEOBJECTTYPE::PART)
		{
			m_LockOnTarget = static_pointer_cast<PartObject>(attacker)->Get_Owner();
		}
		else if (attacker->Get_GameObjectType() == GAMEOBJECTTYPE::CONTAINER)
		{
			m_LockOnTarget = attacker;
		}
	}

	return isEvaded;
}

HRESULT Pl0000::Ready_PartObjects()
{
	m_LightSheathMatrix =
		Matrix::CreateRotationX(XMConvertToRadians(90.f)) * Matrix::CreateRotationZ(XMConvertToRadians(-30.f)) *
		Matrix::CreateTranslation(Vector3{ 0.f, 1.5f, -0.4f });
	m_HeavySheathMatrix = 
		Matrix::CreateRotationX(XMConvertToRadians(90.f)) * Matrix::CreateRotationY(XMConvertToRadians(-40.f))  * Matrix::CreateRotationZ(XMConvertToRadians(-20.f)) *
		Matrix::CreateTranslation(Vector3{ 0.f, 1.0f, -0.5f });

	Pl0000Body::Pl0000BODY_DESC desc{};
	desc.parentMatrix = m_Transform->Get_WorldMatrixPtr();
	desc.Owner = static_pointer_cast<ContainerObject>(shared_from_this());

	if (FAILED(Add_PartObject(ETOI(LEVEL::GAMEPLAY), L"Pl0000Body", L"Pl0000Body", &desc)))
		return E_FAIL;
	if (FAILED(Add_PartObject(ETOI(LEVEL::GAMEPLAY), L"WP0070Body", L"WP0070Body", &desc)))
		return E_FAIL;
	if (FAILED(Add_PartObject(ETOI(LEVEL::GAMEPLAY), L"WP0220Body", L"WP0220Body", &desc)))
		return E_FAIL;
	if (FAILED(Add_PartObject(ETOI(LEVEL::GAMEPLAY), L"WP3000Body", L"WP3000Body", &desc)))
		return E_FAIL;
	if (FAILED(Add_PartObject(ETOI(LEVEL::GAMEPLAY), L"Pl0000EvadeChecker", L"Pl0000EvadeChecker", &desc)))
		return E_FAIL;
	if (FAILED(Add_PartObject(ETOI(LEVEL::GAMEPLAY), L"Pl0000MonsterChecker", L"Pl0000MonsterChecker", &desc)))
		return E_FAIL;

	m_MainBody = static_pointer_cast<Pl0000Body>(Find_PartObject(L"Pl0000Body"));
	m_MonsterChecker = static_pointer_cast<Pl0000MonsterChecker>(Find_PartObject(L"Pl0000MonsterChecker"));

	auto wp0070 = static_pointer_cast<WP0070Body>(Find_PartObject(L"WP0070Body"));
	wp0070->DrawWP0070();
	wp0070->Set_Sheathing(m_LightSheathMatrix);
	auto wp0220 = static_pointer_cast<WP0220Body>(Find_PartObject(L"WP0220Body"));
	wp0220->DrawWP0220();
	wp0220->Set_Sheathing(m_HeavySheathMatrix);

	auto wp3000 = static_pointer_cast<WP3000Body>(Find_PartObject(L"WP3000Body"));
	wp3000->Set_Pl0000Container(static_pointer_cast<Pl0000>(shared_from_this()));
	wp3000->Set_Pl0000Body(m_MainBody);

	return S_OK;
}

HRESULT Pl0000::Ready_Components()
{
	m_Pl0000Input = Add_Component<Pl0000Input>(ETOI(LEVEL::GAMEPLAY));
	if (nullptr == m_Pl0000Input)
		return E_FAIL;

	Pl0000Movement::PL0000_MOVEMENT_DESC movementDesc{};
	movementDesc.velocity = Vector3{0.f, 0.f, 0.f};
	movementDesc.moveSpeed = 0.f;
	movementDesc.targetDirection = Vector3::Zero;
	movementDesc.turnSpeed = 7.5f;
	m_Pl0000Movement = Add_Component<Pl0000Movement>(ETOI(LEVEL::GAMEPLAY), &movementDesc);
	if (nullptr == m_Pl0000Movement)
		return E_FAIL;

	Navigation::NAVIGATION_DESC navDesc;
	navDesc.startCellIndex = 0;
	m_Navigation = Add_Component_Tag<Navigation>(ETOI(LEVEL::STATIC), L"CityOfRuinEntry", &navDesc);

	// StateMachine은 마지막에 처리
	if ((m_Pl0000States = Add_Component<Pl0000StateMachine>(ETOI(LEVEL::GAMEPLAY))))
	{
		auto pl0000 = static_pointer_cast<Pl0000>(shared_from_this());

		if (FAILED(m_Pl0000States->Add_State(State2B_Idle::Create(
			Helper::To_wString(magic_enum::enum_name(PL0000_STATE::IDLE)), pl0000))))
			return E_FAIL;
		if (FAILED(m_Pl0000States->Add_State(State2B_Run::Create(
			Helper::To_wString(magic_enum::enum_name(PL0000_STATE::RUN)), pl0000))))
			return E_FAIL;
		if (FAILED(m_Pl0000States->Add_State(State2B_Sprint::Create(
			Helper::To_wString(magic_enum::enum_name(PL0000_STATE::SPRINT)), pl0000))))
			return E_FAIL;
		if (FAILED(m_Pl0000States->Add_State(State2B_Jump::Create(
			Helper::To_wString(magic_enum::enum_name(PL0000_STATE::JUMP)), pl0000))))
			return E_FAIL;
		if (FAILED(m_Pl0000States->Add_State(State2B_Evade::Create(
			Helper::To_wString(magic_enum::enum_name(PL0000_STATE::EVADE)), pl0000))))
			return E_FAIL;
		if (FAILED(m_Pl0000States->Add_State(State2B_AttackGround::Create(
			Helper::To_wString(magic_enum::enum_name(PL0000_STATE::ATTACK_GROUND)), pl0000))))
			return E_FAIL;
		if (FAILED(m_Pl0000States->Add_State(State2B_AttackAir::Create(
			Helper::To_wString(magic_enum::enum_name(PL0000_STATE::ATTACK_AIR)), pl0000))))
			return E_FAIL;

		m_Pl0000States->Change_State(PL0000_STATE::IDLE);
	}
	else
		return E_FAIL;

	m_Pl0000Movement->Begin();
	m_Pl0000Input->Begin();

	SphereCollider::SPHERE_COLLIDER_DESC physicalZoneDesc{};
	physicalZoneDesc.offset = Vector3::UnitY;
	physicalZoneDesc.radius = 0.75f;
	m_PhysicalZone = Add_Component<SphereCollider>(ETOI(LEVEL::STATIC), &physicalZoneDesc);
	if (nullptr == m_PhysicalZone)
		return E_FAIL;

	return S_OK;
}

Shared<Pl0000> Pl0000::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	Shared<Pl0000> pInstance = make_shared<Pl0000>(device, context);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		LOG_ERROR(L"Failed to Created : 2B");
		MSG_BOX("Failed to Created : 2B");
	}
	return pInstance;
}

Shared<GameObject> Pl0000::Clone(void* arg)
{
	Shared<Pl0000> pInstance = make_shared<Pl0000>(*this);

	if (FAILED(pInstance->Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Cloned : 2B");
		MSG_BOX("Failed to Cloned : 2B");
	}
	return pInstance;
}

}
