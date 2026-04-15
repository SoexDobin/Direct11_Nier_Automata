#pragma once
#include "EngineManager.h"

NS_BEGIN(Engine)
	class VIBuffer_Rect;

	class Shader;
class RenderTarget;

class RenderTargetManager final : public EngineManager
{
	NO_COPY(RenderTargetManager)
public:
	explicit RenderTargetManager() = default;
	explicit RenderTargetManager(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	~RenderTargetManager() override;

public:
	HRESULT Add_RenderTarget(const wstring& rtTag, uint32 sizeX, uint32 sizeY, DXGI_FORMAT pixelFormat, const Color& clearColor);
	HRESULT Bind_ShaderResource(const Shared<Shader>& shader, const Char* constantName, const wstring& rtTag);

public:
	HRESULT Add_MultiRenderTarget(const wstring& mrtTag, const wstring& rtTag);
	HRESULT Begin_MultiRenderTarget(const wstring& mrtTag);
	HRESULT End_MultiRenderTarget();

private:
	ComPtr<ID3D11Device> m_Device = { nullptr };
	ComPtr<ID3D11DeviceContext> m_Context = { nullptr };

	map<const wstring, Shared<RenderTarget>>			m_RenderTargets;
	map<const wstring, list<Shared<RenderTarget>>>		m_MultiRenderTargets;

	ComPtr<ID3D11RenderTargetView> m_BackBuffer{ nullptr };
	ComPtr<ID3D11DepthStencilView> m_OriginalDepthStencil{ nullptr };

private:
	Shared<RenderTarget> Find_RenderTarget(const wstring& rtTag);
	list<Shared<RenderTarget>>* Find_MultiRenderTarget(const wstring& mrtTag);

public:
	static Unique<RenderTargetManager> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);

#ifdef _DEBUG
	HRESULT Ready_RenderTarget_Debug(const wstring& rtTag, Float x, Float y, Float sizeX, Float sizeY);
	HRESULT Render_RenderTarget_Debug(const Shared<VIBuffer_Rect>& buffer, const Shared<Shader>& shader, const wstring& mrtTag);
#endif
};

NS_END