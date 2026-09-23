#include "pch.h"
#include "WorldMap.h"
#include "Shader.h"
#include "Model.h"
#include "Game.h"
#include "Navigation.h"
#include "WorldCollider.h"
#include "Camera.h"
#include <SpdLogger.h>

namespace
{
	/* 임시 거리 규칙이다. 원작 DistRate(0.70/0.35/0.10/0.03)를 거리로 바꾸는 공식이 아직
	   풀리지 않았으므로, 타일마다 크기가 크게 다른 점만 반영해 AABB 반경에 비례한 배수를 쓴다.
	   들어갈 임계값을 나올 임계값보다 크게 둬서 경계에 서 있어도 매 프레임 뒤집히지 않는다. */
	constexpr Float LOD_ENTER_RADIUS_SCALE{ 2.2f };
	constexpr Float LOD_EXIT_RADIUS_SCALE{ 1.8f };
}

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

	const Shared<Model>& model = Select_Model();
	const uint32 numMeshes = static_cast<uint32>(model->Get_NumMeshes());
	const Bool culling = GAME_INSTANCE->Get_FrustumCulling();
	const Matrix worldMatrix = m_Transform->Get_WorldMatrix();

	// 타일이 통째로 화면 밖이면 메시를 하나도 보지 않는다.
	if (culling)
	{
		BoundingBox tileLocal{};
		BoundingBox tileWorld{};
		if (model->Compute_LocalBounds(tileLocal))
		{
			tileLocal.Transform(tileWorld, worldMatrix);
			if (!GAME_INSTANCE->Is_Visible(tileWorld))
			{
				for (uint32 i = 0; i < numMeshes; ++i)
					GAME_INSTANCE->Add_CulledMesh();
				return S_OK;
			}
		}
	}

	for (uint32 i = 0; i < numMeshes; ++i)
	{
		if (culling)
		{
			BoundingBox meshLocal{};
			BoundingBox meshWorld{};
			if (model->Get_MeshLocalBounds(i, meshLocal))
			{
				meshLocal.Transform(meshWorld, worldMatrix);
				if (!GAME_INSTANCE->Is_Visible(meshWorld))
				{
					GAME_INSTANCE->Add_CulledMesh();
					continue;
				}
			}
		}

		if (FAILED(model->BindAndBeginMaterial(m_Shader, i)))
			return E_FAIL;

		if (FAILED(model->Render(i))) return E_FAIL;
	}

	return S_OK;
}

const Shared<Model>& WorldMap::Select_Model()
{
	/* Editor는 정지 상태에서 Update를 돌리지 않으면서 렌더는 계속하므로 판정이 Update가 아닌
	   렌더 경로에 있다. SkyBox::Follow_Camera가 같은 이유로 여기에 있다. Render는 View마다
	   불리지만 입력이 메인 카메라 하나라 두 View가 같은 LOD를 보고 히스테리시스도 흔들리지 않는다. */
	if (!m_LodResolved)
	{
		m_LodResolved = true;

		if (m_Model && !m_Model->Get_ModelTag().empty())
		{
			// 충돌이 <ModelTag>_COL을 따라가는 것과 같은 규칙이다.
			const wstring lodTag = m_Model->Get_ModelTag() + L"_LOD";
			const int32 levIndex = GAME_INSTANCE->Get_ContainLevelByModelTag(lodTag);
			if (levIndex != -1)
			{
				/* 정적 월드 모델은 인스턴스 상태가 없고 메시·머티리얼은 어차피 프로토타입과
				   공유되므로 클론하지 않고 프로토타입을 그대로 그린다. */
				m_LodModel = GAME_INSTANCE->Get_Model(static_cast<uint32>(levIndex), lodTag.c_str());
				if (m_LodModel && FAILED(m_LodModel->Validate_MaterialBindings(m_Shader)))
				{
					LOG_WARN(L"[WorldMap] LOD model {} failed material validation; LOD disabled", lodTag);
					m_LodModel = nullptr;
				}
			}
		}
	}

	if (nullptr == m_LodModel || !GAME_INSTANCE->Get_WorldLod())
	{
		m_UseLod = false;
		return m_Model;
	}

	const auto camera = GAME_INSTANCE->Get_MainCamera();
	BoundingBox tileLocal{};
	if (camera && camera->Get_Transform() && m_Model->Compute_LocalBounds(tileLocal))
	{
		BoundingBox tileWorld{};
		tileLocal.Transform(tileWorld, m_Transform->Get_WorldMatrix());

		const Vector3 center{ tileWorld.Center };
		const Vector3 extents{ tileWorld.Extents };
		const Float radius = extents.Length();
		const Float distance = Vector3::Distance(center, camera->Get_Transform()->Get_Position());

		m_UseLod = m_UseLod ?
			distance > radius * LOD_EXIT_RADIUS_SCALE :
			distance > radius * LOD_ENTER_RADIUS_SCALE;
	}

	if (m_UseLod)
	{
		GAME_INSTANCE->Add_LodTile();
		return m_LodModel;
	}

	return m_Model;
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
