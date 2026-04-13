#include "Navigation.h"

#include "Game.h"
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
	, m_Cells{ rhs.m_Cells } {}

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
	m_CurrentCellIndex = desc->startCellIndex;
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
	auto cells = NavigationBuilder::Import_Binary(filePath);
	if (cells.empty())
	{
		LOG_ERROR(L"[Navigation] Load_FromBinary 실패: {}",
			Helper::To_wString(filePath));
		return E_FAIL;
	}
	m_Cells = std::move(cells);
	m_CurrentCellIndex = 0;

	LOG_INFO(L"[Navigation] Loaded {} cells from binary", m_Cells.size());
	return S_OK;
}

Bool Navigation::Has_NeighborCell(const Vector3& position)
{
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
	if (m_CurrentCellIndex < 0 || m_CurrentCellIndex >= static_cast<int32>(m_Cells.size()))
		return;

	Vector3 pos = transform->Get_Position();
	Float y = m_Cells[m_CurrentCellIndex].Compute_Height(pos.x, pos.z);
	transform->Set_Position(pos.x, y, pos.z);
}

void Navigation::Compute_CurrentCellByPosition(const Vector3& position)
{
	for (int32 i = 0; i < m_Cells.size(); ++i)
	{
		int32 neighborIndex = -1;
		if (m_Cells[i].IsIn(position, &neighborIndex))
		{
			m_CurrentCellIndex = i;
			return;
		}
	}

	LOG_WARN(L"[Navigation] Failed to find Navigation Cell at Initial Position!");
	m_CurrentCellIndex = 0;
}

Float Navigation::Get_HeightAtPoint(const Vector3& position) const
{
	if (m_CurrentCellIndex < 0 || m_CurrentCellIndex >= static_cast<int32>(m_Cells.size()))
		return 0.f;

	return m_Cells[m_CurrentCellIndex].Compute_Height(position.x, position.z);
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