#include "RenderTarget.h"

#include "Shader.h"
#include "SpdLogger.h"
#include "VIBuffer_Rect.h"

RenderTarget::RenderTarget(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: m_Device{device}, m_Context{context}
{
}

HRESULT RenderTarget::OnResize(uint32 sizeX, uint32 sizeY)
{
	m_ShaderResourceView.Reset();
	m_RenderTargetView.Reset();
	m_Texture2D.Reset();

	return Initialize(sizeX, sizeY, m_PixelFormat, m_ClearColor, m_IsResizable);
}

HRESULT RenderTarget::Initialize(uint32 sizeX, uint32 sizeY, DXGI_FORMAT pixelFormat, const Color& clearColor, Bool isResizable)
{
	D3D11_TEXTURE2D_DESC texDesc{};
	texDesc.Width = sizeX;
	texDesc.Height = sizeY;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = pixelFormat;
	texDesc.SampleDesc.Quality = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	if (FAILED(m_Device->CreateTexture2D(&texDesc, nullptr, m_Texture2D.GetAddressOf())))
		return E_FAIL;

	if (FAILED(m_Device->CreateRenderTargetView(m_Texture2D.Get(), nullptr, m_RenderTargetView.GetAddressOf())))
		return E_FAIL;

	if (FAILED(m_Device->CreateShaderResourceView(m_Texture2D.Get(), nullptr, m_ShaderResourceView.GetAddressOf())))
		return E_FAIL;

	m_ClearColor = clearColor;
	m_PixelFormat = pixelFormat;

	m_IsResizable = isResizable;

	return S_OK;
}

HRESULT RenderTarget::Begin()
{
	return Object::Begin();
}

HRESULT RenderTarget::Bind_ShaderResource(const Shared<Shader>& shader, const Char* constantName) const
{
	return shader->Bind_SRV(constantName, m_ShaderResourceView);
}

void RenderTarget::Clear_RenderTarget() const
{
	if (m_RenderTargetView)
		m_Context->ClearRenderTargetView(m_RenderTargetView.Get(), m_ClearColor);
}

Shared<RenderTarget> RenderTarget::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, uint32 sizeX, uint32 sizeY, DXGI_FORMAT pixelFormat, const Color& clearColor, Bool isResizable)
{
	auto renderTarget = make_shared<RenderTarget>(device, context);

	if (FAILED(renderTarget->Initialize(sizeX, sizeY, pixelFormat, clearColor, isResizable)))
	{
		LOG_CRITICAL(L"Failed to Create Render Target");
		return nullptr;
	}

	return renderTarget;
}

#ifdef _DEBUG
HRESULT RenderTarget::Ready_Debug(Float x, Float y, Float sizeX, Float sizeY)
{
	uint32			numViewports = { 1 };
	D3D11_VIEWPORT	viewportDesc{};
	m_Context->RSGetViewports(&numViewports, &viewportDesc);

	m_WorldMatrix = Matrix::Identity;

	m_WorldMatrix._11 = sizeX;
	m_WorldMatrix._22 = sizeY;

	m_WorldMatrix._41 = x - viewportDesc.Width * 0.5f;
	m_WorldMatrix._42 = -y + viewportDesc.Height * 0.5f;

	return S_OK;
}

HRESULT RenderTarget::Render_Debug(const Shared<VIBuffer_Rect>& buffer, const Shared<Shader>& shader) const
{
	shader->Bind_Matrix(WorldMatrix, &m_WorldMatrix);
	shader->Bind_SRV(DefaultMap, m_ShaderResourceView);
	shader->Begin(0);
	buffer->Bind_Resources();
	buffer->Render();

	shader->Bind_SRV(DefaultMap, nullptr);
	shader->Begin(0);

	return S_OK;
}
#endif