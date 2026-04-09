#include "pch.h"
#include "MonsterSight.h"
#include "SphereCollider.h"
#include "SpdLogger.h"

MonsterSight::MonsterSight() : PartObject{} {}
MonsterSight::MonsterSight(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: PartObject{device, context} {}
MonsterSight::MonsterSight(const MonsterSight& rhs)
	: PartObject{rhs} {}

HRESULT MonsterSight::Initialize_Prototype()
{
	return PartObject::Initialize_Prototype();
}

HRESULT MonsterSight::Initialize(void* arg)
{
	if (nullptr == arg)
	{
		LOG_ERROR(L"Failed to Initialize MonsterSight");
		return E_FAIL;
	}

	if (FAILED(PartObject::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize MonsterSight");
		return E_FAIL;
	}

	auto& sightDesc = *static_cast<MONSTER_SIGHT_DESC*>(arg);

	SphereCollider::SPHERE_COLLIDER_DESC sphereColDesc{};
	sphereColDesc.radius = sightDesc.radius;
	sphereColDesc.offset = sightDesc.offset;
	m_Sight = Add_Component<SphereCollider>(ETOI(LEVEL::GAMEPLAY), &sphereColDesc);

	if (nullptr == m_Sight)
	{
		LOG_ERROR(L"Failed to Init Monster Sight Collider Component");
		return E_FAIL;
	}

	return S_OK;
}

void MonsterSight::Update(Float timeDelta)
{
	m_Transform->Update_WorldMatrix();
	Update_CombineWorldMatrix(*m_Transform->Get_WorldMatrixPtr());
	m_Sight->Update(m_CombinedWorldMatrix);
}

void MonsterSight::Late_Update(Float timeDelta)
{
	
}

void MonsterSight::OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	
}

void MonsterSight::OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	
}

void MonsterSight::OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	
}

Shared<MonsterSight> MonsterSight::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<MonsterSight>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : MonsterSight");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> MonsterSight::Clone(void* arg)
{
	auto instance = make_shared<MonsterSight>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : MonsterSight");
		return nullptr;
	}

	return instance;
}


