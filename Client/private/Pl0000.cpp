#include "pch.h"
#include "Pl0000.h"

#include <Game.h>
#include <SpdLogger.h>
#include <SphereCollider.h>
#include <Navigation.h>

#include "Bullet.h"
#include "LoadingFadeOut.h"
#include "Monster.h"
#include "Pl0000Body.h"
#include "Pl0000HpBar.h"
#include "Pl0000Movement.h"
#include "Pl0000StateMachine.h"
#include "SheathWP0070Body.h"
#include "SheathWP0220Body.h"
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
#include "State2B_Hit.h"
#include "WeaponHalo.h"

namespace Client {

Pl0000::Pl0000(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Entity{ device, context } {}
Pl0000::Pl0000(const Pl0000& rhs)
	: Entity{ rhs } {}

void Pl0000::Draw_LightWeapon()
{
	if (m_LightWeapon)
	{
		m_LightWeapon->Set_Active(true);
		m_LightWeapon->DrawWP0070();
	}
	
	if (m_SheathLightWeapon)
	{
		m_SheathLightWeapon->Set_Active(false);
		m_LightHalo->Set_Active(false);
	}
}
void Pl0000::Sheathe_LightWeapon()
{
	if (m_LightWeapon)
	{
		m_LightWeapon->Set_Active(false);
		m_LightWeapon->Set_Sheathing(); 
	}

	if (m_SheathLightWeapon)
	{
		m_SheathLightWeapon->Set_Active(true);
		m_LightHalo->Set_Active(true);
	}
}

void Pl0000::Draw_HeavyWeapon()
{
	if (m_HeavyWeapon)
	{
		m_HeavyWeapon->Set_Active(true);
		m_HeavyWeapon->DrawWP0220();
	}

	if (m_SheathHeavyWeapon)
	{
		m_SheathHeavyWeapon->Set_Active(false);
		m_HeavyHalo->Set_Active(false);
	}
}

void Pl0000::Sheathe_HeavyWeapon()
{
	if (m_HeavyWeapon)
	{
		m_HeavyWeapon->Set_Active(false);
		m_HeavyWeapon->Set_Sheathing();
	}

	if (m_SheathHeavyWeapon)
	{
		m_SheathHeavyWeapon->Set_Active(true);
		m_HeavyHalo->Set_Active(true);
	}
}

HRESULT Pl0000::Initialize_Prototype()
{
	m_LayerMask.Set_Layer(L"PlayerPhysical");

	

	return ContainerObject::Initialize_Prototype();
}

HRESULT Pl0000::Initialize(void* arg)
{
	uint32 levIndex = GAME_INSTANCE->Get_TargetLevelIndex();
	GAME_INSTANCE->Add_Instance_Event(levIndex, L"Add_Owner_To_Camera", [&]()
		{
			auto camera = GAME_INSTANCE->Get_MainCamera();
			
			if (camera != nullptr)
			{
				camera->Set_Target(shared_from_this());
			}
		});
	Pl0000HpBar::PL0000_HP_BAR_DESC desc{};
	desc.target = static_pointer_cast<Entity>(shared_from_this());
	desc.sizeX = 250.f;
	desc.sizeY = 20.f;
	desc.anchor = UI_ANCHOR::TOP_LEFT;
	desc.x = 300.f;
	desc.y = 80.f;
	// desc.anchor, x, y 등을 직접 줘도 되고 비워두면 자동 좌상단 세팅
	GAME_INSTANCE->Add_Instance_Event(levIndex, L"SpawnPl0000HpBar", [=]() mutable {
		GAME_INSTANCE->Instantiate<Pl0000HpBar>(L"Pl0000HpBar", levIndex, &desc);
		});

	GAME_INSTANCE->Add_Instance_Event(levIndex, L"Fade_Out", [=]() mutable {
		GAME_INSTANCE->Instantiate<LoadingFadeOut>(L"LoadingFadeOut", levIndex);
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

	m_BulletLayerIndex = ETOI(GAME_INSTANCE->Get_LayerRegister()->Get_LayerByName(L"Bullet"));

	return S_OK;
}

void Pl0000::On_Destroy()
{
	
	ContainerObject::On_Destroy();
}

void Pl0000::Priority_Update(Float timeDelta)
{
	m_Pl0000Input->Update_Pl0000_InputState(timeDelta);
}

void Pl0000::Update(Float timeDelta)
{
	if (m_IsGlobalLagActive)
	{
		Float unscaledDelta = GAME_INSTANCE->Compute_UnscaledTimeDelta();

		m_GlobalLagTimer -= unscaledDelta;

		if (m_GlobalLagTimer <= 0.f)
		{
			m_IsGlobalLagActive = false;
			GAME_INSTANCE->Set_TimeScale(1.0f); // 정상 속도로 복귀
		}
	}

	if (m_InvincibleTimer > 0.f)
	{
		m_InvincibleTimer -= GAME_INSTANCE->Compute_UnscaledTimeDelta();
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

void Pl0000::Set_Navigation(const Shared<Navigation>& navigation)
{
	Entity::Set_Navigation(navigation);
	m_Pl0000Movement->Set_TargetNavigation(m_Navigation);
}

void Pl0000::TakeDamage(const DAMAGE_INFO& dmgInfo)
{
	if (TryEvade(dmgInfo.attacker.lock())) return;

	LOG_INFO(L"{}", m_Hp);

	if (Is_Invincible_Active()) return;

	Entity::TakeDamage(dmgInfo);
	if (dmgInfo.damage >= 30.f)
		m_Pl0000States->Change_State(PL0000_STATE::HIT);
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
	if (attacker == nullptr) return false;
	if (attacker->Get_LayerMask().Get_Layer() == m_BulletLayerIndex) return false;

	if (m_Pl0000States->Get_CurPl0000State() != PL0000_STATE::EVADE)
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
	Pl0000Body::Pl0000BODY_DESC desc{};
	uint32 levIndex = GAME_INSTANCE->Get_TargetLevelIndex();
	desc.parentMatrix = m_Transform->Get_WorldMatrixPtr();
	desc.Owner = static_pointer_cast<ContainerObject>(shared_from_this());

	if (FAILED(Add_PartObject(levIndex, L"Pl0000Body", L"Pl0000Body", &desc)))
		return E_FAIL;
	if (FAILED(Add_PartObject(levIndex, L"WP0070Body", L"WP0070Body", &desc)))
		return E_FAIL;
	if (FAILED(Add_PartObject(levIndex, L"SheathWP0070Body", L"SheathWP0070Body", &desc)))
		return E_FAIL;
	if (FAILED(Add_PartObject(levIndex, L"WP0220Body", L"WP0220Body", &desc)))
		return E_FAIL;
	if (FAILED(Add_PartObject(levIndex, L"SheathWP0220Body", L"SheathWP0220Body", &desc)))
		return E_FAIL;
	if (FAILED(Add_PartObject(levIndex, L"WP3000Body", L"WP3000Body", &desc)))
		return E_FAIL;
	if (FAILED(Add_PartObject(levIndex, L"Pl0000EvadeChecker", L"Pl0000EvadeChecker", &desc)))
		return E_FAIL;
	if (FAILED(Add_PartObject(levIndex, L"Pl0000MonsterChecker", L"Pl0000MonsterChecker", &desc)))
		return E_FAIL;

	m_MainBody = static_pointer_cast<Pl0000Body>(Find_PartObject(L"Pl0000Body"));
	m_MonsterChecker = static_pointer_cast<Pl0000MonsterChecker>(Find_PartObject(L"Pl0000MonsterChecker"));

	m_LightWeapon = static_pointer_cast<WP0070Body>(Find_PartObject(L"WP0070Body"));
	m_SheathLightWeapon = static_pointer_cast<SheathWP0070Body>(Find_PartObject(L"SheathWP0070Body"));
	m_HeavyWeapon = static_pointer_cast<WP0220Body>(Find_PartObject(L"WP0220Body"));
	m_SheathHeavyWeapon = static_pointer_cast<SheathWP0220Body>(Find_PartObject(L"SheathWP0220Body"));

	auto wp3000 = static_pointer_cast<WP3000Body>(Find_PartObject(L"WP3000Body"));
	wp3000->Set_Pl0000Container(static_pointer_cast<Pl0000>(shared_from_this()));
	wp3000->Set_Pl0000Body(m_MainBody);

	WeaponHalo::WEAPON_HALO_DESC haloHeavyDesc{};
	haloHeavyDesc.Owner = static_pointer_cast<ContainerObject>(shared_from_this());
	haloHeavyDesc.parentMatrix = m_SheathHeavyWeapon->Get_CombinedWorldMatrix();
	haloHeavyDesc.localMatrix = Matrix::CreateRotationX(XMConvertToRadians(90.f)) *
		Matrix::CreateRotationZ(XMConvertToRadians(-25.f)) *
		Matrix::CreateTranslation(Vector3{ 0.15f, 0.f, 0.35f });
	if (FAILED(Add_PartObject(levIndex, L"WeaponHalo", L"WeaponHalo_Heavy", &haloHeavyDesc))) return E_FAIL;
	m_HeavyHalo = static_pointer_cast<WeaponHalo>(Find_PartObject(L"WeaponHalo_Heavy"));


	WeaponHalo::WEAPON_HALO_DESC haloLightDesc{};
	haloLightDesc.Owner = static_pointer_cast<ContainerObject>(shared_from_this());
	haloLightDesc.parentMatrix = m_SheathLightWeapon->Get_CombinedWorldMatrix();
	haloLightDesc.localMatrix = Matrix::CreateScale(0.75f, 0.75f, 0.75f) *
		Matrix::CreateRotationX(XMConvertToRadians(90.f)) *
		Matrix::CreateRotationY(XMConvertToRadians(20.f)) *
		Matrix::CreateTranslation(Vector3{ 0.1f, -0.05f, 0.075f });
	if (FAILED(Add_PartObject(levIndex, L"WeaponHalo", L"WeaponHalo_Light", &haloLightDesc))) return E_FAIL;
	m_LightHalo = static_pointer_cast<WeaponHalo>(Find_PartObject(L"WeaponHalo_Light"));

	
	// 
	
	return S_OK;
}

HRESULT Pl0000::Ready_Components()
{
	uint32 levIndex = GAME_INSTANCE->Get_TargetLevelIndex();
	m_Pl0000Input = Add_Component<Pl0000Input>(levIndex);
	if (nullptr == m_Pl0000Input)
		return E_FAIL;

	Pl0000Movement::PL0000_MOVEMENT_DESC movementDesc{};
	movementDesc.velocity = Vector3{0.f, 0.f, 0.f};
	movementDesc.moveSpeed = 0.f;
	movementDesc.targetDirection = Vector3::Zero;
	movementDesc.turnSpeed = 7.5f;
	m_Pl0000Movement = Add_Component<Pl0000Movement>(levIndex, &movementDesc);
	if (nullptr == m_Pl0000Movement)
		return E_FAIL;

	Navigation::NAVIGATION_DESC navDesc;
	navDesc.startCellIndex = 0;
	m_Navigation = Add_Component_Tag<Navigation>(ETOI(LEVEL::STATIC), L"CityOfRuinEntry", &navDesc);

	// StateMachine은 마지막에 처리
	if ((m_Pl0000States = Add_Component<Pl0000StateMachine>(levIndex)))
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
		if (FAILED(m_Pl0000States->Add_State(State2B_Hit::Create(
			Helper::To_wString(magic_enum::enum_name(PL0000_STATE::HIT)), pl0000))))
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

void Pl0000::Trigger_GlobalLag(Float timeScale, Float duration)
{
	if (m_IsGlobalLagActive && m_GlobalLagTimer >= duration)
		return;

	m_GlobalLagTimer = duration;
	m_IsGlobalLagActive = true;

	GAME_INSTANCE->Set_TimeScale(timeScale);
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
