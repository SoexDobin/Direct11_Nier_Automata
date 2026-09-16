#include "Navigation.h"

#include "Game.h"
#include "GameObject.h"
#include "SpdLogger.h"
#include "Transform.h"
#include "String_Helper.h"

#ifdef _DEBUG
Bool Navigation::s_DebugRender = false;
#endif

Navigation::Navigation() : Component{} {}
Navigation::Navigation(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) 
	: Component{ device, context } {}
Navigation::Navigation(const Navigation& rhs)
	: Component{ rhs }
	, m_CurrentCellIndex{ -1 }      
	, m_Cells{ rhs.m_Cells }
	, m_BakeWorldMatrix{ rhs.m_BakeWorldMatrix }
	, m_AnchorWorldMatrix{ rhs.m_AnchorWorldMatrix }
	, m_AnchorObjectGuid{ rhs.m_AnchorObjectGuid } {}

HRESULT Navigation::Initialize_Prototype()
{
	return Component::Initialize_Prototype();
}

HRESULT Navigation::Initialize(void* arg)
{
	if (FAILED(Component::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize Navigation");
		return E_FAIL;
	}

#ifdef _DEBUG
	if (FAILED(Ready_Debug()))
	{
		LOG_ERROR(L"Failed To Ready Navigation Debug Draw");
		return E_FAIL;
	}
#endif

	auto* desc = static_cast<NAVIGATION_DESC*>(arg);
	m_CurrentCellIndex = desc ? desc->startCellIndex : 0;
	if (m_CurrentCellIndex >= 0 && m_CurrentCellIndex < static_cast<int32>(m_Cells.size()))
	{
		
	}

	return S_OK;
}

void Navigation::On_Destroy()
{
	m_Cells.clear();
	Component::On_Destroy();
}

void Navigation::Set_NavCells(vector<NavCell>&& cells)
{
	m_Cells = std::move(cells);
	LOG_INFO(L"[Navigation] NavCells 등록: {} cells", m_Cells.size());
}

HRESULT Navigation::Build_FromMesh(
	const Float* vertices, int32 numVertices,
	const int32* triangles, int32 numTriangles,
	const NavigationBuilder::NAV_BUILD_PARAMS_DESC& params)
{
	auto buildResult = GAME_INSTANCE->Build_Navigation(vertices, numVertices, triangles, numTriangles, params);
	if (!buildResult.isSuccess)
	{
		LOG_ERROR(L"[Navigation] Build_FromMesh 실패");
		return E_FAIL;
	}
	m_Cells = std::move(buildResult.navCells);
	m_CurrentCellIndex = 0;
	return S_OK;
}

HRESULT Navigation::Load_FromBinary(const string& filePath)
{
	NavigationBuilder::NAV_IMPORT_INFO importInfo{};
	auto cells = GAME_INSTANCE->Import_Navigation(filePath, &importInfo);
	if (cells.empty())
	{
		LOG_ERROR(L"[Navigation] Load_FromBinary Failed : {}",
			Helper::To_wString(filePath));
		return E_FAIL;
	}
	m_Cells = std::move(cells);
	m_CurrentCellIndex = 0;
	m_BakeWorldMatrix = importInfo.bakeWorldMatrix;
	// 로드 직후 셀은 bake 공간에 있다. anchor를 찾으면 그때 현재 공간으로 옮긴다.
	m_AnchorWorldMatrix = m_BakeWorldMatrix;
	m_AnchorObjectGuid = importInfo.anchorObjectGuid;
	m_AnchorObject.reset();
	m_AnchorUnsupported = false;

	const Vector3 bakeTranslation = m_BakeWorldMatrix.Translation();
	LOG_INFO(L"[Navigation] Loaded {} cells from binary (bake origin {:.2f}, {:.2f}, {:.2f})",
		m_Cells.size(), bakeTranslation.x, bakeTranslation.y, bakeTranslation.z);
	return S_OK;
}

Bool Navigation::Has_NeighborCell(const Vector3& position)
{
	Ensure_Anchored();

	// 재정렬 직후에는 현재 셀이 무효다. 셀을 옮긴 공간에서 다시 찾는다.
	if (m_CurrentCellIndex < 0 && !m_Cells.empty())
		Compute_CurrentCellByPosition(position);

	if (m_CurrentCellIndex < 0 || m_CurrentCellIndex >= static_cast<int32>(m_Cells.size()))
		return false;

	int32 neighborIndex{ -1 };
	if (m_Cells[m_CurrentCellIndex].IsIn(position, &neighborIndex))
		return true;

	constexpr int32 maxChain = 16;
	int32 chain = 0;

	while (neighborIndex != -1 && chain++ < maxChain)
	{
		int32 nextNeighbor = -1;
		if (m_Cells[neighborIndex].IsIn(position, &nextNeighbor))
		{
			m_CurrentCellIndex = neighborIndex;
			return true;
		}

		neighborIndex = nextNeighbor;
	}

	return false; // 전체 셀을 뒤져도 이웃 없음 (완전 NavMesh 밖)
}

void Navigation::Compute_Height(const Shared<Transform>& transform)
{
	Ensure_Anchored();

	// 재정렬 직후에는 현재 셀이 무효다. 셀을 옮긴 공간에서 다시 찾는다.
	if (m_CurrentCellIndex < 0 && !m_Cells.empty())
		Compute_CurrentCellByPosition(transform->Get_Position());

	if (m_CurrentCellIndex < 0 || m_CurrentCellIndex >= static_cast<int32>(m_Cells.size()))
		return;

	Vector3 pos = transform->Get_Position();
	Float y = m_Cells[m_CurrentCellIndex].Compute_Height(pos.x, pos.z);
	transform->Set_Position(pos.x, y, pos.z);
}

Bool Navigation::Compute_CurrentCellByPosition(const Vector3& position)
{
	Ensure_Anchored();

	int32 bestIndex = -1;
	Float minHeightDiff = FLT_MAX;

	for (int32 i = 0; i < m_Cells.size(); ++i)
	{
		int32 neighborIndex = -1;
		if (m_Cells[i].IsIn(position, &neighborIndex))
		{
			Float cellHeight = m_Cells[i].Compute_Height(position.x, position.z);
			Float heightDiff = fabsf(position.y - cellHeight);

			if (heightDiff < minHeightDiff)
			{
				minHeightDiff = heightDiff;
				bestIndex = i;
			}
		}
	}

	if (bestIndex != -1)
	{
		m_CurrentCellIndex = bestIndex;
		return true;
	}

	LOG_WARN(L"[Navigation] Failed to find Navigation Cell at Initial Position!");
	m_CurrentCellIndex = m_Cells.empty() ? -1 : 0;
	return false;
}

Float Navigation::Get_HeightAtPoint(const Vector3& position) const
{
	if (m_CurrentCellIndex < 0 || m_CurrentCellIndex >= static_cast<int32>(m_Cells.size()))
		return 0.f;

	return m_Cells[m_CurrentCellIndex].Compute_Height(position.x, position.z);
}

void Navigation::Ensure_Anchored()
{
	// anchor 없이 구운 navmesh는 옮길 기준이 없다. bake 공간 그대로 쓴다.
	if (!m_AnchorObjectGuid.Is_Valid() || m_AnchorUnsupported)
		return;

	Shared<GameObject> anchor = m_AnchorObject.lock();
	if (!anchor)
	{
		// anchor가 아직 레벨에 없을 수 있다. 다음 질의에서 다시 찾는다.
		anchor = GAME_INSTANCE->Find(m_AnchorObjectGuid);
		if (!anchor)
			return;

		m_AnchorObject = anchor;
	}

	const Shared<Transform> transform = anchor->Get_Transform();
	if (!transform)
		return;

	Rebase_ToWorld(transform->Get_WorldMatrix());
}

Bool Navigation::Rebase_ToWorld(const Matrix& anchorWorldMatrix)
{
	if (m_Cells.empty())
		return false;

	// 지금 놓인 공간에서 목표 공간으로 가는 차이만큼만 옮긴다.
	Matrix delta = m_AnchorWorldMatrix.Invert() * anchorWorldMatrix;

	constexpr Float epsilon = 0.0001f;
	Bool isIdentity = true;
	for (int32 row = 0; row < 4 && isIdentity; ++row)
	{
		for (int32 col = 0; col < 4; ++col)
		{
			const Float expected = (row == col) ? 1.f : 0.f;
			if (fabsf(delta.m[row][col] - expected) > epsilon)
			{
				isIdentity = false;
				break;
			}
		}
	}

	if (isIdentity)
		return false;

	// NavCell의 내/외 판별과 높이 보간은 XZ 평면 투영이다. Y축을 기울이거나
	// 축마다 다른 스케일을 주면 셀 판정이 깨지므로 재bake를 요구한다.
	Vector3 deltaScale{};
	Quaternion deltaRotation{};
	Vector3 deltaTranslation{};
	if (!delta.Decompose(deltaScale, deltaRotation, deltaTranslation))
	{
		LOG_ERROR(L"[Navigation] Rebase_ToWorld: 분해할 수 없는 행렬입니다. 재bake가 필요합니다.");
		m_AnchorUnsupported = true;
		return false;
	}

	const Vector3 rotatedUp = Vector3::Transform(Vector3::Up, deltaRotation);
	const Bool isUpPreserved = fabsf(rotatedUp.x) < 0.001f && fabsf(rotatedUp.z) < 0.001f && rotatedUp.y > 0.f;
	const Bool isUniformScale = fabsf(deltaScale.x - deltaScale.y) < 0.001f &&
								fabsf(deltaScale.y - deltaScale.z) < 0.001f;

	if (!isUpPreserved || !isUniformScale)
	{
		LOG_ERROR(L"[Navigation] Rebase_ToWorld: 이동/Y축 회전/균등 스케일만 지원합니다. "
			L"(up {:.3f}, {:.3f}, {:.3f} / scale {:.3f}, {:.3f}, {:.3f}) 재bake가 필요합니다.",
			rotatedUp.x, rotatedUp.y, rotatedUp.z, deltaScale.x, deltaScale.y, deltaScale.z);
		m_AnchorUnsupported = true;
		return false;
	}

	for (auto& cell : m_Cells)
		cell.Transform_By(delta);

	m_AnchorWorldMatrix = anchorWorldMatrix;
	m_CurrentCellIndex = -1; // 옮긴 뒤의 현재 셀은 다시 찾아야 한다.

	LOG_INFO(L"[Navigation] Rebase_ToWorld: {} cells 재정렬 (이동 {:.2f}, {:.2f}, {:.2f})",
		m_Cells.size(), deltaTranslation.x, deltaTranslation.y, deltaTranslation.z);

	return true;
}

Shared<Navigation> Navigation::CreatePrototype()
{
	auto nav = make_shared<Navigation>(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context());

	if (FAILED(nav->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Initialize Prototype : Navigation");
		return nullptr;
	}

	return nav;
}
Shared<Navigation> Navigation::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto nav = make_shared<Navigation>(device, context);
	if (FAILED(nav->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Navigation");
		return nullptr;
	}
	return nav;
}
Shared<Component> Navigation::Clone(void* arg)
{
	auto nav = make_shared<Navigation>(*this);
	if (FAILED(nav->Initialize(arg)))
	{
		MSG_BOX("Failed to Cloned : Navigation");
		return nullptr;
	}
	return nav;
}

#ifdef _DEBUG
HRESULT Navigation::Ready_Debug()
{
	m_Batch = make_shared<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>(m_Context.Get());
	m_Effect = make_shared<DirectX::BasicEffect>(m_Device.Get());
	m_Effect->SetVertexColorEnabled(true);
	const void* shaderByteCode;
	size_t byteCodeLength;
	m_Effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);
	if (FAILED(m_Device->CreateInputLayout(DirectX::VertexPositionColor::InputElements,
		DirectX::VertexPositionColor::InputElementCount,
		shaderByteCode, byteCodeLength,
		m_InputLayout.GetAddressOf())))
	{
		return E_FAIL;
	}
	return S_OK;
}
HRESULT Navigation::Render_Debug()
{
	if (!s_DebugRender) return S_OK;

	if (!m_Batch || !m_Effect) return E_FAIL;

	Ensure_Anchored();
	m_Effect->SetView(GAME_INSTANCE->Get_Transform(D3DTS::VIEW));
	m_Effect->SetProjection(GAME_INSTANCE->Get_Transform(D3DTS::PROJ));

	m_Effect->SetWorld(Matrix::Identity);
	m_Context->IASetInputLayout(m_InputLayout.Get());
	m_Effect->Apply(m_Context.Get());
	m_Batch->Begin();
	for (int32 i = 0; i < m_Cells.size(); ++i)
	{
		// 플레이어가 위치한 현재 셀은 붉은색, 나머지는 초록색 라인으로 긋습니다.
		XMVECTORF32 renderColor = (i == m_CurrentCellIndex) ? Colors::Red : Colors::Green;
		m_Cells[i].Render_Debug(m_Batch, static_cast<Color>(renderColor));
	}
	m_Batch->End();
	return S_OK;
}
#endif
