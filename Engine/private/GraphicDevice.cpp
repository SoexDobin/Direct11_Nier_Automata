#include "GraphicDevice.h"
#include "Game.h"
#include "SpdLogger.h"

GraphicDevice::~GraphicDevice() {}

HRESULT GraphicDevice::Initialize(const ENGINE_DESC &engineDesc) {
  uint32 flag = {};

  if constexpr (_DEBUG)
    flag |= D3D11_CREATE_DEVICE_DEBUG;

  D3D_FEATURE_LEVEL feature_Level = {};
  if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flag,
                               nullptr, 0, D3D11_SDK_VERSION,
                               m_Device.GetAddressOf(), &feature_Level,
                               m_Context.GetAddressOf()))) {
    return E_FAIL;
  }

  if (FAILED(Ready_SwapChain(engineDesc.hWnd, engineDesc.winMode,
                             engineDesc.viewportWidth,
                             engineDesc.viewportHeight)))
    return E_FAIL;

  if (FAILED(Ready_BackBufferRenderTargetView()))
    return E_FAIL;

  if (FAILED(Ready_DepthStencilView(engineDesc.viewportWidth,
                                    engineDesc.viewportHeight)))
    return E_FAIL;

  { // Set Viewport
    ComPtr<ID3D11RenderTargetView> RTVs[1] = {m_RTV};
    m_Context->OMSetRenderTargets(1, RTVs[0].GetAddressOf(), m_DSV.Get());

    m_ViewPort.TopLeftX = 0.f;
    m_ViewPort.TopLeftY = 0.f;
    m_ViewPort.Width = static_cast<Float>(engineDesc.viewportWidth);
    m_ViewPort.Height = static_cast<Float>(engineDesc.viewportHeight);
    m_ViewPort.MinDepth = 0.f;
    m_ViewPort.MaxDepth = 1.f;

    m_Context->RSSetViewports(1, &m_ViewPort);
  }

  if (engineDesc.useOffscreenRendering && engineDesc.renderTargetCount > 0) {
    for (uint32 i = 0; i < engineDesc.renderTargetCount; ++i) {

      if (FAILED(Create_OffScreenTarget(engineDesc.viewportWidth,
                                        engineDesc.viewportHeight)))
        return E_FAIL;
    }
  }

  return S_OK;
}

void GraphicDevice::On_Destroy() {
  m_Offscreens.clear();
  Object::On_Destroy();
}

HRESULT
GraphicDevice::Clear_BackBufferView(const Shared<Float4> &clearColor) const {
  if (nullptr == m_Context)
    return E_FAIL;

  m_Context->ClearRenderTargetView(
      m_RTV.Get(), reinterpret_cast<const Float *>(clearColor.get()));

  return S_OK;
}

HRESULT GraphicDevice::Clear_DepthStencilView() const {
  if (nullptr == m_Context)
    return E_FAIL;

  m_Context->ClearDepthStencilView(
      m_DSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

  return S_OK;
}

void GraphicDevice::Set_DepthStencilState(ID3D11DepthStencilState* state, UINT ref)
{
    m_Context->OMSetDepthStencilState(state, ref);
}

HRESULT GraphicDevice::Present() const {
  if (nullptr == m_SwapChain)
    return E_FAIL;

  m_Context->OMSetRenderTargets(0, nullptr, nullptr);

  return m_SwapChain->Present(0, 0);
}

HRESULT GraphicDevice::OnResize(uint32 width, uint32 height, uint32 screenIndex) {
  if (nullptr == m_SwapChain || nullptr == m_Device)
    return E_FAIL;

  if (width == 0 || height == 0)
    return S_OK;

  if (screenIndex != UINT_MAX &&
      screenIndex < static_cast<uint32>(m_Offscreens.size())) {
    auto &renderTarget = m_Offscreens[screenIndex];

    renderTarget.texture.Reset();
    renderTarget.RTV.Reset();
    renderTarget.SRV.Reset();
    renderTarget.DSV.Reset();

    D3D11_TEXTURE2D_DESC textureDesc{};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags =
        D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    m_Device->CreateTexture2D(&textureDesc, nullptr,
                              renderTarget.texture.GetAddressOf());
    m_Device->CreateRenderTargetView(renderTarget.texture.Get(), nullptr,
                                     renderTarget.RTV.GetAddressOf());
    m_Device->CreateShaderResourceView(renderTarget.texture.Get(), nullptr,
                                       renderTarget.SRV.GetAddressOf());

    D3D11_TEXTURE2D_DESC depthStencilDesc{};
    depthStencilDesc.Width = width;
    depthStencilDesc.Height = height;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ComPtr<ID3D11Texture2D> depthStencilTex;
    m_Device->CreateTexture2D(&depthStencilDesc, nullptr,
                              depthStencilTex.GetAddressOf());
    m_Device->CreateDepthStencilView(depthStencilTex.Get(), nullptr,
                                     renderTarget.DSV.GetAddressOf());

    renderTarget.viewport.Width = static_cast<Float>(width);
    renderTarget.viewport.Height = static_cast<Float>(height);
    return S_OK;
  }

  m_Context->OMSetRenderTargets(0, nullptr, nullptr);
  m_RTV.Reset();
  m_DSV.Reset();
  m_Context->ClearState();
  m_Context->Flush();

  if (FAILED(
          m_SwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0)))
    return E_FAIL;

  if (FAILED(Ready_BackBufferRenderTargetView()))
    return E_FAIL;

  if (FAILED(Ready_DepthStencilView(width, height)))
    return E_FAIL;

  m_ViewPort.Width = static_cast<Float>(width);
  m_ViewPort.Height = static_cast<Float>(height);

  uint32 size = m_Offscreens.size();
  m_Offscreens.clear();

  for (uint32 i = 0; i < size; ++i) {
    if (FAILED(Create_OffScreenTarget(width, height)))
      return E_FAIL;
  }

  ID3D11RenderTargetView *rtvs[] = {m_RTV.Get()};
  m_Context->OMSetRenderTargets(1, rtvs, m_DSV.Get());
  m_Context->RSSetViewports(1, &m_ViewPort);

  return S_OK;
}

