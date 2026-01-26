#pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)

class GraphicDevice final 
{
public:
	GraphicDevice();
	~GraphicDevice() = default;

public:
    HRESULT Initialize(HWND hWnd, WINMODE isWindowed, uint32 winSizeX, uint32 iWinSizeY,
						_Inout_ ComPtr<ID3D11Device>& device,
						_Inout_ ComPtr<ID3D11DeviceContext>& context);
    HRESULT Clear_BackBufferView(const Float4 *clearColor) const;
    HRESULT Clear_DepthStencilView() const;
    HRESULT Present() const;

private:
    HRESULT Ready_SwapChain(HWND hWnd, WINMODE isWindowed, uint32 winSizeX,
                            uint32 winSizeY);
    HRESULT Ready_BackBufferRenderTargetView();
    HRESULT Ready_DepthStencilView(uint32 winSizeX, uint32 winSizeY);
    // HRESULT Ready_ShaderResourceView();

private:
    ComPtr<ID3D11Device> m_Device = {nullptr};
    ComPtr<ID3D11DeviceContext> m_Context = {nullptr};
    ComPtr<IDXGISwapChain> m_SwapChain = {nullptr};

    ComPtr<ID3D11RenderTargetView> m_RTV = {nullptr};
    ComPtr<ID3D11DepthStencilView> m_DSV = {nullptr};

public:
    static Shared<GraphicDevice>
    Create(_In_  HWND hWnd, WINMODE isWindowed, uint32 winSizeX, uint32 winSizeY,
           _Out_ ComPtr<ID3D11Device> &device,
           _Out_ ComPtr<ID3D11DeviceContext> &context);
};

NS_END