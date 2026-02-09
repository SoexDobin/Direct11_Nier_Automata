#include "Renderer.h"

#include "GameObject.h"

Renderer::Renderer(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: m_Device(device), m_Context(context)
{
}

void Renderer::Add_RenderGroup(RENDERGROUP renderGroup, const Shared<GameObject>& gameObject)
{
	m_RenderGroup[ETOI(renderGroup)].push_back(gameObject);
}

void Renderer::Draw()
{
	for (uint32 i = 0; i < ETOI(RENDERGROUP::END); ++i)
	{
		Render_Group(i);
	}
}

HRESULT Renderer::Initialize(const Shared<void>& arg)
{
	m_LayerMask = ETOI(LAYER::ALL_LAYER);

	return EngineManager::Initialize(arg);
}

void Renderer::On_Destroy()
{
	for (auto group : m_RenderGroup)
		group.clear();

}

void Renderer::On_Disable()
{
	EngineManager::On_Disable();
}

void Renderer::On_Enable()
{
	EngineManager::On_Enable();
}

void Renderer::Set_Active(Bool isActive)
{
	EngineManager::Set_Active(isActive);
}

void Renderer::Render_Group(uint32 groupIndex)
{
	for (auto& object : m_RenderGroup[groupIndex])
	{
		uint32 objLayer = object->Get_LayerMask().Get_Layer();
		
		if ((m_LayerMask & objLayer) == 0) continue;
		
		object->Render();
	}

	m_RenderGroup[groupIndex].clear();
}

Unique<Renderer> Renderer::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto renderer = make_unique<Renderer>(device, context);

	if (FAILED(renderer->Initialize(nullptr)))
	{
		MSG_BOX("Failed To Create Renderer");
		return nullptr;
	}

	return renderer;
}
