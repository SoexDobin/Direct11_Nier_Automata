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
	/* 원작 ObjectParam의 DistRate를 그대로 쓴다(g11120·g11121·g11220 모두 0.70/0.35/0.10/0.03).
	   미터로 환산할 공식이 아직 없으므로 겉보기 크기 비율 s = 타일 AABB 반경 / 가장 가까운
	   면까지의 거리로 읽는다. 값이 내림차순인 것과 자연스럽게 맞는다. DistRate3(0.03)은
	   LOD3이 아니라 가시성 한계이고, 우리 타일 반경에서는 far plane 밖이라 쓰지 않는다. */
	constexpr Float LOD_RATE_LEAVE_LOD0{ 0.70f };
	constexpr Float LOD_RATE_LEAVE_LOD1{ 0.35f };
	// 잠정 배율. 원작 전환 공식이 밝혀지면 이 상수만 걷어내면 된다.
	constexpr Float LOD_RATE_SCALE{ 1.0f };
	// 경계에서 매 프레임 뒤집히지 않게 하는 폭.
	constexpr Float LOD_HYSTERESIS{ 0.10f };

	/// 이력 없이 겉보기 크기만으로 고르는 기본 판정.
	uint32 Band_From_Size(Float size)
	{
		if (size >= LOD_RATE_LEAVE_LOD0 * LOD_RATE_SCALE) return 0;
		if (size >= LOD_RATE_LEAVE_LOD1 * LOD_RATE_SCALE) return 1;
		return 2;
	}

	/// "LOD1_<이름>-LOD1.003"과 "g11021_build1-LOD1.002" 두 표기를 모두 받는다.
	uint32 Parse_LodLevel(const string& name)
	{
		const size_t suffix = name.rfind("-LOD");
		if (suffix != string::npos && suffix + 4 < name.size())
		{
			const Char digit = name[suffix + 4];
			if (digit >= '0' && digit <= '9')
				return static_cast<uint32>(digit - '0');
		}
		if (name.size() > 4 && name.compare(0, 3, "LOD") == 0 && name[4] == '_' &&
			name[3] >= '0' && name[3] <= '9')
			return static_cast<uint32>(name[3] - '0');
		return 0;
	}

	/// 레벨 표기와 Blender 사본 번호를 떼어 같은 물체끼리 묶을 이름을 만든다.
	string Strip_LodDecoration(const string& name)
	{
		string out = name;
		if (out.size() > 4 && out.compare(0, 3, "LOD") == 0 && out[4] == '_' &&
			out[3] >= '0' && out[3] <= '9')
			out.erase(0, 5);

		const size_t dash = out.rfind("-LOD");
		if (dash != string::npos)
		{
			size_t end = dash + 4;
			while (end < out.size() && out[end] >= '0' && out[end] <= '9')
				++end;
			out.erase(dash, end - dash);
		}

		const size_t dot = out.rfind('.');
		if (dot != string::npos && dot + 1 < out.size())
		{
			Bool allDigits = true;
			for (size_t i = dot + 1; i < out.size(); ++i)
				if (out[i] < '0' || out[i] > '9') { allDigits = false; break; }
			if (allDigits) out.erase(dot);
		}
		return out;
	}
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
	/* 밴드0은 원본 모델 전체다. 밴드1·2는 _LOD.model에서 그 레벨에 속한 메시와, LOD 판이 없어
	   원본으로 남는 메시(m_LodKeepOriginal)를 함께 그린다.
	   참조 대신 포인터를 쓰는 이유는 nullptr로 "전체"를 표현하면서 벡터 복사를 피하기 위해서다. */
	const vector<uint32>* meshOrder = nullptr;
	if (m_LodBand == 1) meshOrder = &m_LodBand1;
	else if (m_LodBand == 2) meshOrder = &m_LodBand2;
	const vector<uint32>* keepOrder = meshOrder ? &m_LodKeepOriginal : nullptr;

	const uint32 numMeshes = (meshOrder ?
		static_cast<uint32>(meshOrder->size()) : static_cast<uint32>(model->Get_NumMeshes())) +
		(keepOrder ? static_cast<uint32>(keepOrder->size()) : 0u);
	const Bool culling = GAME_INSTANCE->Get_FrustumCulling();
	const Matrix worldMatrix = m_Transform->Get_WorldMatrix();

	// 타일이 통째로 화면 밖이면 메시를 하나도 보지 않는다. 밴드1·2는 원본 메시도 섞이므로 원본 상자로 본다.
	if (culling)
	{
		BoundingBox tileLocal{};
		BoundingBox tileWorld{};
		if (m_Model->Compute_LocalBounds(tileLocal))
		{
			BoundingBox lodLocal{};
			if (meshOrder && model->Compute_LocalBounds(lodLocal))
				BoundingBox::CreateMerged(tileLocal, tileLocal, lodLocal);
			tileLocal.Transform(tileWorld, worldMatrix);
			if (!GAME_INSTANCE->Is_Visible(tileWorld))
			{
				for (uint32 i = 0; i < numMeshes; ++i)
					GAME_INSTANCE->Add_CulledMesh();
				return S_OK;
			}
		}
	}

	const auto renderMeshes = [&](const Shared<Model>& target, const vector<uint32>* order) -> HRESULT
	{
		const uint32 count = order ?
			static_cast<uint32>(order->size()) : static_cast<uint32>(target->Get_NumMeshes());
		for (uint32 k = 0; k < count; ++k)
		{
			const uint32 i = order ? (*order)[k] : k;

			if (culling)
			{
				BoundingBox meshLocal{};
				BoundingBox meshWorld{};
				if (target->Get_MeshLocalBounds(i, meshLocal))
				{
					meshLocal.Transform(meshWorld, worldMatrix);
					if (!GAME_INSTANCE->Is_Visible(meshWorld))
					{
						GAME_INSTANCE->Add_CulledMesh();
						continue;
					}
				}
			}

			if (FAILED(target->BindAndBeginMaterial(m_Shader, i)))
				return E_FAIL;

			if (FAILED(target->Render(i))) return E_FAIL;
		}
		return S_OK;
	};

	if (keepOrder && FAILED(renderMeshes(m_Model, keepOrder)))
		return E_FAIL;

	return renderMeshes(model, meshOrder);
}

