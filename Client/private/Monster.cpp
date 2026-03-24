#include "pch.h"
#include "Monster.h"

#include <Game.h>
#include <Transform.h>

#include "SpdLogger.h"

Monster::Monster() : GameObject{} {}
Monster::Monster(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: GameObject{device, context}
{}
Monster::Monster(const Monster& rhs)
	: GameObject{rhs}
{}

HRESULT Monster::Initialize_Prototype()
{
	return GameObject::Initialize_Prototype();
}

HRESULT Monster::Initialize(void* arg)
{
	if (FAILED(GameObject::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize GameObject {}", m_ObjectName);
		return E_FAIL;
	}

	if (FAILED(Ready_Components()))
	{
		LOG_ERROR(L"Failed to Ready Components {}", m_ObjectName);
		return E_FAIL;
	}

	return S_OK;
}

void Monster::On_Destroy()
{
	GameObject::On_Destroy();
}

void Monster::On_Enable()
{
	GameObject::On_Enable();
}

void Monster::On_Disable()
{
	GameObject::On_Disable();
}

void Monster::Priority_Update(Float timeDelta)
{

}

void Monster::Update(Float timeDelta)
{
	
}

void Monster::Late_Update(Float timeDelta)
{
}

void Monster::Fixed_Update(Float fixedDelta)
{
	m_Model->Update_ModelAnimation(fixedDelta);
}

HRESULT Monster::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t numMeshes = m_Model->Get_NumMeshes();
	for (uint32 i = 0; i < numMeshes; ++i)
	{
		m_Model->Bind_Material(m_Shader, "g_DiffuseTexture", i, 1, 0);
		m_Model->Bind_BoneMatrices(m_Shader, "g_BoneMatrices", i);

		if (FAILED(m_Shader->Begin(0)))
			return E_FAIL;
	
		m_Model->Render(i);
	}

	return S_OK;
}

void Monster::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

HRESULT Monster::Ready_Components()
{
	Shader::SHADER_DESC shaderDesc{ VTXMESH::Tag,  VTXMESH::Elements, VTXMESH::numElements };
	m_Shader = Add_Component<Shader>(ETOI(LEVEL::STATIC), &shaderDesc);
	if (nullptr == m_Shader)
		return E_FAIL;

	Model::MODEL_DESC modelDesc{ L"CityOfRuinsMainField" };
	m_Model = Add_Component<Model>(ETOI(LEVEL::STATIC), &modelDesc);
	if (nullptr == m_Model)
		return E_FAIL;

	return S_OK;
}

HRESULT Monster::Bind_ShaderResources()
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

Shared<Monster> Monster::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	Shared<Monster> defaultMonster = make_shared<Monster>(device, context);

	if (FAILED(defaultMonster->Initialize_Prototype()))
	{
		LOG_ERROR(L"Failed to Created : Monster");
		MSG_BOX("Failed to Created : Monster");
	}
	return defaultMonster;
}

Shared<GameObject> Monster::Clone(void* arg)
{
	Shared<Monster> defaultMonster = make_shared<Monster>(*this);

	if (FAILED(defaultMonster->Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Cloned : Monster");
		MSG_BOX("Failed to Cloned : CMonster");
	}
	return defaultMonster;
}
