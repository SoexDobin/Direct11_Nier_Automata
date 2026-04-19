#include "pch.h"
#include "CityOfRuinBridge.h"
#include "Shader.h"
#include "Model.h"
#include "Game.h"
#include "Navigation.h"
#include <SpdLogger.h>

CityOfRuinBridge::CityOfRuinBridge() : WorldObject{} {}
CityOfRuinBridge::CityOfRuinBridge(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: WorldObject{ device, context } {}
CityOfRuinBridge::CityOfRuinBridge(const CityOfRuinBridge& rhs)
	: WorldObject{ rhs } {}

HRESULT CityOfRuinBridge::Initialize_Prototype()
{
	return GameObject::Initialize_Prototype();
}

HRESULT CityOfRuinBridge::Initialize(void* arg)
{
	// 210 185
	WorldObject::WORLD_OBJECT_DESC desc;
	desc.vertexTag = VTXWORLDMESH;
	desc.modelTag = L"CityOfRuinBridge";
	desc.navTag = L"CityOfRuinBridge";

	if (FAILED(WorldObject::Initialize(&desc)))
	{
		LOG_ERROR(L"Failed to Ready Components {}", m_ObjectName);
		return E_FAIL;
	}

	return S_OK;
}

void CityOfRuinBridge::On_Destroy()
{
	GameObject::On_Destroy();
}

void CityOfRuinBridge::On_Enable()
{
	GameObject::On_Enable();
}

void CityOfRuinBridge::On_Disable()
{
	GameObject::On_Disable();
}

HRESULT CityOfRuinBridge::Render()
{
#ifdef _DEBUG
	m_Navigation->Render_Debug();
#endif

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t numMeshes = m_Model->Get_NumMeshes();
	const Matrix& worldMat = m_Transform->Get_WorldMatrix();
	for (uint32 i = 0; i < numMeshes; ++i)
	{
		if (!m_Model->IsInFrustum_PreMesh(i, worldMat))
			continue;

		if (i <= 21)
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

void CityOfRuinBridge::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

Shared<CityOfRuinBridge> CityOfRuinBridge::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<CityOfRuinBridge>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		LOG_ERROR(L"Failed to Created : CityOfRuinBridge");
		MSG_BOX("Failed to Created : CityOfRuinBridge");
	}
	return prototype;
}

Shared<GameObject> CityOfRuinBridge::Clone(void* arg)
{
	auto instance = make_shared<CityOfRuinBridge>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Cloned : CityOfRuinBridge");
		MSG_BOX("Failed to Cloned : CityOfRuinBridge");
	}
	return instance;
}
