#pragma once
#include "Engine_Define.h"
#include "Object.h"


NS_BEGIN(Engine)
class GraphicDevice final : public Object {
private:
  struct OffscreenRenderTarget {
    ComPtr<ID3D11Texture2D> texture;
    ComPtr<ID3D11RenderTargetView> RTV;
    ComPtr<ID3D11ShaderResourceView> SRV;
    ComPtr<ID3D11DepthStencilView> DSV;
    D3D11_VIEWPORT viewport;
  };

public:
	GraphicDevice() = default;
	~GraphicDevice() override;

public:
	ComPtr<ID3D11Device> Get_Device() const { return m_Device; }
	ComPtr<ID3D11DeviceContext> Get_Context() const { return m_Context; }
    const D3D11_VIEWPORT& Get_ViewportDesc() const { return m_ViewPort; }

public:
    PROTOTYPE Get_Prototype() const override { return PROTOTYPE::OBJECT; }
    HRESULT Initialize(const ENGINE_DESC &engineDesc);
    void On_Destroy() override;
    HRESULT Clear_BackBufferView(const Shared<Float4> &clearColor) const;
    HRESULT Clear_DepthStencilView() const;
    HRESULT Present() const;
    HRESULT OnResize(uint32 width, uint32 height, uint32 screenIndex = UINT_MAX);

public:
    HRESULT Begin_RenderOffScreen(uint32 screenIndex);
    HRESULT End_RenderOffScreen();
    ComPtr<ID3D11ShaderResourceView> Get_OffscreenSRV(uint32 screenIndex);

private:
  HRESULT Ready_SwapChain(HWND hWnd, WINMODE isWindowed, uint32 winSizeX, uint32 winSizeY);
  HRESULT Ready_BackBufferRenderTargetView();
  HRESULT Ready_DepthStencilView(uint32 winSizeX, uint32 winSizeY);
  HRESULT Create_OffScreenTarget(uint32 width, uint32 height);

private:
  ComPtr<ID3D11Device> m_Device = {nullptr};
  ComPtr<ID3D11DeviceContext> m_Context = {nullptr};
  ComPtr<IDXGISwapChain1> m_SwapChain = {nullptr};

  ComPtr<ID3D11RenderTargetView> m_RTV = {nullptr};
  ComPtr<ID3D11DepthStencilView> m_DSV = {nullptr};
  D3D11_VIEWPORT m_ViewPort = {};

  vector<OffscreenRenderTarget> m_Offscreens;

public:
  static Unique<GraphicDevice> Create(_In_ const ENGINE_DESC &engineDesc);
};

NS_END