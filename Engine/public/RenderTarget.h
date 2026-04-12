#pragma once
#include "Object.h"

NS_BEGIN(Engine)

class Shader;

class RenderTarget final : public Object
{
public:
	explicit RenderTarget(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	~RenderTarget() override = default;

public:
	HRESULT Initialize(uint32 sizeX, uint32 sizeY, DXGI_FORMAT pixelFormat, const Color& clearColor);
	HRESULT Begin() override;
	PROTOTYPE Get_Prototype() const override { return PROTOTYPE::OBJECT; };

public:
	ComPtr<ID3D11RenderTargetView> Get_RenderTargetView() const { return m_RenderTargetView; }
	HRESULT Bind_ShaderResource(const Shared<Shader>& shader, const Char* constantName) const;
	void Clear_RenderTarget() const;

private:
	Color m_ClearColor{};
	ComPtr<ID3D11Device> m_Device{ nullptr };
	ComPtr<ID3D11DeviceContext> m_Context{ nullptr };
	ComPtr<ID3D11Texture2D> m_Texture2D{ nullptr };
	ComPtr<ID3D11RenderTargetView> m_RenderTargetView{ nullptr };
	ComPtr<ID3D11ShaderResourceView> m_ShaderResourceView{ nullptr };

public:
	static Shared<RenderTarget> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, 
		uint32 sizeX, uint32 sizeY, DXGI_FORMAT pixelFormat, const Color& clearColor);
	
};

NS_END