void WorldMap::Ready_LodBands()
{
	m_LodBand1.clear();
	m_LodBand2.clear();
	m_LodKeepOriginal.clear();
	if (nullptr == m_LodModel)
		return;

	const uint32 numMeshes = m_LodModel->Get_NumMeshes();
	vector<uint32> levels(numMeshes, 0);
	vector<string> groups(numMeshes);
	std::set<string> hasLod2;

	for (uint32 i = 0; i < numMeshes; ++i)
	{
		string meshName{};
		uint32 materialIndex{};
		if (FAILED(m_LodModel->Get_MeshMaterialInfo(i, meshName, materialIndex)))
			continue;

		levels[i] = Parse_LodLevel(meshName);
		groups[i] = Strip_LodDecoration(meshName);
		if (levels[i] == 2)
			hasLod2.insert(groups[i]);
	}

	for (uint32 i = 0; i < numMeshes; ++i)
	{
		if (levels[i] == 2)
		{
			m_LodBand2.push_back(i);
		}
		else if (levels[i] == 1)
		{
			m_LodBand1.push_back(i);
			// 더 거친 판이 없는 물체는 먼 밴드에서도 LOD1을 그대로 쓴다.
			if (!hasLod2.contains(groups[i]))
				m_LodBand2.push_back(i);
		}
		else
		{
			// 레벨 표기가 없는 메시는 어느 밴드에서도 빠지지 않게 둘 다에 넣는다.
			m_LodBand1.push_back(i);
			m_LodBand2.push_back(i);
		}
	}

	/* LOD 판이 있는 그룹만 교체 대상이다. 지면(g11319_ground, GROUND_11220 등)처럼 _LOD.model에
	   같은 그룹이 없는 원본 메시까지 빼면 먼 타일에서 땅이 사라지고 건물만 떠 보인다. */
	const std::set<string> lodGroups(groups.begin(), groups.end());
	const uint32 numOriginal = m_Model->Get_NumMeshes();
	for (uint32 i = 0; i < numOriginal; ++i)
	{
		string meshName{};
		uint32 materialIndex{};
		if (FAILED(m_Model->Get_MeshMaterialInfo(i, meshName, materialIndex)))
			continue;
		if (!lodGroups.contains(Strip_LodDecoration(meshName)))
			m_LodKeepOriginal.push_back(i);
	}
}

