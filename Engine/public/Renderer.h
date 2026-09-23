#pragma once
#include "EngineManager.h"

NS_BEGIN(Engine)

class GameObject;
class Shader;
class VIBuffer_Rect;

class Renderer final : public EngineManager {
	NO_COPY(Renderer)
public:
	explicit Renderer(const ComPtr<ID3D11Device> &device, const ComPtr<ID3D11DeviceContext> &context);
	~Renderer() override = default;

public:
  uint32 Get_LayerBits() const { return m_LayerMask; }

public:
	// 프러스텀 컬링 스위치. 기본은 켜짐이고 Debug에서 전후 비교용으로만 끈다.
	Bool Toggle_FrustumCulling() { m_FrustumCulling = !m_FrustumCulling; return m_FrustumCulling; }
	Bool Get_FrustumCulling() const { return m_FrustumCulling; }
	// 컬링으로 건너뛴 메시 수. draw 통계와 같은 주기로 넘어간다.
	void Add_CulledMesh() { ++m_CulledCount; }
	uint32 Get_FrameCulledCount() const { return m_LastCulledCount; }

	// 월드 타일 LOD 스위치. 컬링과 같은 이유로 기본은 켜짐이고 전후 비교용으로만 끈다.
	Bool Toggle_WorldLod() { m_WorldLod = !m_WorldLod; return m_WorldLod; }
	Bool Get_WorldLod() const { return m_WorldLod; }
	// LOD 메시로 그린 타일 수. 컬링 카운터와 같은 주기로 넘어간다.
	void Add_LodTile() { ++m_LodTileCount; }
	uint32 Get_FrameLodTileCount() const { return m_LastLodTileCount; }

public:
	void Add_RenderGroup(RENDERGROUP renderGroup, const Shared<GameObject> &gameObject);
	void Draw();
	void Draw_NoClearing();
	HRESULT Clear_RenderGroup();

public:
	HRESULT Initialize(void *arg) override;
	void On_Destroy() override;
	void On_Disable() override;
	void On_Enable() override;
	void Set_Active(Bool isActive) override;

private:
	void Render_Deferred() const;
	void Render_Lights() const;
	void Render_Group(uint32 groupIndex) const;
	void Render_Recursive(const Shared<GameObject>& object) const;

private:
	ComPtr<ID3D11Device> m_Device = {nullptr};
	ComPtr<ID3D11DeviceContext> m_Context = {nullptr};
	array<vector<Shared<GameObject>>, ETOI(RENDERGROUP::END)> m_RenderGroup;
	uint32 m_LayerMask = {ETOI(LAYER::ALL_LAYER)};
	Bool m_FrustumCulling{ true };
	uint32 m_CulledCount{};
	uint32 m_LastCulledCount{};
	Bool m_WorldLod{ true };
	uint32 m_LodTileCount{};
	uint32 m_LastLodTileCount{};

private: /* For DefShader */
	Matrix m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};
	Shared<Shader> m_Shader{ nullptr };
	Shared<VIBuffer_Rect> m_Buffer{ nullptr };

public:
	static Unique<Renderer> Create(const ComPtr<ID3D11Device> &device, const ComPtr<ID3D11DeviceContext> &context);


#ifdef _DEBUG
public:
	Bool Toggle_RenderTargetDebug() { m_ShowRenderTargets = !m_ShowRenderTargets; return m_ShowRenderTargets; }
	Bool Get_RenderTargetDebug() const { return m_ShowRenderTargets; }

	// 프레임 draw 통계: VIBuffer가 draw마다 더하고, Clear_RenderGroup에서 직전 프레임 값으로 넘긴다.
	void Add_DrawStats(uint32 triangleCount) { ++m_DrawCount; m_TriangleCount += triangleCount; }
	uint32 Get_FrameDrawCount() const { return m_LastDrawCount; }
	uint64_t Get_FrameTriangleCount() const { return m_LastTriangleCount; }

private:
	void Render_Debug();

	// MRT 미리보기는 필요할 때만 켠다.
	Bool m_ShowRenderTargets{ false };

	uint32 m_DrawCount{}, m_LastDrawCount{};
	uint64_t m_TriangleCount{}, m_LastTriangleCount{};
#endif
};

NS_END