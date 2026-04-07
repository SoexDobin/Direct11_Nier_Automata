#include "pch.h"
#include "Monster.h"

#include <Game.h>
#include <Random_Helper.h>
#include <Transform.h>

#include "SparkEffect.h"
#include "SpdLogger.h"

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
	GameObject::On_Destroy();
}

void Monster::On_Enable()
{
	GameObject::On_Enable();
}

void Monster::On_Disable()
{
	GameObject::On_Disable();
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

