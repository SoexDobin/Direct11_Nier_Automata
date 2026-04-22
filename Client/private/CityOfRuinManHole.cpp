#include "pch.h"
#include "CityOfRuinManHole.h"
#include "Shader.h"
#include "Model.h"
#include "Game.h"
#include "Navigation.h"
#include <SpdLogger.h>

CityOfRuinManHole::CityOfRuinManHole(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: WorldObject{ device, context } {
}

CityOfRuinManHole::CityOfRuinManHole(const CityOfRuinManHole& rhs)
	: WorldObject{ rhs } {
}

HRESULT CityOfRuinManHole::Initialize_Prototype()
{
	return WorldObject::Initialize_Prototype();
}

HRESULT CityOfRuinManHole::Initialize(void* arg)
{
	// 510 0 183
	// 0 -180 0

	WorldObject::WORLD_OBJECT_DESC desc;
	desc.vertexTag = VTXWORLDMESH;
	desc.modelTag = L"CityOfRuinManHole";
	desc.navTag = L"CityOfRuinManHole";

	if (FAILED(WorldObject::Initialize(&desc)))
	{
		LOG_ERROR(L"Failed to Ready Components {}", m_ObjectName);
		return E_FAIL;
	}

	return S_OK;
}

void CityOfRuinManHole::On_Destroy()
{
	WorldObject::On_Destroy();
}

void CityOfRuinManHole::On_Enable()
{
	WorldObject::On_Enable();
}

void CityOfRuinManHole::On_Disable()
{
	WorldObject::On_Disable();
}

HRESULT CityOfRuinManHole::Render()
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

		if (i >= 92 && i <= 106)
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

void CityOfRuinManHole::Submit_RenderGroup()
{
	WorldObject::Submit_RenderGroup();
}

Shared<CityOfRuinManHole> CityOfRuinManHole::Create(const ComPtr<ID3D11Device>& device,
	const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<CityOfRuinManHole>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CityOfRuinManHole");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> CityOfRuinManHole::Clone(void* arg)
{
	auto instance = make_shared<CityOfRuinManHole>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : CityOfRuinManHole");
		return nullptr;
	}

	return instance;
}