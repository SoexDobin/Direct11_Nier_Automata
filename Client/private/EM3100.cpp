#include "pch.h"
#include "Em3100.h"

#include <Game.h>
#include <Navigation.h>

#include "Bullet.h"
#include "Em3100Body.h"
#include "HpBarWorldUI.h"
#include "SphereCollider.h"
#include "ExplodeEffect_Instance.h"
#include "SpdLogger.h"
#include "Random_Helper.h"


Em3100::Em3100() : Monster{} {}
Em3100::Em3100(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Monster{device, context} {}
Em3100::Em3100(const Em3100& rhs)
	: Monster{rhs} {}

HRESULT Em3100::Initialize_Prototype()
{
	if (FAILED(Monster::Initialize_Prototype()))
	{
		LOG_ERROR(L"Failed to Create Prototype Monster");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT Em3100::Initialize(void* arg)
{
	if (FAILED(Monster::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize em3100");
		return E_FAIL;
	}

	if (FAILED(Ready_PartObjects()))
	{
		LOG_ERROR(L"Failed to Ready PartObjects em3100 ");
		return E_FAIL;
	}

	if (FAILED(Ready_Components()))
	{
		LOG_ERROR(L"Failed to Ready Components em3100 ");
		return E_FAIL;
	}

	uint32 levIndex = GAME_INSTANCE->Get_TargetLevelIndex();
	GAME_INSTANCE->Add_Instance_Event(levIndex, L"Add_HpBar", [this, levIndex]()
		{
			HpBarWorldUI::HP_BAR_WORLD_UI_DESC UI_hpDesc{};
			UI_hpDesc.target = static_pointer_cast<Entity>(shared_from_this());
			UI_hpDesc.worldOffset = Vector3{ 0.f, 1.f, 0.f };
			UI_hpDesc.anchor = UI_ANCHOR::TOP_LEFT;
			UI_hpDesc.sizeX = 200.f;
			UI_hpDesc.sizeY = 10.f;
			UI_hpDesc.x = 0.f;
			UI_hpDesc.y = 0.f;
			m_HpBarUI = GAME_INSTANCE->Instantiate<HpBarWorldUI>(L"HpBarWorldUI", levIndex, &UI_hpDesc);
		});

	if (arg != nullptr)
	{
		EM3100_CONTAINER_DESC* desc = static_cast<EM3100_CONTAINER_DESC*>(arg);
		m_TargetY = desc->targetY;
		m_TargetPlayer = GAME_INSTANCE->Find_ByInstanceID(GAME_INSTANCE->Get_TargetLevelIndex(), desc->playerInstanceID);
	}
	
	Float randX = Helper::Random_Float(390.f, 420.f);
	Float randZ = Helper::Random_Float(34.f, 70.f);
	m_Transform->Set_Position(Vector3(randX, 50.f, randZ));
	Vector3 lookTarget = Vector3(400.f, 50.f, 52.f);
	m_Transform->LookAt(lookTarget);

	m_Hp = 500.f;
	m_MaxHp = 500.f;

	return S_OK;
}

void Em3100::Priority_Update(Float timeDelta)
{

}

void Em3100::Update(Float timeDelta)
{
	if (Is_Dead() && !m_IsStartingDeath)
	{
		OnDeath();
		return;
	}

	if (m_IsStartingDeath)
	{
		Vector3 pos = m_Transform->Get_Position();
		Float downSpeed = 2.f;
		Float deltaY = downSpeed * timeDelta;
		pos.y -= deltaY;
		m_DeathTotalDownY += deltaY;

		m_Transform->Set_Position(pos);

		if (m_DeathTotalDownY >= 10.f)
		{
			Destroy(shared_from_this());
		}
		
		m_Transform->Update_WorldMatrix();
		return;
	}

	if (!m_IsLanded)
	{
		Vector3 curPos = m_Transform->Get_Position();
		curPos.y -= 30.f * timeDelta; // 떨어지는 속도 (필요에 따라 조절)
		if (curPos.y <= m_TargetY)
		{
			curPos.y = m_TargetY;
			m_IsLanded = true;
		}
		m_Transform->Set_Position(curPos);
	}
	else
	{
		m_AttackTimer += timeDelta;
		if (m_AttackTimer >= 5.f)
		{
			m_AttackTimer = 0.f;
			if (auto target = m_TargetPlayer.lock())
			{
				Vector3 pos = m_Transform->Get_Position() + Vector3{0.f, 1.75f, 0.f};
				Vector3 bulletSpawnPos = pos;
				bulletSpawnPos.y += 1.5f;
				// 총알 조준: 플레이어의 실제 높이를 반영 (기울어짐과 무관함)
				Vector3 targetHitPos = target->Get_Transform()->Get_Position();
				targetHitPos.y = pos.y;

				Vector3 dir = targetHitPos - bulletSpawnPos;
				dir.Normalize();
				
				Bullet::BULLET_DESC bulletDesc{};
				bulletDesc.initialPosition = pos;
				bulletDesc.direction = dir;
				bulletDesc.maxDistance = 50.f;
				bulletDesc.targetLayer = L"Player";
				bulletDesc.damageInfo.damage = 10.f; 
				bulletDesc.damageInfo.attacker = shared_from_this();
				bulletDesc.resourceTag = L"candy"; 
				bulletDesc.speed = 10.f;
				bulletDesc.scale = Vector3{ 0.5f, 0.5f, 0.5f };
				
				GAME_INSTANCE->Instantiate<Bullet>(L"Bullet", GAME_INSTANCE->Get_TargetLevelIndex(), &bulletDesc);
			}
		}
	}
	m_Transform->Update_WorldMatrix();
	m_InteractionZone->Update(m_Transform->Get_WorldMatrix());
}

void Em3100::Late_Update(Float timeDelta)
{
	
}

void Em3100::Fixed_Update(Float fixedDelta)
{
	
}

HRESULT Em3100::Render()
{
	return S_OK;
}

void Em3100::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

void Em3100::TakeDamage(const DAMAGE_INFO& dmgInfo)
{
	Play_HitSFX(dmgInfo);
	DisplaySparkEffect(dmgInfo.attackType, dmgInfo.hitPosition, dmgInfo.hitRotation);

	Entity::TakeDamage(dmgInfo);
}

void Em3100::OnDeath()
{
	Start_Death();
	m_MainBody->Off_Collider();
}

void Em3100::Start_Death()
{
	if (m_IsStartingDeath) return;

	m_IsStartingDeath = true;

	uint32 levIndex = GAME_INSTANCE->Get_TargetLevelIndex();

	ExplodeEffect_Instance::EXPLODE_EFFECT_INSTANCE_DESC effectDesc{};
	effectDesc.position = m_Transform->Get_Position();
	effectDesc.instanceCount = 5;
	effectDesc.range = Vector3{ 0.02f, 0.02f, 0.02f }; 
	effectDesc.scaleRange = Vector2{ 5.f, 8.f }; 
	effectDesc.textureTag = L"Effect_Explode2";
	
	auto explode = GAME_INSTANCE->Instantiate<ExplodeEffect_Instance>(L"ExplodeEffect_Instance", levIndex, &effectDesc);
	if (explode == nullptr)
		LOG_ERROR(L"Failed Create explode At Em3100");

	GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_27);
	GAME_INSTANCE->PlaySoundFXOnce(L"Explode1", SOUNDCHANNEL::CHANNEL_27, 0.4f);

	if (m_InteractionZone)
		m_InteractionZone->Set_Active(false);

	if (m_HpBarUI)
		m_HpBarUI->Set_Active(false);
}

void Em3100::OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	
}

void Em3100::OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{

}

void Em3100::OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	
}


HRESULT Em3100::Ready_PartObjects()
{
	Em3100Body::EM3100BODY_DESC em3100BodyDesc{};
	em3100BodyDesc.parentMatrix = m_Transform->Get_WorldMatrixPtr();
	em3100BodyDesc.Owner = static_pointer_cast<ContainerObject>(shared_from_this());

	if (FAILED(Add_PartObject(GAME_INSTANCE->Get_TargetLevelIndex(), L"Em3100Body", L"Em3100Body", &em3100BodyDesc)))
		return E_FAIL;
	m_MainBody = static_pointer_cast<Em3100Body>(Find_PartObject(L"Em3100Body"));

	return S_OK;
}

HRESULT Em3100::Ready_Components()
{
	Navigation::NAVIGATION_DESC navDesc;
	navDesc.startCellIndex = 0;
	m_Navigation = Add_Component_Tag<Navigation>(ETOI(LEVEL::STATIC), L"CityOfRuinEntry", &navDesc);
	if (nullptr == m_Navigation)
		return E_FAIL;

	SphereCollider::SPHERE_COLLIDER_DESC sphereDesc{};
	sphereDesc.radius = 2.f;
	sphereDesc.offset = Vector3::UnitY;
	m_InteractionZone = Add_Component<SphereCollider>(ETOI(LEVEL::STATIC), &sphereDesc);
	if (nullptr == m_InteractionZone)
		return E_FAIL;

	return S_OK;
}

Shared<Em3100> Em3100::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<Em3100>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : EM3100");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> Em3100::Clone(void* arg)
{
	auto instance = make_shared<Em3100>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : EM3100");
		return nullptr;
	}

	return instance;
}


