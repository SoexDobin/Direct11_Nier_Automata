#include "Renderer.h"

Renderer::Renderer(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
}

HRESULT Renderer::Initialize(Shared<void> arg)
{
	return EngineManager::Initialize(arg);
}

void Renderer::On_Destroy()
{
	
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
