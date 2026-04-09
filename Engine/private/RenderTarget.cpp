#include "RenderTarget.h"

#include "SpdLogger.h"

RenderTarget::RenderTarget(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: m_Device{device}, m_Context{context}
{
}

HRESULT RenderTarget::Initialize(uint32 sizeX, uint32 sizeY, DXGI_FORMAT pixelFormat, const Color& clearColor)
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

	return S_OK;
}

HRESULT RenderTarget::Begin()
{
	return Object::Begin();
}

HRESULT RenderTarget::Bind_ShaderResource(const Shared<Shader>& shader, const Char* constantName) const
{
	return Bind_ShaderResource(shader, constantName);
}

void RenderTarget::Clear_RenderTarget() const
{
	m_Context->ClearRenderTargetView(m_RenderTargetView.Get(), m_ClearColor);
}

Shared<RenderTarget> RenderTarget::Create(const ComPtr<ID3D11Device>& device,
	const ComPtr<ID3D11DeviceContext>& context, uint32 sizeX, uint32 sizeY, DXGI_FORMAT pixelFormat,
	const Color& clearColor)
{
	auto renderTarget = make_shared<RenderTarget>(device, context);

	if (FAILED(renderTarget->Initialize(sizeX, sizeY, pixelFormat, clearColor)))
	{
		LOG_CRITICAL(L"Failed to Create Render Target");
		return nullptr;
	}

	return renderTarget;
}
