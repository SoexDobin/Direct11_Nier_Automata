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
	HRESULT Build_FromMesh(
		const Float* vertices, int32 numVertices,
		const int32* triangles, int32 numTriangles,
		const NavigationBuilder::NAV_BUILD_PARAMS_DESC& params = {}
	);
	/// .nnav 바이너리 파일에서 NavCell 로드 (이웃 정보 포함, SetUp_Neighbors 불필요)
	HRESULT Load_FromBinary(const string& filePath);

public:
	Bool Has_NeighborCell(const Vector3& position);
	void Compute_Height(const Shared<Transform>& transform);
	Float Get_HeightAtPoint(const Vector3& position) const;
	Bool Compute_CurrentCellByPosition(const Vector3& position);

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

#ifdef _DEBUG
private:
	static Bool s_DebugRender;
	Shared<PrimitiveBatch<DirectX::VertexPositionColor>> m_Batch { nullptr };
	Shared<BasicEffect> m_Effect{ nullptr };
	ComPtr<ID3D11InputLayout> m_InputLayout { nullptr };

public:
	HRESULT Ready_Debug();
	HRESULT Render_Debug();
	static void Toggle_DebugRender() { s_DebugRender = !s_DebugRender; }
	static Bool Get_DebugRender() { return s_DebugRender; }
#endif
};

NS_END