#include "pch.h"
#include "WorldObject.h"

#include <Game.h>
#include <Shader.h>
#include <Model.h>
#include <Navigation.h>
#include <SpdLogger.h>

WorldObject::WorldObject(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: GameObject{ device, context } { }
WorldObject::WorldObject(const WorldObject& rhs)
	: GameObject{ rhs } { }

HRESULT WorldObject::Initialize_Prototype()
{
	return GameObject::Initialize_Prototype();
}

HRESULT WorldObject::Initialize(void* arg)
{
	if (FAILED(GameObject::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize WorldObject");
		return E_FAIL;
	}

	if (FAILED(Ready_Components(*static_cast<WORLD_OBJECT_DESC*>(arg))))
	{
		LOG_ERROR(L"Failed to Initialize WorldObject");
		return E_FAIL;
	}

	return S_OK;
}

void WorldObject::On_Destroy()
{
	GameObject::On_Destroy();
}

void WorldObject::On_Enable()
{
	GameObject::On_Enable();
}

void WorldObject::On_Disable()
{
	GameObject::On_Disable();
}

HRESULT WorldObject::Render()
{
#ifdef _DEBUG
	m_Navigation->Render_Debug();
#endif

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t numMeshes = m_Model->Get_NumMeshes();
	for (uint32 i = 0; i < numMeshes; ++i)
	{
		if (FAILED(m_Model->Bind_Material(m_Shader, DiffuseMap, i, 1, 0)))
		{
			LOG_ERROR(L"Failed to Bind srv : {}", m_ObjectName);
			return E_FAIL;
		}
			

		if (FAILED(m_Shader->Begin(0)))
			return E_FAIL;

		m_Model->Render(i);
	}

	return S_OK;
}

void WorldObject::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

HRESULT WorldObject::Ready_Components(const WORLD_OBJECT_DESC& desc)
{
	Shader::SHADER_DESC shaderDesc{ desc.vertexTag,  VTXMESH::Elements, VTXMESH::numElements };
	m_Shader = Add_Component<Shader>(ETOI(LEVEL::STATIC), &shaderDesc);
	if (nullptr == m_Shader)
		return E_FAIL;

	Model::MODEL_DESC modelDesc{ desc.modelTag };
	m_Model = Add_Component<Model>(ETOI(LEVEL::STATIC), &modelDesc);
	if (nullptr == m_Model)
		return E_FAIL;

	{
		Navigation::NAVIGATION_DESC navDesc{};
		m_Navigation = Add_Component_Tag<Navigation>(ETOI(LEVEL::STATIC), desc.navTag, &navDesc);
		if (m_Navigation == nullptr) {
			LOG_ERROR("Failed to hook pre-baked Navigation Mesh!");
		}
	}

	return S_OK;
}

HRESULT WorldObject::Bind_ShaderResources() const
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