HRESULT GraphicDevice::Begin_RenderOffScreen(uint32 screenIndex) {
  if (screenIndex >= m_Offscreens.size()) {
    LOG_CRITICAL(L"Invalid Offscreen Index: {}", screenIndex);
    return E_FAIL;
  }

  ID3D11ShaderResourceView *nullSRV[1] = {nullptr};
  m_Context->PSSetShaderResources(0, 1, nullSRV);

  auto &rt = m_Offscreens[screenIndex];

  Float clearColor[4] = { 0.f, 0.f, 0.f, 1.f };
  m_Context->ClearRenderTargetView(rt.RTV.Get(), clearColor);

  m_Context->ClearDepthStencilView(
      rt.DSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0.f);
  ID3D11RenderTargetView *RTVs[] = {rt.RTV.Get()};
  m_Context->OMSetRenderTargets(1, RTVs, rt.DSV.Get()); // rt.DSV 사용!
  m_Context->RSSetViewports(1, &rt.viewport);

  return S_OK;
}

HRESULT GraphicDevice::End_RenderOffScreen() {
  ID3D11RenderTargetView *RTVs[] = {m_RTV.Get()};
  m_Context->OMSetRenderTargets(1, RTVs, m_DSV.Get());
  m_Context->RSSetViewports(1, &m_ViewPort);

  return S_OK;
}

ComPtr<ID3D11ShaderResourceView>
GraphicDevice::Get_OffscreenSRV(uint32 screenIndex) {
  if (screenIndex >= m_Offscreens.size()) {
    LOG_CRITICAL(L"Invalid Offscreen Index: {}", screenIndex);
    return nullptr;
  }

  return m_Offscreens[screenIndex].SRV;
}

HRESULT GraphicDevice::Ready_SwapChain(HWND hWnd, WINMODE isWindowed,
                                       uint32 winSizeX, uint32 winSizeY) {
  ComPtr<IDXGIDevice> device = {nullptr};
  if (FAILED(m_Device->QueryInterface(
          __uuidof(IDXGIDevice),
          reinterpret_cast<void **>(device.GetAddressOf()))))
    return E_FAIL;

  ComPtr<IDXGIAdapter> adapter = {nullptr};
  if (FAILED(
          device->GetParent(__uuidof(IDXGIAdapter),
                            reinterpret_cast<void **>(adapter.GetAddressOf()))))
    return E_FAIL;

  ComPtr<IDXGIFactory2> factory2 = {nullptr};
  if (FAILED(adapter->GetParent(
          __uuidof(IDXGIFactory2),
          reinterpret_cast<void **>(factory2.GetAddressOf()))))
    return E_FAIL;

  DXGI_SWAP_CHAIN_DESC1 swapChain = {};
  ZeroMemory(&swapChain, sizeof(DXGI_SWAP_CHAIN_DESC1));
  {
    swapChain.Width = winSizeX;
    swapChain.Height = winSizeY;
    swapChain.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChain.BufferCount = 2;
    swapChain.SampleDesc.Quality = 0;
    swapChain.SampleDesc.Count = 1;
    swapChain.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChain.Scaling = DXGI_SCALING_STRETCH;
    swapChain.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
  }

  DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullScreenSwapChain = {};
  fullScreenSwapChain.RefreshRate = {.Numerator = 60, .Denominator = 1};
  fullScreenSwapChain.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  fullScreenSwapChain.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
  fullScreenSwapChain.Windowed = static_cast<Bool>(isWindowed);

  return factory2->CreateSwapChainForHwnd(m_Device.Get(), hWnd, &swapChain,
                                          &fullScreenSwapChain, nullptr,
                                          m_SwapChain.ReleaseAndGetAddressOf());
}

