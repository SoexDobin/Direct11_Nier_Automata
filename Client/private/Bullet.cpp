#include "pch.h"
#include "Bullet.h"

#include <ContainerObject.h>
#include <Game.h>
#include <SphereCollider.h>

#include "SpdLogger.h"
#include "Model.h"
#include "Monster.h"
#include "Shader.h"
#include "PartObject.h"
#include "Pl0000.h"


Bullet::Bullet() : Projectile{} {}
Bullet::Bullet(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Projectile{device, context} {}
Bullet::Bullet(const Bullet& rhs)
	: Projectile{rhs} {}

HRESULT Bullet::Initialize_Prototype()
{
	m_LayerMask.Set_Layer(L"Bullet");

	return Projectile::Initialize_Prototype();
}

HRESULT Bullet::Initialize(void* arg)
{
	if (FAILED(Projectile::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize Bullet");
		return E_FAIL;
	}

	if (arg != nullptr)
		m_Desc = *static_cast<BULLET_DESC*>(arg);

	m_DamageInfo = m_Desc.damageInfo;
	m_Transform->Set_Position(m_Desc.initialPosition);
	m_Transform->LookAt(m_Desc.initialPosition + m_Desc.direction);

	if (FAILED(Ready_Components()))
	{
		LOG_ERROR(L"Failed to Ready_Components Bullet");
		return E_FAIL;
	}

	return S_OK;
}

void Bullet::Update(Float timeDelta)
{
	m_TravelDistance += m_Speed * timeDelta;

	if (m_TravelDistance >= m_Desc.maxDistance)
	{
		Object::Destroy(shared_from_this());
		return;
	}
	// 직진 이동
	
	m_Transform->Move_Forward(timeDelta, m_Speed);
	m_Collider->Update(m_Transform->Get_WorldMatrix());
	m_Transform->Update_WorldMatrix();
}

void Bullet::Late_Update(Float timeDelta)
{
	
}

HRESULT Bullet::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t numMeshes = m_Model->Get_NumMeshes();
	for (uint32 i = 0; i < numMeshes; ++i)
	{
		m_Model->Bind_Material(m_Shader, DiffuseMap, i, 1, 0);

		if (FAILED(m_Shader->Begin(0)))
			return E_FAIL;

		m_Model->Render(i);
	}

	return S_OK;
}

void Bullet::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

void Bullet::OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	auto target = targetCollider->Get_Owner();
	if (!target) return;

	if (target->Get_LayerMask().Get_Layer() == Get_LayerMask().Get_Layer())
	{
		auto otherBullet = static_pointer_cast<Bullet>(target);

		if (otherBullet->Is_Permanent() || m_Desc.isPermanent) return;
		if (m_Desc.targetLayer == otherBullet->Get_TargetLayerName()) return;
		Destroy(shared_from_this());
		return;
	}

	if (target->Get_GameObjectType() != GAMEOBJECTTYPE::PART) return;
	
	auto entity = static_pointer_cast<PartObject>(target)->Get_Owner();
	const wstring& layerTag = target->Get_LayerMask().Get_LayerName();
	if (m_Desc.targetLayer == L"Monster" &&
		m_Desc.targetLayer == layerTag)
	{

		m_DamageInfo.hitPosition = targetCollider->ClosestPoint(ownCollider->Get_Pivot());

		auto monster = static_pointer_cast<Monster>(entity);
		monster->TakeDamage(m_DamageInfo);
		Destroy(shared_from_this());
		return;
	}
	
	if (m_Desc.targetLayer == L"Player")
	{
		if (layerTag == L"PlayerWeapon"
			&& false == m_Desc.isPermanent)
		{
			if (m_Desc.isPermanent) return;
			Destroy(shared_from_this());
			return;
		}
		if (layerTag == L"Player")
		{
			m_DamageInfo.hitPosition = targetCollider->ClosestPoint(ownCollider->Get_Pivot());
			auto player = static_pointer_cast<Pl0000>(entity);
			player->TakeDamage(m_DamageInfo);
			Destroy(shared_from_this());
		}
		return;
	}
}

HRESULT Bullet::Ready_Components()
{
	Shader::SHADER_DESC shaderDesc{ VTXMESH::Tag,  VTXMESH::Elements, VTXMESH::numElements };
	m_Shader = Add_Component<Shader>(ETOI(LEVEL::STATIC), &shaderDesc);
	if (nullptr == m_Shader)
		return E_FAIL;

	Model::MODEL_DESC modelDesc{ m_Desc.resourceTag };
	m_Model = Add_Component<Model>(ETOI(LEVEL::STATIC), &modelDesc);
	if (nullptr == m_Model)
		return E_FAIL;

	SphereCollider::SPHERE_COLLIDER_DESC sphereColliderDesc{};
	sphereColliderDesc.radius = 1.f;
	sphereColliderDesc.offset = Vector3::Zero;
	m_Collider = Add_Component<SphereCollider>(ETOI(LEVEL::GAMEPLAY), &sphereColliderDesc);
	if (nullptr == m_Collider)
		return E_FAIL;

	m_Transform->Set_Scale(0.3f, 0.3f, 0.3f);

	return S_OK;
}

HRESULT Bullet::Bind_ShaderResources()
{
	if (FAILED(m_Transform->Bind_ShaderResource(m_Shader, WorldMatrix)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_TransformMatrix(m_Shader, ViewMatrix, D3DTS::VIEW)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_TransformMatrix(m_Shader, ProjMatrix, D3DTS::PROJ)))
		return E_FAIL;

	if (FAILED(GAME_INSTANCE->Bind_CameraPosition(m_Shader, CameraPosition)))
		return E_FAIL;

	return S_OK;
}

Shared<Bullet> Bullet::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<Bullet>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Bullet");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> Bullet::Clone(void* arg)
{
	auto instance = make_shared<Bullet>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : Bullet");
		return nullptr;
	}

	return instance;
}


