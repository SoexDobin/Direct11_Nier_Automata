#include "pch.h"
#include "TriggerObject.h"
#include "SphereCollider.h"
#include "Game.h"

TriggerObject::TriggerObject(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: GameObject(device, context) {}
TriggerObject::TriggerObject(const TriggerObject& prototype)
	: GameObject(prototype) {}

HRESULT TriggerObject::Initialize_Prototype()
{
	if (FAILED(GameObject::Initialize_Prototype()))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT TriggerObject::Initialize(void* arg)
{
	if (FAILED(GameObject::Initialize(arg)))
	{
		return E_FAIL;
	}

	if (arg == nullptr)
	{
		return E_FAIL;
	}

	TRIGGER_DESC& desc = *static_cast<TRIGGER_DESC*>(arg);

	m_Transform->Set_Position(desc.position);

	m_LevelIndex = desc.levelIndex;
	m_TargetLayerIndex = ETOI(GAME_INSTANCE->Get_LayerRegister()->Get_LayerByName(desc.targetLayerName));
	for (const auto& callback : desc.callbacks)
	{
		m_Callbacks.push_back(callback);
	}

	SphereCollider::SPHERE_COLLIDER_DESC colliderDesc;
	colliderDesc.radius = desc.radius;
	m_SphereCollider = Add_Component<Engine::SphereCollider>(m_LevelIndex, &colliderDesc);
	if (m_SphereCollider == nullptr)
	{
		return E_FAIL;
	}
	return S_OK;
}
void TriggerObject::Update(Float timeDelta)
{
	m_Transform->Update_WorldMatrix();
	if (m_SphereCollider != nullptr && m_Transform != nullptr)
	{
		m_SphereCollider->Update(m_Transform->Get_WorldMatrix());
	}
}
void TriggerObject::OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	if (m_IsTriggered) return;

	if (ownCollider == m_SphereCollider && !m_IsTriggered)
	{
		if (m_TargetLayerIndex != targetCollider->Get_Owner()->Get_LayerMask().Get_Layer()) return;
		
		int32 callbackIndex = 0;
		for (const auto& callback : m_Callbacks)
		{
			if (callback != nullptr)
			{
				std::wstring eventTag = L"TriggerEvent_Instance_" + std::to_wstring(reinterpret_cast<uint64_t>(this)) + L"_" + std::to_wstring(callbackIndex++);

				if (FAILED(GAME_INSTANCE->Add_Instance_Event(GAME_INSTANCE->Get_CurrentLevelIndex(), eventTag, callback)))
				{
					Destroy(shared_from_this());
					return;
				}
			}
		}
		m_IsTriggered = true;
		Destroy(shared_from_this());
	}
}

Shared<TriggerObject> TriggerObject::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<TriggerObject>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : TriggerObject");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> TriggerObject::Clone(void* arg)
{
	auto instance = make_shared<TriggerObject>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : TriggerObject");
		return nullptr;
	}

	return instance;
}
