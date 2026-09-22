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