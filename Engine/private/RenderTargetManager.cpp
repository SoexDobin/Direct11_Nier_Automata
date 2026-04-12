#include "RenderTargetManager.h"
#include "RenderTarget.h"

RenderTargetManager::RenderTargetManager(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: m_Device{device}, m_Context{context} {}

RenderTargetManager::~RenderTargetManager()
{
	m_RenderTargets.clear();

	for (auto& rtList : m_MultiRenderTargets) {
		rtList.second.clear();
	}
	m_MultiRenderTargets.clear();
}

HRESULT RenderTargetManager::Add_RenderTarget(const wstring& rtTag, uint32 sizeX, uint32 sizeY, DXGI_FORMAT pixelFormat, const Color& clearColor)
{
	if (nullptr != Find_RenderTarget(rtTag))
		return E_FAIL;

	auto renderTarget = RenderTarget::Create(m_Device, m_Context, sizeX, sizeY, pixelFormat, clearColor);
	if (nullptr == renderTarget)
		return E_FAIL;

	m_RenderTargets.emplace(rtTag, renderTarget);

	return S_OK;
}

HRESULT RenderTargetManager::Bind_ShaderResource(const Shared<Shader>& shader, const Char* constantName, const wstring& rtTag)
{
	auto renderTarget = Find_RenderTarget(rtTag);
	if (nullptr == renderTarget)
		return E_FAIL;

	return renderTarget->Bind_ShaderResource(shader, constantName);
}

Shared<RenderTarget> RenderTargetManager::Find_RenderTarget(const wstring& rtTag)
{
	if (m_RenderTargets.contains(rtTag))
		return m_RenderTargets[rtTag];

	return nullptr;
}

Unique<RenderTargetManager> RenderTargetManager::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto instance = make_unique<RenderTargetManager>(device, context);

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : RenderTargetManager");
		return nullptr;
	}

	return instance;
}