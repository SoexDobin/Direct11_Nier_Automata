#include "Navigation.h"

#include "Game.h"
#include "SpdLogger.h"
#include "Transform.h"

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
	const float* vertices, int32 numVertices,
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

	return false; // 이웃 없음 (완전 경계 밖)
}

void Navigation::Compute_Height(const Shared<Transform>& transform)
{
	if (m_CurrentCellIndex < 0 || m_CurrentCellIndex >= static_cast<int32>(m_Cells.size()))
		return;

	Vector3 pos = transform->Get_Position();
	Float y = m_Cells[m_CurrentCellIndex].Compute_Height(pos.x, pos.z);
	transform->Set_Position(pos.x, y, pos.z);
}

Float Navigation::Get_HeightAtPoint(Float pointX, Float pointZ) const
{
	if (m_CurrentCellIndex < 0 || m_CurrentCellIndex >= static_cast<int32>(m_Cells.size()))
		return 0.f;

	return m_Cells[m_CurrentCellIndex].Compute_Height(pointX, pointZ);
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