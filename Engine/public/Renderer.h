#pragma once
#include "EngineManager.h"

NS_BEGIN(Engine)

class GameObject;

class Renderer final : public EngineManager
{
	NO_COPY(Renderer)
public:
	explicit Renderer(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	~Renderer() override = default;

public:
	void Add_RenderGroup(RENDERGROUP renderGroup, const Shared<GameObject>& gameObject);
	void Draw();

public:
	HRESULT Initialize(Shared<void> arg) override;
	void On_Destroy() override;
	void On_Disable() override;
	void On_Enable() override;
	void Set_Active(Bool isActive) override;

private:
	void Render_Group(uint32 groupIndex);
	

private:
	ComPtr<ID3D11Device>										m_Device = { nullptr };
	ComPtr<ID3D11DeviceContext>									m_Context = { nullptr };
	array<vector<Shared<GameObject>>, ETOI(RENDERGROUP::END)>	m_RenderGroup;
	array<Bool, ETOI(LAYER::END)>								m_LayerEnable = { true, };

	constexpr const uint32 renderGroupSize = ETOI(RENDERGROUP::END);
	constexpr const uint32 layerSize = ETOI(LAYER::END);

public:
	static Unique<Renderer> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	
};

NS_END