const Shared<Model>& WorldMap::Select_Model()
{
	/* Editor는 정지 상태에서 Update를 돌리지 않으면서 렌더는 계속하므로 판정이 Update가 아닌
	   렌더 경로에 있다. SkyBox::Follow_Camera가 같은 이유로 여기에 있다.
	   Render는 View마다 불리고, Editor는 View마다 메인 카메라를 바꿔 끼운다
	   (EditorManager.cpp:969, :982). 따라서 밴드는 그 View의 카메라로 판정해야 하고,
	   히스테리시스는 같은 카메라로 이어질 때만 적용한다. */
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

		Ready_LodBands();
	}

	if (nullptr == m_LodModel || m_LodBand1.empty() || !GAME_INSTANCE->Get_WorldLod())
	{
		m_LodBand = 0;
		return m_Model;
	}

	const auto camera = GAME_INSTANCE->Get_MainCamera();
	/* 원본 모델 상자는 타일 밖 물체까지 품는다. G11319의 g11420_dummybuild는 214 떨어진
	   이웃 타일 위에, G11121의 enkei 판은 수천 밖에 있어 카메라가 이웃 타일로 가도 상자 안에
	   남아 LOD0에 머문다. 바꿔 그릴 대상인 LOD 모델의 상자로 잰다. */
	BoundingBox tileLocal{};
	if (camera && camera->Get_Transform() && m_LodModel->Compute_LocalBounds(tileLocal))
	{
		BoundingBox tileWorld{};
		tileLocal.Transform(tileWorld, m_Transform->Get_WorldMatrix());

		const Vector3 center{ tileWorld.Center };
		const Vector3 extents{ tileWorld.Extents };
		const Float radius = extents.Length();

		/* 중심까지가 아니라 AABB에서 가장 가까운 점까지를 잰다. 타일이 200~600으로 커서
		   중심 거리로 재면 바로 옆에 서 있어도 멀다고 판정된다. 상자 안에 있으면 0이 되어
		   항상 LOD0이고, 멀어지면 중심 거리에서 반경을 뺀 값에 수렴한다. */
		const Vector3 cameraPosition = camera->Get_Transform()->Get_Position();
		const Vector3 boxMin = center - extents;
		const Vector3 boxMax = center + extents;
		const Vector3 nearestPoint{
			std::clamp(cameraPosition.x, boxMin.x, boxMax.x),
			std::clamp(cameraPosition.y, boxMin.y, boxMax.y),
			std::clamp(cameraPosition.z, boxMin.z, boxMax.z) };
		const Float distance = Vector3::Distance(nearestPoint, cameraPosition);

		// 겉보기 크기. 가까울수록 커진다.
		const Float size = distance > 0.f ? radius / distance : FLT_MAX;
		const Float leave0 = LOD_RATE_LEAVE_LOD0 * LOD_RATE_SCALE;
		const Float leave1 = LOD_RATE_LEAVE_LOD1 * LOD_RATE_SCALE;

		if (camera.get() != m_LodBandCamera)
		{
			/* 다른 View다. 이전 밴드를 이어받으면 한 번에 한 단계씩만 움직이는 탓에
			   Game View가 밀어놓은 밴드에서 못 빠져나온다. 이 카메라 기준으로 새로 판정한다. */
			m_LodBand = Band_From_Size(size);
			m_LodBandCamera = camera.get();
		}
		else
		{
			switch (m_LodBand)
			{
			case 0:
				if (size < leave0 * (1.f - LOD_HYSTERESIS)) m_LodBand = 1;
				break;
			case 1:
				if (size < leave1 * (1.f - LOD_HYSTERESIS)) m_LodBand = 2;
				else if (size > leave0 * (1.f + LOD_HYSTERESIS)) m_LodBand = 0;
				break;
			default:
				if (size > leave1 * (1.f + LOD_HYSTERESIS)) m_LodBand = 1;
				break;
			}
		}

		// LOD2가 아예 없는 타일은 밴드2로 내려가도 볼 게 없으므로 밴드1에 머문다.
		if (m_LodBand == 2 && m_LodBand2.empty())
			m_LodBand = 1;


	}

	if (m_LodBand != 0)
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
