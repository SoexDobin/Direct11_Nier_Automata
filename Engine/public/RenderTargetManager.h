#pragma once
#include "EngineManager.h"

NS_BEGIN(Engine)

class Shader;
class RenderTarget;

class RenderTargetManager final : public EngineManager
{
	NO_COPY(RenderTargetManager)
public:
	explicit RenderTargetManager() = default;
	explicit RenderTargetManager(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	~RenderTargetManager() override ;

public:
	HRESULT Add_RenderTarget(const wstring& rtTag, uint32 sizeX, uint32 sizeY, DXGI_FORMAT pixelFormat, const Color& clearColor);
	HRESULT Bind_ShaderResource(const Shared<Shader>& shader, const Char* constantName, const wstring& rtTag);

private:
	ComPtr<ID3D11Device> m_Device = { nullptr };
	ComPtr<ID3D11DeviceContext> m_Context = { nullptr };

private:
	map<const wstring, Shared<RenderTarget>>			m_RenderTargets;
	map<const wstring, list<Shared<RenderTarget>>>		m_MultiRenderTargets;

private:
	Shared<RenderTarget> Find_RenderTarget(const wstring& rtTag);

public:
	static Unique<RenderTargetManager> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);

};

NS_END