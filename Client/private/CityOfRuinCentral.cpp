#include "pch.h"
#include "CityOfRuinCentral.h"

#include <Game.h>
#include <Shader.h>
#include <Model.h>
#include <Navigation.h>
#include <SpdLogger.h>

CityOfRuinCentral::CityOfRuinCentral(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: WorldObject{ device, context } { }

CityOfRuinCentral::CityOfRuinCentral(const CityOfRuinCentral& rhs)
	: WorldObject{ rhs } { }

HRESULT CityOfRuinCentral::Initialize_Prototype()
{
	return WorldObject::Initialize_Prototype();
}

HRESULT CityOfRuinCentral::Initialize(void* arg)
{
	// 360 98.5
	WorldObject::WORLD_OBJECT_DESC desc;
	desc.vertexTag = VTXWORLDMESH;
	desc.modelTag = L"CityOfRuinCentral";
	desc.navTag = L"CityOfRuinCentral";

	if (FAILED(WorldObject::Initialize(&desc)))
	{
		LOG_ERROR(L"Failed to Ready Components {}", m_ObjectName);
		return E_FAIL;
	}

	return S_OK;
}

void CityOfRuinCentral::On_Destroy()
{
	WorldObject::On_Destroy();
}

void CityOfRuinCentral::On_Enable()
{
	WorldObject::On_Enable();
}

void CityOfRuinCentral::On_Disable()
{
	WorldObject::On_Disable();
}

HRESULT CityOfRuinCentral::Render()
{	
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t numMeshes = m_Model->Get_NumMeshes();
	Matrix worldMat = m_Transform->Get_WorldMatrix();
	for (uint32 i = 0; i < numMeshes; ++i)
	{
		if (!m_Model->IsInFrustum_PreMesh(i, worldMat))
			continue;

		if (i <= 22)
		{
			auto w = Vector4{ 0.1f, 0.45f, 0.45f, 10.f };
			m_Shader->Bind_RawValue(BlendWeight, &w, sizeof(Vector4));
		}
		else
		{
			auto w = Vector4{ 0.33f, 0.33f, 0.34f, 1.f };
			m_Shader->Bind_RawValue(BlendWeight, &w, sizeof(Vector4));
		}


		if (FAILED(m_Model->Bind_Material(m_Shader, DiffuseMap, i, 1, 0)))
			continue;

		HRESULT h1 = m_Model->Bind_Material(m_Shader, DiffuseMap1, i, 1, 1);
		HRESULT h2 = m_Model->Bind_Material(m_Shader, DiffuseMap2, i, 1, 2);
		Bool isBlend = SUCCEEDED(h1) && SUCCEEDED(h2);
		uint32 passIndex = isBlend ? 0 : 1;

		m_Model->Bind_Material(m_Shader, NormalMap, i, 6, 0);
		if (isBlend)
		{
			m_Model->Bind_Material(m_Shader, NormalMap1, i, 6, 1);
			m_Model->Bind_Material(m_Shader, NormalMap2, i, 6, 2);
		}


		if (FAILED(m_Shader->Begin(passIndex)))
			return E_FAIL;

		m_Model->Render(i);
	}


	return S_OK;
}

HRESULT CityOfRuinCentral::Render_Shadow()
{
	if (FAILED(m_Transform->Bind_ShaderResource(m_Shader, WorldMatrix)))
		return E_FAIL;

	if (FAILED(GAME_INSTANCE->Bind_Shadow_TransformMatrix(m_Shader, ViewMatrix, D3DTS::VIEW)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_Shadow_TransformMatrix(m_Shader, ProjMatrix, D3DTS::PROJ)))
		return E_FAIL;


	size_t numMeshes = m_Model->Get_NumMeshes();
	for (uint32 i = 0; i < numMeshes; ++i)
	{

		m_Model->Bind_BoneMatrices(m_Shader, BoneMatrices, i);

		if (FAILED(m_Shader->Begin(2)))
			return E_FAIL;

		m_Model->Render(i);
	}
	return S_OK;
}


void CityOfRuinCentral::Submit_RenderGroup()
{
	WorldObject::Submit_RenderGroup();
}

Shared<CityOfRuinCentral> CityOfRuinCentral::Create(const ComPtr<ID3D11Device>& device,
                                                    const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<CityOfRuinCentral>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CityOfRuinCentral");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> CityOfRuinCentral::Clone(void* arg)
{
	auto instance = make_shared<CityOfRuinCentral>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : CityOfRuinCentral");
		return nullptr;
	}

	return instance;
}


