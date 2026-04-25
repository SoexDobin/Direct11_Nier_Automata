#include "pch.h"
#include "CityOfRuinBuilding.h"
#include "Shader.h"
#include "Model.h"
#include "Game.h"
#include "Navigation.h"
#include <SpdLogger.h>



CityOfRuinBuilding::CityOfRuinBuilding(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: WorldObject{ device, context } {
}

CityOfRuinBuilding::CityOfRuinBuilding(const CityOfRuinBuilding& rhs)
	: WorldObject{ rhs } {
}

HRESULT CityOfRuinBuilding::Initialize_Prototype()
{
	return WorldObject::Initialize_Prototype();
}

HRESULT CityOfRuinBuilding::Initialize(void* arg)
{
	// 290 0 -41.5
	// 0 - 90 0
	WorldObject::WORLD_OBJECT_DESC desc;
	desc.vertexTag = VTXWORLDMESH;
	desc.modelTag = L"CityOfRuinBuilding";
	desc.navTag = L"CityOfRuinBuilding";

	if (FAILED(WorldObject::Initialize(&desc)))
	{
		LOG_ERROR(L"Failed to Ready Components {}", m_ObjectName);
		return E_FAIL;
	}

	return S_OK;
}

void CityOfRuinBuilding::On_Destroy()
{
	WorldObject::On_Destroy();
}

void CityOfRuinBuilding::On_Enable()
{
	WorldObject::On_Enable();
}

void CityOfRuinBuilding::On_Disable()
{
	WorldObject::On_Disable();
}

HRESULT CityOfRuinBuilding::Render()
{
#ifdef _DEBUG
	m_Navigation->Render_Debug();
#endif

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t numMeshes = m_Model->Get_NumMeshes();
	Matrix worldMat = m_Transform->Get_WorldMatrix();
	for (uint32 i = 0; i < numMeshes; ++i)
	{
		if (!m_Model->IsInFrustum_PreMesh(i, worldMat))
			continue;

		if (!m_Model->IsInFrustum_PreMesh(i, worldMat))
			continue;

		if (i <= 7)
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

HRESULT CityOfRuinBuilding::Render_Shadow()
{
	if (FAILED(m_Shader->Bind_Matrix(WorldMatrix, m_Transform->Get_WorldMatrixPtr())))
		return E_FAIL;

	size_t numMeshes = m_Model->Get_NumMeshes();
	for (uint32 i = 0; i < numMeshes; ++i)
	{
		if (!m_Model->IsInFrustum_PreMesh(i, m_Transform->Get_WorldMatrix()))
			continue;

		if (FAILED(m_Shader->Begin(2)))
			return E_FAIL;

		m_Model->Render(i);
	}

	return S_OK;
}

void CityOfRuinBuilding::Submit_RenderGroup()
{
	WorldObject::Submit_RenderGroup();
}

Shared<CityOfRuinBuilding> CityOfRuinBuilding::Create(const ComPtr<ID3D11Device>& device,
                                                      const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<CityOfRuinBuilding>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CityOfRuinBuilding");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> CityOfRuinBuilding::Clone(void* arg)
{
	auto instance = make_shared<CityOfRuinBuilding>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : CityOfRuinBuilding");
		return nullptr;
	}

	return instance;
}


