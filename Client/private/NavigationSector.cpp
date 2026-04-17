#include "pch.h"
#include "NavigationSector.h"

#include <Game.h>
#include <Entity.h>
#include <Navigation.h>
#include <SpdLogger.h>
#include <Navigation.h>
#include "AABBCollider.h"

NavigationSector::NavigationSector(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: GameObject{device, context} {}
NavigationSector::NavigationSector(const NavigationSector& rhs)
	: GameObject{rhs} {}

HRESULT NavigationSector::Initialize_Prototype()
{
	m_TargetLayerIndex.emplace(ETOI(GAME_INSTANCE->Get_LayerRegister()->Get_LayerByName(L"MonsterPhysical")));
	m_TargetLayerIndex.emplace(ETOI(GAME_INSTANCE->Get_LayerRegister()->Get_LayerByName(L"PlayerPhysical")));

	return GameObject::Initialize_Prototype();
}

HRESULT NavigationSector::Initialize(void* arg)
{
	if (FAILED(GameObject::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Create NavigationSector");
		return E_FAIL;
	}

	if (arg)
	{
		NAVIGATION_COLLISION_DESC& desc = *static_cast<NAVIGATION_COLLISION_DESC*>(arg);

		m_Navigation = GAME_INSTANCE->Instantiate<Navigation>(desc.navTag, ETOI(LEVEL::STATIC), &desc);
		AABBCollider::AABB_COLLIDER_DESC colDesc{};
		colDesc.offset = Vector3::Zero;
		colDesc.extents = desc.collisionExtends;
		m_AABBCollider = Add_Component<AABBCollider>(ETOI(LEVEL::STATIC), &colDesc);
		if (nullptr == m_AABBCollider)
			return E_FAIL;

		m_Transform->Set_Position(desc.worldPosition);
	}
	else
	{
		AABBCollider::AABB_COLLIDER_DESC colDesc{};
		colDesc.offset = Vector3::Zero;
		colDesc.extents = Vector3{ 0.5f, 0.5f, 0.5f };
		m_AABBCollider = Add_Component<AABBCollider>(ETOI(LEVEL::STATIC), &colDesc);
		if (nullptr == m_AABBCollider)
			return E_FAIL;

		m_Transform->Set_Position(Vector3::Zero);
	}
	
	m_Transform->Update_WorldMatrix();
	m_AABBCollider->Update(m_Transform->Get_WorldMatrix());

	return S_OK;
}

void NavigationSector::On_Destroy()
{
	GameObject::On_Destroy();
}


void NavigationSector::OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	if (m_Navigation == nullptr) return;

	auto target = targetCollider->Get_Owner();

	if (target->Get_GameObjectType() != GAMEOBJECTTYPE::CONTAINER) return;
	if (m_TargetLayerIndex.contains(ETOI(target->Get_LayerMask().Get_Layer()) == false)) return;

	if (auto entity = static_pointer_cast<Entity>(target))
	{
		if (nullptr == entity->Get_Navigation()) return;
		if (entity->Get_Navigation()->Get_InstanceID() == m_Navigation->Get_InstanceID())
		{
			LOG_ERROR(L"Enter Same Navigation Sector"); 
			return;
		}
			
		LOG_INFO(L"Enter Navigation Sector {}", m_ObjectName);
		entity->Set_Navigation(m_Navigation);
	}
}

void NavigationSector::OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{

}

void NavigationSector::OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{

}

Shared<NavigationSector> NavigationSector::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<NavigationSector>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : NavigationSector");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> NavigationSector::Clone(void* arg)
{
	auto instance = make_shared<NavigationSector>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : NavigationSector");
		return nullptr;
	}

	return instance;
}


