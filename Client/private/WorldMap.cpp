#include "pch.h"
#include "WorldMap.h"
#include "Shader.h"
#include "Model.h"
#include "Game.h"
#include "Navigation.h"
#include "WorldCollider.h"
#include <SpdLogger.h>

WorldMap::WorldMap() : GameObject{} {}
WorldMap::WorldMap(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: GameObject{ device, context } {}
WorldMap::WorldMap(const WorldMap& rhs)
	: GameObject{ rhs } {}

HRESULT WorldMap::Initialize_Prototype()
{
	return GameObject::Initialize_Prototype();
}

HRESULT WorldMap::Initialize(void* arg)
{
	if (FAILED(GameObject::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize WorldMap {}", m_ObjectName);
		return E_FAIL;
	}

	if (FAILED(Ready_Components()))
	{
		LOG_ERROR(L"Failed to Ready Components {}", m_ObjectName);
		return E_FAIL;
	}

	return S_OK;
}

void WorldMap::On_Destroy()
{
	GameObject::On_Destroy();
}

void WorldMap::On_Enable()
{
	GameObject::On_Enable();
}

void WorldMap::On_Disable()
{
	GameObject::On_Disable();
}

HRESULT WorldMap::Render()
{
#ifdef _DEBUG
	if (m_Navigation)
		m_Navigation->Render_Debug();
#endif

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t numMeshes = m_Model->Get_NumMeshes();
	for (uint32 i = 0; i < numMeshes; ++i)
	{
		if (FAILED(m_Model->BindAndBeginMaterial(m_Shader, i)))
			return E_FAIL;

		if (FAILED(m_Model->Render(i))) return E_FAIL;
	}

	return S_OK;
}

void WorldMap::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

HRESULT WorldMap::Ready_Components()
{
	Shader::SHADER_DESC shaderDesc{ VTXMESH::Tag,  VTXMESH::Elements, VTXMESH::numElements };
	m_Shader = Add_Component<Shader>(ETOI(LEVEL::STATIC), &shaderDesc);
	if (nullptr == m_Shader)
		return E_FAIL;

	// 새로 배치한 타일의 기본 모델. 저장된 ModelTag가 있으면 로드 때 그 모델로 바뀐다.
	Model::MODEL_DESC modelDesc{ L"CityOfRuinEntry" };
	m_Model = Add_Component<Model>(ETOI(LEVEL::STATIC), &modelDesc);
	if (nullptr == m_Model)
		return E_FAIL;
	if (FAILED(m_Model->Validate_MaterialBindings(m_Shader))) return E_FAIL;

	// 태그를 비워 두면 Post_Load에서 최종 ModelTag를 따라간다. 길찾기 데이터 전용이다.
	Navigation::NAVIGATION_DESC navDesc{};
	m_Navigation = Add_Component<Navigation>(ETOI(LEVEL::STATIC), &navDesc);
	if (nullptr == m_Navigation)
		LOG_ERROR(L"Failed to attach Navigation to WorldMap {}", m_ObjectName);

	// 충돌은 원작 COL(<ModelTag>_COL)이 정의한다. 시각 메시는 충돌로 쓰지 않는다.
	WorldCollider::WORLD_COLLIDER_DESC colliderDesc{};
	m_WorldCollider = Add_Component<WorldCollider>(ETOI(LEVEL::STATIC), &colliderDesc);
	if (nullptr == m_WorldCollider)
		LOG_ERROR(L"Failed to attach WorldCollider to WorldMap {}", m_ObjectName);

	return S_OK;
}

HRESULT WorldMap::Bind_ShaderResources()
{
	if (FAILED(m_Transform->Bind_ShaderResource(m_Shader, WorldMatrix)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_TransformMatrix(m_Shader, ViewMatrix, D3DTS::VIEW)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_TransformMatrix(m_Shader, ProjMatrix, D3DTS::PROJ)))
		return E_FAIL;

	return S_OK;
}

Shared<WorldMap> WorldMap::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<WorldMap>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		LOG_ERROR(L"Failed to Created : WorldMap");
		MSG_BOX("Failed to Created : WorldMap");
	}
	return prototype;
}

Shared<GameObject> WorldMap::Clone(void* arg)
{
	auto instance = make_shared<WorldMap>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Cloned : WorldMap");
		MSG_BOX("Failed to Cloned : WorldMap");
	}
	return instance;
}
