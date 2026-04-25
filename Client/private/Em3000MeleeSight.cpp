#include "pch.h"
#include "Em3000MeleeSight.h"

#include <ContainerObject.h>
#include <Game.h>

#include "SphereCollider.h"
#include "SpdLogger.h"

Em3000MeleeSight::Em3000MeleeSight(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: PartObject{device, context} {}
Em3000MeleeSight::Em3000MeleeSight(const Em3000MeleeSight& rhs)
	: PartObject{rhs} {}

HRESULT Em3000MeleeSight::Initialize_Prototype()
{
	m_LayerMask.Set_Layer(L"Sight");
	return PartObject::Initialize_Prototype();
}

HRESULT Em3000MeleeSight::Initialize(void* arg)
{
	if (nullptr == arg)
	{
		LOG_ERROR(L"Failed to Initialize Em3000MeleeSight");
		return E_FAIL;
	}

	if (FAILED(PartObject::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize Em3000MeleeSight");
		return E_FAIL;
	}

	auto& sightDesc = *static_cast<MONSTER_SIGHT_DESC*>(arg);

	SphereCollider::SPHERE_COLLIDER_DESC sphereColDesc{};
	sphereColDesc.radius = sightDesc.radius;
	sphereColDesc.offset = sightDesc.offset;
	m_Sight = Add_Component<SphereCollider>(ETOI(LEVEL::STATIC), &sphereColDesc);

	if (nullptr == m_Sight)
	{
		LOG_ERROR(L"Failed to Init Em3000MeleeSight Collider Component");
		return E_FAIL;
	}

	m_PlayerPhysicalLayerIndex = ETOI(GAME_INSTANCE->Get_LayerRegister()->Get_LayerByName(L"PlayerPhysical"));

	return S_OK;
}

void Em3000MeleeSight::On_Destroy() { PartObject::On_Destroy(); }
void Em3000MeleeSight::On_Enable() { PartObject::On_Enable(); }
void Em3000MeleeSight::On_Disable() { PartObject::On_Disable(); }

void Em3000MeleeSight::Update(Float timeDelta)
{
	m_Transform->Update_WorldMatrix();
	Update_CombineWorldMatrix(*m_Transform->Get_WorldMatrixPtr());
	m_Sight->Update(m_CombinedWorldMatrix);
}

void Em3000MeleeSight::Late_Update(Float timeDelta)
{
	
}

void Em3000MeleeSight::OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	auto target = targetCollider->Get_Owner();
	if (nullptr == target) return;

	if (target->Get_GameObjectType() != GAMEOBJECTTYPE::CONTAINER) return;
	uint32 targetLayer = target->Get_LayerMask().Get_Layer();
	if (m_PlayerPhysicalLayerIndex != targetLayer) return;

	m_HasTarget = true;

	Vector3 pos = m_Owner.lock()->Get_Transform()->Get_Position();
	Vector3 targetPos = target->Get_Transform()->Get_Position();
	Vector3 lookDir = m_Owner.lock()->Get_Transform()->Get_Look(); // Z축을 시야의 정면 방향으로 사용
	lookDir.y = 0.f;
	lookDir.Normalize();

	Vector3 toTarget = targetPos - pos;
	toTarget.y = 0.f;
	toTarget.Normalize();

	// 시야 각도 90도 안쪽이면 (내적 > 0) 앞으로 판별
	if (lookDir.Dot(toTarget) > 0.f)
	{
		m_IsTargetFront = true;
	}
	else
	{
		m_IsTargetFront = false;
	}
}

void Em3000MeleeSight::OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	auto target = targetCollider->Get_Owner();
	if (nullptr == target) return;

	if (target->Get_GameObjectType() != GAMEOBJECTTYPE::CONTAINER) return;
	uint32 targetLayer = target->Get_LayerMask().Get_Layer();
	if (m_PlayerPhysicalLayerIndex != targetLayer) return;

	m_HasTarget = true;

	Vector3 pos = m_Owner.lock()->Get_Transform()->Get_Position();
	Vector3 targetPos = target->Get_Transform()->Get_Position();
	Vector3 lookDir = m_Owner.lock()->Get_Transform()->Get_Look(); // Z축을 시야의 정면 방향으로 사용
	lookDir.y = 0.f;
	lookDir.Normalize();

	Vector3 toTarget = targetPos - pos;
	toTarget.y = 0.f;
	toTarget.Normalize();

	// 시야 각도 90도 안쪽이면 (내적 > 0) 앞으로 판별
	if (lookDir.Dot(toTarget) > 0.f)
	{
		m_IsTargetFront = true;
	}
	else
	{
		m_IsTargetFront = false;
	}
}

void Em3000MeleeSight::OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	auto target = targetCollider->Get_Owner();
	if (nullptr == target) return;

	uint32 targetLayer = target->Get_LayerMask().Get_Layer();
	if (m_PlayerPhysicalLayerIndex != targetLayer) return;

	m_HasTarget = false;
	m_IsTargetFront = false;
}

Shared<Em3000MeleeSight> Em3000MeleeSight::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<Em3000MeleeSight>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Em3000MeleeSight");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> Em3000MeleeSight::Clone(void* arg)
{
	auto instance = make_shared<Em3000MeleeSight>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : Em3000MeleeSight");
		return nullptr;
	}

	return instance;
}


