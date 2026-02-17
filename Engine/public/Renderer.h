#pragma once
#include "EngineManager.h"

NS_BEGIN(Engine)

class GameObject;

class Renderer final : public EngineManager {
  NO_COPY(Renderer)
public:
  explicit Renderer(const ComPtr<ID3D11Device> &device,
                    const ComPtr<ID3D11DeviceContext> &context);
  ~Renderer() override = default;

public:
  uint32 Get_LayerBits() const { return m_LayerMask; }

public:
  void Add_RenderGroup(RENDERGROUP renderGroup,
                       const Shared<GameObject> &gameObject);
  void Draw();

public:
  HRESULT Initialize(void *arg) override;
  void On_Destroy() override;
  void On_Disable() override;
  void On_Enable() override;
  void Set_Active(Bool isActive) override;

private:
  void Render_Group(uint32 groupIndex);

private:
  ComPtr<ID3D11Device> m_Device = {nullptr};
  ComPtr<ID3D11DeviceContext> m_Context = {nullptr};
  array<vector<Shared<GameObject>>, ETOI(RENDERGROUP::END)> m_RenderGroup;
  uint32 m_LayerMask = {ETOI(LAYER::ALL_LAYER)};

public:
  static Unique<Renderer> Create(const ComPtr<ID3D11Device> &device,
                                 const ComPtr<ID3D11DeviceContext> &context);
};

NS_END