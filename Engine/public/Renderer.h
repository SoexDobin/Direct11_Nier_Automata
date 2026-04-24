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
	HRESULT OnResize(uint32 width, uint32 height);
	HRESULT Initialize(void *arg) override;
	void On_Destroy() override;
	void On_Disable() override;
	void On_Enable() override;
	void Set_Active(Bool isActive) override;

private:
	void Render_Shadow();
	void Render_Combined() const;
	void Render_Lights() const;
	void Render_Group(uint32 groupIndex) const;
	void Render_Recursive(const Shared<GameObject>& object) const;

private:
	HRESULT Ready_ShadowDSV();
	void Change_ViewportDesc(uint32 width, uint32 height);

private:
	ComPtr<ID3D11Device> m_Device = {nullptr};
	ComPtr<ID3D11DeviceContext> m_Context = {nullptr};
	array<vector<Shared<GameObject>>, ETOI(RENDERGROUP::END)> m_RenderGroup;
	uint32 m_LayerMask = {ETOI(LAYER::ALL_LAYER)};
	Vector2 m_ViewPort{};

private: /* For DefShader */
	Matrix m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};
	Shared<Shader> m_Shader{ nullptr };
	Shared<VIBuffer_Rect> m_Buffer{ nullptr };
	ComPtr<ID3D11DepthStencilView> m_ShadowDSV{ nullptr };

public:
	static Unique<Renderer> Create(const ComPtr<ID3D11Device> &device, const ComPtr<ID3D11DeviceContext> &context);


#ifdef _DEBUG
private:
	void Render_Debug();
#endif
};

NS_END