HRESULT GraphicDevice::Ready_BackBufferRenderTargetView() {
  if (nullptr == m_Device)
    return E_FAIL;

  ComPtr<ID3D11Texture2D> backBufferTexture = {nullptr};

  if (FAILED(m_SwapChain->GetBuffer(
          0, __uuidof(ID3D11Texture2D),
          reinterpret_cast<void **>(backBufferTexture.GetAddressOf()))))
    return E_FAIL;
  if (FAILED(m_Device->CreateRenderTargetView(backBufferTexture.Get(), nullptr,
                                              m_RTV.GetAddressOf())))
    return E_FAIL;

  return S_OK;
}

HRESULT GraphicDevice::Ready_DepthStencilView(uint32 winSizeX,
                                              uint32 winSizeY) {
  if (nullptr == m_Device)
    return E_FAIL;

  ComPtr<ID3D11Texture2D> depthStencilTexture = {nullptr};
  D3D11_TEXTURE2D_DESC textureDesc = {};
  {
    textureDesc.Width = winSizeX;
    textureDesc.Height = winSizeY;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    textureDesc.SampleDesc.Quality = 0;
    textureDesc.SampleDesc.Count = 1;

    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;
  }

  if (FAILED(m_Device->CreateTexture2D(&textureDesc, nullptr,
                                       depthStencilTexture.GetAddressOf())))
    return E_FAIL;

  if (FAILED(m_Device->CreateDepthStencilView(depthStencilTexture.Get(),
                                              nullptr, m_DSV.GetAddressOf())))
    return E_FAIL;

  return S_OK;
}

HRESULT GraphicDevice::Create_OffScreenTarget(uint32 width, uint32 height) {
  OffscreenRenderTarget rt = {};
  D3D11_TEXTURE2D_DESC textureDesc = {};
  {
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags =
        D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
  }

  if (FAILED(m_Device->CreateTexture2D(&textureDesc, nullptr,
                                       rt.texture.GetAddressOf())))
    return E_FAIL;

  if (FAILED(m_Device->CreateRenderTargetView(rt.texture.Get(), nullptr,
                                              rt.RTV.GetAddressOf())))
    return E_FAIL;

  if (FAILED(m_Device->CreateShaderResourceView(rt.texture.Get(), nullptr,
                                                rt.SRV.GetAddressOf())))
    return E_FAIL;

  D3D11_TEXTURE2D_DESC dsDesc = {};
  dsDesc.Width = width;
  dsDesc.Height = height;
  dsDesc.MipLevels = 1;
  dsDesc.ArraySize = 1;
  dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  dsDesc.SampleDesc.Count = 1;
  dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  ComPtr<ID3D11Texture2D> depthStencilTex;
  if (FAILED(m_Device->CreateTexture2D(&dsDesc, nullptr,
                                       depthStencilTex.GetAddressOf())))
    return E_FAIL;
  if (FAILED(m_Device->CreateDepthStencilView(depthStencilTex.Get(), nullptr,
                                              rt.DSV.GetAddressOf())))
    return E_FAIL;

  rt.viewport.TopLeftX = 0.f;
  rt.viewport.TopLeftY = 0.f;
  rt.viewport.Width = static_cast<Float>(width);
  rt.viewport.Height = static_cast<Float>(height);
  rt.viewport.MinDepth = 0.f;
  rt.viewport.MaxDepth = 1.f;

  m_Offscreens.push_back(rt);

  return S_OK;
}

Unique<GraphicDevice>
GraphicDevice::Create(_In_ const ENGINE_DESC &engineDesc) {
  auto pInstance = make_unique<GraphicDevice>();

  if (FAILED(pInstance->Initialize(engineDesc))) {
    MSG_BOX("Failed to Created : CGraphic_Device");
  }

  return pInstance;
}
