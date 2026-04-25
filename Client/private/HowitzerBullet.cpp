#include "pch.h"
#include "HowitzerBullet.h"

#include "Game.h"
#include <SpdLogger.h>
#include <SphereCollider.h>
#include <Model.h>

#include "Bullet.h"
#include "ExplodeEffect.h"
#include "MonsterShockWave.h"
#include "Pl0000.h"

HowitzerBullet::HowitzerBullet(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Bullet{device, context} {}
HowitzerBullet::HowitzerBullet(const HowitzerBullet& rhs)
	: Bullet{rhs} {}


HRESULT HowitzerBullet::Initialize_Prototype()
{
	return Bullet::Initialize_Prototype();
}

HRESULT HowitzerBullet::Initialize(void* arg)
{
	if (FAILED(Bullet::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize HowitzerBullet");
		return E_FAIL;
	}

	if (arg != nullptr)
	{
		auto* howitzerArg = static_cast<HOWITZER_BULLET_DESC*>(arg);
		m_GravityStrength = howitzerArg->gravityStrength;
		m_TargetY = howitzerArg->targetY;
	}

	m_CurvedVelocity = m_Desc.direction * m_Desc.speed;
	m_CurvedVelocity.y += m_GravityStrength * 0.75f;

	m_Transform->Set_Scale(0.75f, 0.75f, 0.75f);

	return S_OK;
}
void HowitzerBullet::Update(Float timeDelta)
{
	if (m_IsDestroy) return;

	m_CurvedVelocity.y -= m_GravityStrength * timeDelta;
	Vector3 currentPos = m_Transform->Get_Position();
	currentPos += m_CurvedVelocity * timeDelta;
	m_Transform->Set_Position(currentPos);
	
	if (m_CurvedVelocity.LengthSquared() > 0.001f)
		m_Transform->LookAt(currentPos + m_CurvedVelocity);
	
	if (currentPos.y <= m_TargetY)
	{
		currentPos.y = m_TargetY;
		m_Transform->Set_Position(currentPos);

		InstanceExplodeEffect();

		Object::Destroy(shared_from_this());
		return;
	}
	m_Collider->Update(m_Transform->Get_WorldMatrix());
	m_Transform->Update_WorldMatrix();
}

void HowitzerBullet::Late_Update(Float timeDelta)
{

}

HRESULT HowitzerBullet::Render()
{
	return Bullet::Render();
}

void HowitzerBullet::Submit_RenderGroup()
{
	Bullet::Submit_RenderGroup();
}

void HowitzerBullet::OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	auto target = targetCollider->Get_Owner();

	if (target->Get_GameObjectType() != GAMEOBJECTTYPE::PART) return;
	auto entity = static_pointer_cast<PartObject>(target)->Get_Owner();

	if (m_TargetLayerIndex == m_PlayerLayerIndex)
	{
		m_DamageInfo.hitPosition = targetCollider->ClosestPoint(ownCollider->Get_Pivot());
		auto player = static_pointer_cast<Pl0000>(entity);
		player->TakeDamage(m_DamageInfo);
		InstanceExplodeEffect();
		Destroy(shared_from_this());
	}
}
void HowitzerBullet::InstanceExplodeEffect()
{
	uint32 levIndex = GAME_INSTANCE->Get_TargetLevelIndex();


	ExplodeEffect::EXPLODE_EFFECT_DESC explodeDesc{};
	explodeDesc.textureTag = L"Effect_Explode";
	explodeDesc.position = m_Transform->Get_Position();
	explodeDesc.scale = Vector3{ 4.f, 4.f, 4.f };
	explodeDesc.threshold = 0.04f;
	GAME_INSTANCE->Instantiate<ExplodeEffect>(L"ExplodeEffect", levIndex, &explodeDesc);

	{
		Entity::DAMAGE_INFO dmgInfo{};
		dmgInfo.attacker = m_Desc.damageInfo.attacker;
		dmgInfo.damage = 20.f;
		dmgInfo.groggyWeight = 20.f;
		dmgInfo.attackType = ATK_TYPE::HEAVY;
		dmgInfo.hitPosition = m_Transform->Get_Position();
		dmgInfo.hitRotation = Quaternion::Identity;
		dmgInfo.knockbackForce = 1.25f;

		MonsterShockWave::MONSTER_SHOCKWAVE_DESC desc{};
		desc.damageInfo = dmgInfo;
		desc.position = m_Transform->Get_Position();
		desc.radius = 1.f;

		GAME_INSTANCE->Instantiate<MonsterShockWave>(L"MonsterShockWave", levIndex, &desc);
	}

	GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_26);
	GAME_INSTANCE->PlaySoundFXOnce(L"Howitzer_Explode", SOUNDCHANNEL::CHANNEL_26, 0.4f);
}

Shared<HowitzerBullet> HowitzerBullet::Create(const ComPtr<ID3D11Device>& device,
                                              const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<HowitzerBullet>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : HowitzerBullet");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> HowitzerBullet::Clone(void* arg)
{
	auto instance = make_shared<HowitzerBullet>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : HowitzerBullet");
		return nullptr;
	}

	return instance;
}


