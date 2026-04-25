#include "pch.h"
#include "MonsterShockWave.h"

#include <SpdLogger.h>
#include <Game.h>
#include <SphereCollider.h>
#include "Monster.h"
#include "PartObject.h"
#include "Pl0000.h"

MonsterShockWave::MonsterShockWave() : GameObject{} {}
MonsterShockWave::MonsterShockWave(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: GameObject{ device, context } {
}
MonsterShockWave::MonsterShockWave(const MonsterShockWave& rhs)
	: GameObject{ rhs }, m_PlayerLayerIndex{ rhs.m_PlayerLayerIndex } {
}

HRESULT MonsterShockWave::Initialize_Prototype()
{
	m_LayerMask.Set_Layer(L"MonsterAttack");
	m_PlayerLayerIndex = ETOI(GAME_INSTANCE->Get_LayerRegister()->Get_LayerByName(L"Player"));

	return GameObject::Initialize_Prototype();
}

HRESULT MonsterShockWave::Initialize(void* arg)
{
	if (FAILED(GameObject::Initialize(arg)))
		return E_FAIL;

	if (arg == nullptr) return E_FAIL;

	MONSTER_SHOCKWAVE_DESC* desc = static_cast<MONSTER_SHOCKWAVE_DESC*>(arg);
	m_Transform->Set_Position(desc->position);
	m_DamageInfo = desc->damageInfo;

	if (FAILED(Ready_Components(*desc)))
	{
		LOG_ERROR(L"Failed To Ready_Components : MonsterShockWave");
		return E_FAIL;
	}

	return S_OK;
}

void MonsterShockWave::On_Destroy()
{
	GameObject::On_Destroy();
}

void MonsterShockWave::Priority_Update(Float timeDelta)
{
	if (false == m_IsFirstFrame)
	{
		Destroy(shared_from_this());
		Set_Active(false);
	}

	if (m_IsFirstFrame)
	{
		m_IsFirstFrame = false;
	}
}

void MonsterShockWave::Update(Float timeDelta)
{

}

void MonsterShockWave::Late_Update(Float timeDelta)
{
	m_Transform->Update_WorldMatrix();
	m_ShockwaveCollider->Update(m_Transform->Get_WorldMatrix());
}
void MonsterShockWave::Fixed_Update(Float fixedDelta)
{

}

void MonsterShockWave::OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{

}

void MonsterShockWave::OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	if (m_IsDestroy) return;
	auto target = targetCollider->Get_Owner();

	if (target->Get_LayerMask().Get_Layer() != m_PlayerLayerIndex) return;
	uint32 targetID = target->Get_InstanceID();

	if (!m_HitEntities.contains(targetID))
	{
		m_HitEntities.insert(targetID);
		m_DamageInfo.hitPosition = targetCollider->ClosestPoint(ownCollider->Get_Pivot());

		auto entity = target;
		if (target->Get_GameObjectType() == GAMEOBJECTTYPE::PART)
		{
			entity = static_pointer_cast<PartObject>(target)->Get_Owner();
		}

		if (auto monster = static_pointer_cast<Pl0000>(entity))
		{
			monster->TakeDamage(m_DamageInfo);
		}
	}
}

void MonsterShockWave::OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{

}

HRESULT MonsterShockWave::Ready_Components(const MONSTER_SHOCKWAVE_DESC& desc)
{
	SphereCollider::SPHERE_COLLIDER_DESC colDesc{};
	colDesc.radius = desc.radius;
	colDesc.offset = Vector3::Zero;

	m_ShockwaveCollider = Add_Component<SphereCollider>(ETOI(LEVEL::STATIC), &colDesc);
	if (nullptr == m_ShockwaveCollider)
		return E_FAIL;

	return S_OK;
}

Shared<MonsterShockWave> MonsterShockWave::Create(const ComPtr<ID3D11Device>& device,
                                                  const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<MonsterShockWave>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : MonsterShockWave");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> MonsterShockWave::Clone(void* arg)
{
	auto instance = make_shared<MonsterShockWave>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : MonsterShockWave");
		return nullptr;
	}

	return instance;
}




