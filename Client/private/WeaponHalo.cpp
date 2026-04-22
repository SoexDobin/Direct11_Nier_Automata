#include "pch.h"
#include "WeaponHalo.h"

#include <ContainerObject.h>
#include <Game.h>
#include <SpdLogger.h>
#include <Shader.h>
#include <Model.h>

#include "Pl0000Parts.h"

WeaponHalo::WeaponHalo(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: PartObject{device, context} {}
WeaponHalo::WeaponHalo(const WeaponHalo& rhs)
	: PartObject{rhs} {}
HRESULT WeaponHalo::Initialize_Prototype()
{
	return PartObject::Initialize_Prototype();
}

HRESULT WeaponHalo::Initialize(void* arg)
{
	if (FAILED(PartObject::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Init WeaponHalo");
		return E_FAIL;
	}

	auto& desc = *static_cast<WEAPON_HALO_DESC*>(arg);

	if (FAILED(Ready_Components()))
	{
		LOG_ERROR(L"Failed to Ready_Components Halo");
		return E_FAIL;
	}

	return S_OK;
}

void WeaponHalo::Update(Float timeDelta)
{
	if (m_Owner.expired())
	{
		Destroy(shared_from_this());
		return;
	}
}

void WeaponHalo::Late_Update(Float timeDelta)
{
	m_Transform->Update_WorldMatrix();
	Update_CombineWorldMatrix(*m_Transform->Get_WorldMatrixPtr());
}

HRESULT WeaponHalo::Render()
{
	if (m_IsActive)
	{
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		size_t numMeshes = m_Model->Get_NumMeshes();
		for (uint32 i = 0; i < numMeshes; ++i)
		{
			m_Model->Bind_Material(m_Shader, DiffuseMap, i, 0);
			m_Model->Bind_Material(m_Shader, DiffuseMap1, i, 0);
			m_Model->Bind_Material(m_Shader, DiffuseMap2, i, 0);

			if (FAILED(m_Shader->Begin(4)))
				return E_FAIL;
			m_Model->Render(i);
		}
	}
	return S_OK;
}

void WeaponHalo::Submit_RenderGroup()
{
	if (m_IsActive)
	{
		GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONLIGHT, shared_from_this());
	}
}

void WeaponHalo::OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider)
{
	GameObject::OnCollisionEnter(ownCollider, targetCollider);
}

HRESULT WeaponHalo::Ready_Components()
{
	Shader::SHADER_DESC shaderDesc{ VTXMESH::Tag,  VTXMESH::Elements, VTXMESH::numElements };
	m_Shader = Add_Component<Shader>(ETOI(LEVEL::STATIC), &shaderDesc);
	if (nullptr == m_Shader)
		return E_FAIL;

	Model::MODEL_DESC modelDesc{ L"halo" };
	m_Model = Add_Component<Model>(ETOI(LEVEL::STATIC), &modelDesc);
	if (nullptr == m_Model)
		return E_FAIL;

	return S_OK;
}

HRESULT WeaponHalo::Bind_ShaderResources()
{
	if (FAILED(m_Shader->Bind_Matrix(WorldMatrix, &m_CombinedWorldMatrix)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_TransformMatrix(m_Shader, ViewMatrix, D3DTS::VIEW)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_TransformMatrix(m_Shader, ProjMatrix, D3DTS::PROJ)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_CameraPosition(m_Shader, CameraPosition)))
		return E_FAIL;

	return S_OK;
}
Shared<WeaponHalo> WeaponHalo::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<WeaponHalo>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : WeaponHalo");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> WeaponHalo::Clone(void* arg)
{
	auto instance = make_shared<WeaponHalo>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : WeaponHalo");
		return nullptr;
	}

	return instance;
}


