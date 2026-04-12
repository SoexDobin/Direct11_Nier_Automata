#include "pch.h"
#include "Monster.h"

#include <Game.h>
#include <Random_Helper.h>
#include <Transform.h>

#include "SparkEffect.h"
#include "SpdLogger.h"
#include "Collider.h"

Monster::Monster() : Entity{} {}
Monster::Monster(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Entity{device, context} {}
Monster::Monster(const Monster& rhs)
	: Entity{rhs} {}

HRESULT Monster::Initialize_Prototype()
{
	m_LayerMask.Set_Layer(L"MonsterPhysical");
	m_TagMask.Set_Tag({ L"Monster" });

	return Entity::Initialize_Prototype();
}

HRESULT Monster::Initialize(void* arg)
{
	if (FAILED(Entity::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize Monster {}", m_ObjectName);
		return E_FAIL;
	}

	return S_OK;
}

void Monster::On_Destroy()
{
	m_HpBarUI.reset();
	Entity::On_Destroy();
}

void Monster::On_Enable()
{
	Entity::On_Enable();
}

void Monster::On_Disable()
{
	Entity::On_Disable();
}

void Monster::Priority_Update(Float timeDelta)
{

}

void Monster::Update(Float timeDelta)
{
	
}

void Monster::Late_Update(Float timeDelta)
{
}

void Monster::Fixed_Update(Float fixedDelta)
{
	
}

HRESULT Monster::Render()
{
	return S_OK;
}

void Monster::Submit_RenderGroup()
{
}

void Monster::OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{

}

void Monster::OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	auto target = targetCollider->Get_Owner();

	if (target->Get_GameObjectType() != GAMEOBJECTTYPE::CONTAINER) return;

	auto targetLayerName = target->Get_LayerMask().Get_LayerName();
	if (targetLayerName == L"PlayerPhysical")
	{
		Vector3 pushDir = PushoutDelta(ownCollider, targetCollider, 1.0f);
		Apply_PushoutCorrection(pushDir);
	}
	if (targetLayerName == L"MonsterPhysical")
	{
		Vector3 pushDir = PushoutDelta(ownCollider, targetCollider, 0.5f);
		Apply_PushoutCorrection(pushDir);
	}
}

void Monster::OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{

}

void Monster::TakeDamage(const DAMAGE_INFO& dmgInfo)
{
	if (m_TargetPlayer.expired())
		m_TargetPlayer = dmgInfo.attacker;

	Entity::TakeDamage(dmgInfo);
}

Bool Monster::Is_TargetFront() const
{
	if (m_TargetPlayer.expired()) return false;
	
	Vector3 pos = m_Transform->Get_Look();
	Vector3 targetPos = m_TargetPlayer.lock()->Get_Transform()->Get_Position();
	Vector3 toTarget = targetPos - m_Transform->Get_Position();

	Float scalar = pos.Dot(toTarget);

	return scalar > 0.f;
}

Vector3 Monster::Get_DirectionToTarget() const
{
	if (m_TargetPlayer.expired()) return Vector3::Zero;

	Vector3 currentPos = m_Transform->Get_Position();
	Vector3 targetPos = m_TargetPlayer.lock()->Get_Transform()->Get_Position();
	Vector3 toTarget = targetPos - currentPos;
	toTarget.y = 0.f;

	if (toTarget.Length() < 0.001f) return Vector3::Zero;

	toTarget.Normalize();
	return toTarget;
}

Float Monster::Get_DistanceToTarget() const
{
	if (m_TargetPlayer.expired()) return 0;

	Vector3 currentPos = m_Transform->Get_Position();
	Vector3 targetPos = m_TargetPlayer.lock()->Get_Transform()->Get_Position();
	Vector3 toTarget = targetPos - currentPos;
	toTarget.y = 0.f;

	return toTarget.Length();
}

void Monster::Play_HitSFX(const DAMAGE_INFO& dmgInfo) const
{
	switch (int32 rand = Helper::Random_Int(0, 4))
	{
	case 0:
		GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_20);
		if (dmgInfo.attackType == ATK_TYPE::LIGHT)
			GAME_INSTANCE->PlaySoundFXOnce(L"SwordHit1", SOUNDCHANNEL::CHANNEL_20, 0.3f);
		else if (dmgInfo.attackType == ATK_TYPE::HEAVY)
			GAME_INSTANCE->PlaySoundFXOnce(L"BigSwordHit1", SOUNDCHANNEL::CHANNEL_20, 0.3f);
		else if (dmgInfo.attackType == ATK_TYPE::POD)
			GAME_INSTANCE->PlaySoundFXOnce(L"BulletHit1", SOUNDCHANNEL::CHANNEL_20, 0.3f);
		break;
	case 1:
		GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_21);
		if (dmgInfo.attackType == ATK_TYPE::LIGHT )
			GAME_INSTANCE->PlaySoundFXOnce(L"SwordHit2", SOUNDCHANNEL::CHANNEL_21, 0.3f);
		else if (dmgInfo.attackType == ATK_TYPE::HEAVY)
			GAME_INSTANCE->PlaySoundFXOnce(L"BigSwordHit2", SOUNDCHANNEL::CHANNEL_21, 0.3f);
		else if (dmgInfo.attackType == ATK_TYPE::POD)
			GAME_INSTANCE->PlaySoundFXOnce(L"BulletHit2", SOUNDCHANNEL::CHANNEL_21, 0.3f);
		break;
	case 2:
		GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_22);
		if (dmgInfo.attackType == ATK_TYPE::LIGHT)
			GAME_INSTANCE->PlaySoundFXOnce(L"SwordHit3", SOUNDCHANNEL::CHANNEL_22, 0.3f);
		else if (dmgInfo.attackType == ATK_TYPE::HEAVY)
			GAME_INSTANCE->PlaySoundFXOnce(L"BigSwordHit3", SOUNDCHANNEL::CHANNEL_22, 0.3f);
		else if (dmgInfo.attackType == ATK_TYPE::POD)
			GAME_INSTANCE->PlaySoundFXOnce(L"BulletHit3", SOUNDCHANNEL::CHANNEL_22, 0.3f);
		break;
	case 3:
		GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_23);
		if (dmgInfo.attackType == ATK_TYPE::LIGHT)
			GAME_INSTANCE->PlaySoundFXOnce(L"SwordHit4", SOUNDCHANNEL::CHANNEL_23, 0.3f);
		else if (dmgInfo.attackType == ATK_TYPE::HEAVY)
			GAME_INSTANCE->PlaySoundFXOnce(L"BigSwordHit4", SOUNDCHANNEL::CHANNEL_23, 0.3f);
		else if (dmgInfo.attackType == ATK_TYPE::POD)
			GAME_INSTANCE->PlaySoundFXOnce(L"BulletHit4", SOUNDCHANNEL::CHANNEL_23, 0.3f);
		break;
	default:
		GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_24);
		if (dmgInfo.attackType == ATK_TYPE::LIGHT)
			GAME_INSTANCE->PlaySoundFXOnce(L"SwordHit5", SOUNDCHANNEL::CHANNEL_24, 0.3f);
		else if (dmgInfo.attackType == ATK_TYPE::HEAVY)
			GAME_INSTANCE->PlaySoundFXOnce(L"BigSwordHit5", SOUNDCHANNEL::CHANNEL_24, 0.3f);
		else if (dmgInfo.attackType == ATK_TYPE::POD)
			GAME_INSTANCE->PlaySoundFXOnce(L"BulletHit1", SOUNDCHANNEL::CHANNEL_24, 0.3f);
		break;
	}
	
}

void Monster::DisplaySparkEffect(ATK_TYPE atkType, Vector3 position, Quaternion rotation) const
{
	SparkEffect::SPARK_EFFECT_DESC sparkDesc{};
	sparkDesc.atkType = atkType;
	sparkDesc.position = position;
	sparkDesc.rotation = rotation;
	GAME_INSTANCE->Instantiate<SparkEffect>(L"SparkEffect", ETOI(LEVEL::GAMEPLAY), &sparkDesc);
}

