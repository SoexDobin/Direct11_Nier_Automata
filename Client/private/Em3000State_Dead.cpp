#include "pch.h"
#include "StateEm3000_Dead.h"
#include <Game.h>
#include <Transform.h>
#include "Em3000.h"
#include "Em3000Body.h"
#include "ExplodeEffect_Instance.h"
#include "Random_Helper.h"

StateEm3000_Dead::StateEm3000_Dead(const wstring& tag, const Shared<Em3000>& owner)
	: StateEm3000{ tag, owner }
{
}
HRESULT StateEm3000_Dead::Initialize()
{
	if (FAILED(StateEm3000::Initialize()))
		return E_FAIL;
	return S_OK;
}
Bool StateEm3000_Dead::StateEnterInvoke()
{
	m_TotalSunkY = 0.f;
	m_ExplosionTimer = 0.f;
	return true;
}
void StateEm3000_Dead::Update(Float timeDelta)
{
	auto owner = m_Owner.lock();
	if (!owner) return;
	auto transform = owner->Get_Transform();
	
	Vector3 pos = transform->Get_Position();
	Float deltaY = m_SinkingSpeed * timeDelta;
	pos.y -= deltaY;
	m_TotalSunkY += deltaY;
	transform->Set_Position(pos);
	
	m_ExplosionTimer += timeDelta;
	if (m_ExplosionTimer >= m_ExplosionInterval)
	{
		m_ExplosionTimer = 0.f;
		uint32 levIndex = GAME_INSTANCE->Get_TargetLevelIndex();
		ExplodeEffect_Instance::EXPLODE_EFFECT_INSTANCE_DESC effectDesc{};

		// 보스 주변 랜덤 위치 계산
		Float randX = Helper::Random_Float(-2.5f, 2.5f);
		Float randY = Helper::Random_Float(0.f, 4.f);
		Float randZ = Helper::Random_Float(-2.5f, 2.5f);

		effectDesc.position = pos + Vector3{ randX, randY, randZ };
		effectDesc.instanceCount = 3;
		effectDesc.range = Vector3{ 0.1f, 0.1f, 0.1f };
		effectDesc.scaleRange = Vector2{ 2.f, 4.f };
		effectDesc.textureTag = L"Effect_Explode2";
		GAME_INSTANCE->Instantiate<ExplodeEffect_Instance>(L"ExplodeEffect_Instance", levIndex, &effectDesc);

		GAME_INSTANCE->PlaySoundFXOnce(L"Explode2", SOUNDCHANNEL::CHANNEL_27, 0.2f);
	}
	
	if (m_TotalSunkY >= 10.f)
	{
		owner->Destroy(owner);
	}
}

void StateEm3000_Dead::Late_Update(Float timeDelta)
{
}

void StateEm3000_Dead::StateExitInvoke()
{
}

Shared<StateEm3000_Dead> StateEm3000_Dead::Create(const wstring& tag, const Shared<Em3000>& owner)
{
	auto instance = make_shared<StateEm3000_Dead>(tag, owner);
	if (FAILED(instance->Initialize()))
	{
		return nullptr;
	}
	return instance;
}
