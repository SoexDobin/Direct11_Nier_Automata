#include "pch.h"
#include "Pl0000Shockwave.h"

#include <SpdLogger.h>
#include <Game.h>
#include <SphereCollider.h>
#include "Monster.h"
#include "PartObject.h"

Pl0000Shockwave::Pl0000Shockwave() : GameObject{} {}
Pl0000Shockwave::Pl0000Shockwave(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: GameObject{ device, context } {
}
Pl0000Shockwave::Pl0000Shockwave(const Pl0000Shockwave& rhs)
	: GameObject{ rhs } {
}

HRESULT Pl0000Shockwave::Initialize_Prototype()
{
	m_LayerMask.Set_Layer(L"PlayerWeapon");
	m_TagMask.Set_Tag({ L"PlayerWeapon" });

	return GameObject::Initialize_Prototype();
}

HRESULT Pl0000Shockwave::Initialize(void* arg)
{
	if (FAILED(GameObject::Initialize(arg)))
		return E_FAIL;

	if (arg == nullptr) return E_FAIL;

	PLAYER_SHOCKWAVE_DESC* desc = static_cast<PLAYER_SHOCKWAVE_DESC*>(arg);
	m_Transform->Set_Position(desc->position);
	m_DamageInfo = desc->damageInfo;

	if (FAILED(Ready_Components(*desc)))
	{
		LOG_ERROR(L"Failed To Ready_Components : PlayerShockwave");
		return E_FAIL;
	}

	return S_OK;
}

void Pl0000Shockwave::On_Destroy()
{
	GameObject::On_Destroy();
}

void Pl0000Shockwave::Priority_Update(Float timeDelta)
{
	if (false == isFirstFrame)
	{
		Destroy(shared_from_this());
		Set_Active(false);
	}

	if (isFirstFrame)
	{
		isFirstFrame = false;
	}
}

void Pl0000Shockwave::Update(Float timeDelta)
{
	
}

void Pl0000Shockwave::Late_Update(Float timeDelta)
{
	m_Transform->Update_WorldMatrix();
	m_ShockwaveCollider->Update(m_Transform->Get_WorldMatrix());
}
void Pl0000Shockwave::Fixed_Update(Float fixedDelta)
{

}

void Pl0000Shockwave::OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{

}

void Pl0000Shockwave::OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	if (m_IsDestroy) return;
	auto target = targetCollider->Get_Owner();
	
	if (target->Get_LayerMask().Get_LayerName() != L"Monster") return;
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

		if (auto monster = static_pointer_cast<Monster>(entity))
		{
			monster->TakeDamage(m_DamageInfo);
		}
	}
}

void Pl0000Shockwave::OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	
}

HRESULT Pl0000Shockwave::Ready_Components(const PLAYER_SHOCKWAVE_DESC& desc)
{
	SphereCollider::SPHERE_COLLIDER_DESC colDesc{};
	colDesc.radius = desc.radius;
	colDesc.offset = Vector3::Zero;

	m_ShockwaveCollider = Add_Component<SphereCollider>(ETOI(LEVEL::STATIC), &colDesc);
	if (nullptr == m_ShockwaveCollider)
		return E_FAIL;

	return S_OK;
}

Shared<Pl0000Shockwave> Pl0000Shockwave::Create(const ComPtr<ID3D11Device>& device,
                                                const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<Pl0000Shockwave>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Pl0000Shockwave");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> Pl0000Shockwave::Clone(void* arg)
{
	auto instance = make_shared<Pl0000Shockwave>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : Pl0000Shockwave");
		return nullptr;
	}

	return instance;
}


