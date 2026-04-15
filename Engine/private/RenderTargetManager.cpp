#include "RenderTargetManager.h"

#include "Game.h"
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

HRESULT RenderTargetManager::Add_MultiRenderTarget(const wstring& mrtTag, const wstring& rtTag)
{
	auto renderTarget = Find_RenderTarget(rtTag);
	if (!renderTarget)
		return E_FAIL;

	auto mrtList = Find_MultiRenderTarget(mrtTag);
	
	if (!mrtList)
	{
		list<Shared<RenderTarget>> newList;
		newList.push_back(renderTarget);
		m_MultiRenderTargets.emplace(mrtTag, newList);
	}
	else
	{
		mrtList->push_back(renderTarget);
	}

	return S_OK;
}

HRESULT RenderTargetManager::Begin_MultiRenderTarget(const wstring& mrtTag)
{
	auto mrtList = Find_MultiRenderTarget(mrtTag);
	if (!mrtList)
		return E_FAIL;

	m_Context->OMGetRenderTargets(1, m_BackBuffer.GetAddressOf(), m_OriginalDepthStencil.GetAddressOf());

	ID3D11RenderTargetView* renderTargets[8] = { nullptr };
	uint32 numRenderTargets{ 0 };

	for (auto& renderTarget : *mrtList)
	{
		renderTarget->Clear_RenderTarget();
		renderTargets[numRenderTargets++] = renderTarget->Get_RenderTargetView().Get();
	}

	m_Context->OMSetRenderTargets(numRenderTargets, renderTargets, m_OriginalDepthStencil.Get());

	return S_OK;
}

HRESULT RenderTargetManager::End_MultiRenderTarget()
{
	m_Context->OMSetRenderTargets(1, m_BackBuffer.GetAddressOf(), m_OriginalDepthStencil.Get());

	m_BackBuffer.Reset();
	m_OriginalDepthStencil.Reset();

	return S_OK;
}

list<Shared<RenderTarget>>* RenderTargetManager::Find_MultiRenderTarget(const wstring& mrtTag)
{
	if (m_MultiRenderTargets.contains(mrtTag))
	{
		return &m_MultiRenderTargets[mrtTag];
	}

	return nullptr;
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

#ifdef _DEBUG
HRESULT RenderTargetManager::Ready_RenderTarget_Debug(const wstring& rtTag, Float x, Float y, Float sizeX, Float sizeY)
{
	Shared<RenderTarget> renderTarget = Find_RenderTarget(rtTag);
	if (nullptr == renderTarget)
		return E_FAIL;

	return renderTarget->Ready_Debug(x, y, sizeX, sizeY);
}

HRESULT RenderTargetManager::Render_RenderTarget_Debug(const Shared<VIBuffer_Rect>& buffer, const Shared<Shader>& shader, const wstring& mrtTag)
{
	list<Shared<RenderTarget>>* mrtList = Find_MultiRenderTarget(mrtTag);
	if (nullptr == mrtList)
		return E_FAIL;

	for (auto& renderTarget : *mrtList)
	{
		if (FAILED(renderTarget->Render_Debug(buffer, shader)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}
#endif
