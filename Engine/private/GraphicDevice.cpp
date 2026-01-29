#include "GraphicDevice.h"

HRESULT GraphicDevice::Initialize(HWND hWnd, WINMODE isWindowed, uint32 winSizeX, uint32 winSizeY,
	_Inout_ ComPtr<ID3D11Device>& device,
	_Inout_ ComPtr<ID3D11DeviceContext>& context)
{
	uint32 flag = {};
	const uint32 sizeRTV = 1;

	if constexpr (_DEBUG)
		flag |= D3D11_CREATE_DEVICE_DEBUG;

	D3D_FEATURE_LEVEL	feature_Level = {};
	if (FAILED(D3D11CreateDevice(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		flag, nullptr, 0, D3D11_SDK_VERSION,
		m_Device.GetAddressOf(), &feature_Level, m_Context.GetAddressOf())))
	{
		return E_FAIL;
	}

	if (FAILED(Ready_SwapChain(hWnd, isWindowed, winSizeX, winSizeY)))
		return E_FAIL;
	if (FAILED(Ready_BackBufferRenderTargetView()))
		return E_FAIL;
	if (FAILED(Ready_DepthStencilView(winSizeX, winSizeY)))
		return E_FAIL;

	{
		const uint32 mainViewIndex = 1;

		ComPtr<ID3D11RenderTargetView> RTVs[sizeRTV] = { m_RTV };
		m_Context->OMSetRenderTargets(mainViewIndex, RTVs[0].GetAddressOf(), m_DSV.Get());

		D3D11_VIEWPORT	viewPortDesc;
		ZeroMemory(&viewPortDesc, sizeof(D3D11_VIEWPORT));
		viewPortDesc.TopLeftX = 0.f;
		viewPortDesc.TopLeftY = 0.f;
		viewPortDesc.Width = static_cast<Float>(winSizeX);
		viewPortDesc.Height = static_cast<Float>(winSizeY);
		viewPortDesc.MinDepth = 0.f;
		viewPortDesc.MaxDepth = 1.f;

		m_Context->RSSetViewports(mainViewIndex, &viewPortDesc);
	}

	device = m_Device;
	context = m_Context;

	return S_OK;
}

HRESULT GraphicDevice::Clear_BackBufferView(const Float4* clearColor) const
{
	if (nullptr == m_Device)
		return E_FAIL;

	m_Context->ClearRenderTargetView(m_RTV.Get(),
		reinterpret_cast<const Float*>(clearColor));

	return S_OK;
}

HRESULT GraphicDevice::Clear_DepthStencilView() const
{
	if (nullptr == m_Device)
		return E_FAIL;

	m_Context->ClearDepthStencilView(m_DSV.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	return S_OK;
}

HRESULT GraphicDevice::Present() const
{
	if (nullptr == m_SwapChain)
		return E_FAIL;

	return m_SwapChain->Present(0, 0);
}

HRESULT GraphicDevice::Ready_SwapChain(HWND hWnd, WINMODE isWindowed, uint32 winSizeX, uint32 winSizeY)
{
	HRESULT hr = S_OK;

	ComPtr<IDXGIDevice> device = { nullptr };
	hr = m_Device->QueryInterface(__uuidof(IDXGIDevice),
		reinterpret_cast<void**>(device.GetAddressOf()));

	ComPtr<IDXGIAdapter> adapter = { nullptr };
	hr = device->GetParent(__uuidof(IDXGIAdapter),
		reinterpret_cast<void**>(adapter.GetAddressOf()));

	ComPtr<IDXGIFactory> factory = { nullptr };
	hr = device->GetParent(__uuidof(IDXGIFactory),
		reinterpret_cast<void**>(factory.GetAddressOf()));

	DXGI_SWAP_CHAIN_DESC	swapChain = {};
	ZeroMemory(&swapChain, sizeof(DXGI_SWAP_CHAIN_DESC));
	{
		swapChain.BufferDesc.Width = winSizeX;
		swapChain.BufferDesc.Height = winSizeY;

		swapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		swapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChain.BufferCount = 1;

		swapChain.BufferDesc.RefreshRate.Numerator = 60;
		swapChain.BufferDesc.RefreshRate.Denominator = 1;

		swapChain.SampleDesc.Quality = 0;	/* 멀티샘플링 : 안티얼라이징 (계단현상방지) */
		swapChain.SampleDesc.Count = 1;		/* 나중에 후처리 렌더링 : 멀티샘플링 지원(x) */

		swapChain.OutputWindow = hWnd;
		swapChain.Windowed = static_cast<Bool>(isWindowed);
		swapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		hr = factory->CreateSwapChain(m_Device.Get(), &swapChain, m_SwapChain.GetAddressOf());
	}

	return hr;
}

HRESULT GraphicDevice::Ready_BackBufferRenderTargetView()
{
	if (nullptr == m_Device)
		return E_FAIL;

	ComPtr<ID3D11Texture2D> backBufferTexture = { nullptr };

	if (FAILED(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBufferTexture.GetAddressOf()))))
		return E_FAIL;
	if (FAILED(m_Device->CreateRenderTargetView(backBufferTexture.Get(), nullptr, m_RTV.GetAddressOf())))
		return E_FAIL;

	return S_OK;
}

HRESULT GraphicDevice::Ready_DepthStencilView(uint32 winSizeX, uint32 winSizeY)
{
	if (nullptr == m_Device)
		return E_FAIL;

	ComPtr<ID3D11Texture2D>* depthStencilTexture = { nullptr };
	D3D11_TEXTURE2D_DESC	 textureDesc = {};
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

	if (FAILED(m_Device->CreateTexture2D(&textureDesc, nullptr, depthStencilTexture->GetAddressOf())))
		return E_FAIL;

	return S_OK;
}

Unique<GraphicDevice> GraphicDevice::Create(_In_ HWND hWnd, WINMODE isWindowed, uint32 winSizeX, uint32 winSizeY,
	_Out_ ComPtr<ID3D11Device>& device,
	_Out_ ComPtr<ID3D11DeviceContext>& context)
{
	auto pInstance = make_unique<GraphicDevice>();

	if (FAILED(pInstance->Initialize(hWnd, isWindowed, winSizeX, winSizeY, device, context)))
	{
		MSG_BOX("Failed to Created : CGraphic_Device");
	}

	return pInstance;
}
