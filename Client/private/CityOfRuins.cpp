#include "pch.h"
#include "CityOfRuins.h"
#include "Shader.h"
#include "Model.h"
#include "Game.h"
#include <SpdLogger.h>

CityOfRuins::CityOfRuins() : GameObject{} {}
CityOfRuins::CityOfRuins(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: GameObject{ device, context } {}
CityOfRuins::CityOfRuins(const CityOfRuins& rhs)
	: GameObject{ rhs } {}

HRESULT CityOfRuins::Initialize_Prototype()
{
	return GameObject::Initialize_Prototype();
}

HRESULT CityOfRuins::Initialize(void* arg)
{
	if (FAILED(GameObject::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize CityOfRuins {}", m_ObjectName);
		return E_FAIL;
	}

	if (FAILED(Ready_Components()))
	{
		LOG_ERROR(L"Failed to Ready Components {}", m_ObjectName);
		return E_FAIL;
	}

	return S_OK;
}

void CityOfRuins::On_Destroy()
{
	GameObject::On_Destroy();
}

void CityOfRuins::On_Enable()
{
	GameObject::On_Enable();
}

void CityOfRuins::On_Disable()
{
	GameObject::On_Disable();
}

void CityOfRuins::Priority_Update(Float timeDelta)
{

}

HRESULT CityOfRuins::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t numMeshes = m_Model->Get_NumMeshes();
	for (uint32 i = 0; i < numMeshes; ++i)
	{
		m_Model->Bind_Material(m_Shader, "g_DiffuseTexture", i, 1, 0);

		if (FAILED(m_Shader->Begin(0)))
			return E_FAIL;

		m_Model->Render(i);
	}

	return S_OK;
}

void CityOfRuins::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

HRESULT CityOfRuins::Ready_Components()
{
	Shader::SHADER_DESC shaderDesc{ VTXMESH::Tag,  VTXMESH::Elements, VTXMESH::numElements };
	m_Shader = Add_Component<Shader>(&shaderDesc);
	if (nullptr == m_Shader)
		return E_FAIL;

	Model::MODEL_DESC modelDesc{ L"CityOfRuinEntry" };
	m_Model = Add_Component<Model>(&modelDesc);
	if (nullptr == m_Model)
		return E_FAIL;

	return S_OK;
}

HRESULT CityOfRuins::Bind_ShaderResources()
{
	if (FAILED(m_Transform->Bind_ShaderResource(m_Shader, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_TransformMatrix(m_Shader, "g_ViewMatrix", D3DTS::VIEW)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_TransformMatrix(m_Shader, "g_ProjMatrix", D3DTS::PROJ)))
		return E_FAIL;

	if (FAILED(GAME_INSTANCE->Bind_CameraPosition(m_Shader, "g_CameraPosition")))
		return E_FAIL;

	const LIGHT_DESC* lightDesc = GAME_INSTANCE->Get_LightDesc(0);
	if (nullptr == lightDesc)
		return E_FAIL;

	if (FAILED(m_Shader->Bind_RawValue("g_LightDir", &lightDesc->direction, sizeof(Float4))))
		return E_FAIL;
	if (FAILED(m_Shader->Bind_RawValue("g_LightDiffuse", &lightDesc->diffuse, sizeof(Float4))))
		return E_FAIL;
	if (FAILED(m_Shader->Bind_RawValue("g_LightAmbient", &lightDesc->ambient, sizeof(Float4))))
		return E_FAIL;
	if (FAILED(m_Shader->Bind_RawValue("g_LightSpecular", &lightDesc->specular, sizeof(Float4))))
		return E_FAIL;

	return S_OK;
}

Shared<CityOfRuins> CityOfRuins::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto defaultMonster = make_shared<CityOfRuins>(device, context);

	if (FAILED(defaultMonster->Initialize_Prototype()))
	{
		LOG_ERROR(L"Failed to Created : CityOfRuins");
		MSG_BOX("Failed to Created : CityOfRuins");
	}
	return defaultMonster;
}

Shared<GameObject> CityOfRuins::Clone(void* arg)
{
	auto defaultMonster = make_shared<CityOfRuins>(*this);

	if (FAILED(defaultMonster->Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Cloned : CityOfRuins");
		MSG_BOX("Failed to Cloned : CityOfRuins");
	}
	return defaultMonster;
}
