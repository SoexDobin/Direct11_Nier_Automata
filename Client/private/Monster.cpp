#include "pch.h"
#include "Monster.h"

#include <Game.h>
#include <Random_Helper.h>
#include <Transform.h>

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
		if (dmgInfo.attackType == ATK_TYPE::LIGHT)
			GAME_INSTANCE->PlaySoundFXOnce(L"SwordHit1", SOUNDCHANNEL::CHANNEL_20, 0.5f);
		else if (dmgInfo.attackType == ATK_TYPE::HEAVY)
			GAME_INSTANCE->PlaySoundFXOnce(L"BigSwordHit1", SOUNDCHANNEL::CHANNEL_20, 0.5f);
		break;
	case 1:
		if (dmgInfo.attackType == ATK_TYPE::LIGHT)
			GAME_INSTANCE->PlaySoundFXOnce(L"SwordHit2", SOUNDCHANNEL::CHANNEL_21, 0.5f);
		else if (dmgInfo.attackType == ATK_TYPE::HEAVY)
			GAME_INSTANCE->PlaySoundFXOnce(L"BigSwordHit2", SOUNDCHANNEL::CHANNEL_21, 0.5f);
		break;
	case 2:
		if (dmgInfo.attackType == ATK_TYPE::LIGHT)
			GAME_INSTANCE->PlaySoundFXOnce(L"SwordHit3", SOUNDCHANNEL::CHANNEL_22, 0.5f);
		else if (dmgInfo.attackType == ATK_TYPE::HEAVY)
			GAME_INSTANCE->PlaySoundFXOnce(L"BigSwordHit3", SOUNDCHANNEL::CHANNEL_22, 0.5f);
		break;
	case 3:
		if (dmgInfo.attackType == ATK_TYPE::LIGHT)
			GAME_INSTANCE->PlaySoundFXOnce(L"SwordHit4", SOUNDCHANNEL::CHANNEL_23, 0.5f);
		else if (dmgInfo.attackType == ATK_TYPE::HEAVY)
			GAME_INSTANCE->PlaySoundFXOnce(L"BigSwordHit4", SOUNDCHANNEL::CHANNEL_23, 0.5f);
		break;
	default:
		if (dmgInfo.attackType == ATK_TYPE::LIGHT)
			GAME_INSTANCE->PlaySoundFXOnce(L"SwordHit5", SOUNDCHANNEL::CHANNEL_24, 0.5f);
		else if (dmgInfo.attackType == ATK_TYPE::HEAVY)
			GAME_INSTANCE->PlaySoundFXOnce(L"BigSwordHit5", SOUNDCHANNEL::CHANNEL_24, 0.5f);
		break;
	}
	
}

