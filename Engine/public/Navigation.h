#pragma once
#include "Component.h"
#include "NavCell.h"
#include "NavigationBuilder.h"

NS_BEGIN(Engine)

class Shader;
class Transform;

class ENGINE_DLL Navigation final : public Component
{
	RTTR_ENABLE(Component)
public:
	typedef struct tagNavigationDesc : public COMPONENT_DESC
	{
		int32 startCellIndex{ 0 };
	} NAVIGATION_DESC;

public:
	explicit Navigation();
	explicit Navigation(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Navigation(const Navigation& rhs);
	~Navigation() override = default;

public:
	COMPONENT_TYPE Get_ComponentType() const override { return COMPONENT_TYPE::NAVIGATION; }
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;
	
public: /* build nav */
	vector<NavCell> Get_NavCells() const { return m_Cells; }
	void Set_NavCells(vector<NavCell>&& cells);
	/// 소스 메시로부터 직접 빌드
	HRESULT Build_FromMesh(
		const Float* vertices, int32 numVertices,
		const int32* triangles, int32 numTriangles,
		const NavigationBuilder::NAV_BUILD_PARAMS_DESC& params = {}
	);

public:
	Bool Has_NeighborCell(const Vector3& position);
	void Compute_Height(const Shared<Transform>& transform);
	Float Get_HeightAtPoint(Float pointX, Float pointZ) const;

public:
	uint32 Get_NumCells() const { return static_cast<uint32>(m_Cells.size()); }
	int32 Get_CurrentCellIndex() const { return m_CurrentCellIndex; }
	void Set_CurrentCellIndex(int32 cellIndex) { m_CurrentCellIndex = cellIndex; }

private:
	int32 m_CurrentCellIndex{ -1 };
	vector<NavCell> m_Cells;

public:
	static Shared<Navigation> CreatePrototype();
	static Shared<Navigation> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<Component> Clone(void* arg = nullptr) override;
};

NS_END