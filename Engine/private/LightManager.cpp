#include "LightManager.h"
#include "Light.h"

LightManager::LightManager()
{
}

const LIGHT_DESC* LightManager::Get_LightDesc(uint32 index) const
{
	if (index >= m_Lights.size())
		return nullptr;

	return &m_Lights[index]->Get_LightDesc();
}

HRESULT LightManager::Add_Light(const LIGHT_DESC& desc)
{
	Shared<Light> light = Light::Create(desc);
	if (!light)
	{
		MSG_BOX("Failed to create Light");
		return E_FAIL;
	}

	m_Lights.push_back(light);

	return S_OK;
}

HRESULT LightManager::Remove_Light(uint32 index)
{
	if (index >= m_Lights.size())
		return E_FAIL;

	swap(m_Lights[index], m_Lights.back());
	m_Lights.pop_back();

	return S_OK;
}

HRESULT LightManager::Add_Light(const Shared<Light>& light)
{
	if (!light)
		return E_FAIL;
	m_Lights.push_back(light);
	return S_OK;
}

HRESULT LightManager::Remove_Light(const Shared<Light>& light)
{
	if (!light)
		return E_FAIL;
	
	std::erase_if(m_Lights, [&light](const Shared<Light>& pListLight) {
		return pListLight == light;
		});

	return S_OK;
}

HRESULT LightManager::Clear_Lights()
{
	m_Lights.shrink_to_fit(); 
	m_Lights.clear();
	return S_OK; 
}

HRESULT LightManager::Render_Lights(const Shared<Shader>& shader, const Shared<VIBuffer_Rect>& buffer)
{
	for (auto& light : m_Lights)
	{
		if (light)
			light->Render(shader, buffer);
	}

	return S_OK;
}

HRESULT LightManager::Initialize_Prototype() { return EngineManager::Initialize_Prototype(); }
HRESULT LightManager::Initialize(void* arg) { return EngineManager::Initialize(arg); }
void LightManager::On_Destroy()
{
	EngineManager::On_Destroy();
	m_Lights.clear();
}

Unique<LightManager> LightManager::Create()
{
	auto lightManager = make_unique<LightManager>();

	if (FAILED(lightManager->Initialize_Prototype()))
	{
		MSG_BOX("Failed to create LightManager");
		return nullptr;
	}
	if (FAILED(lightManager->Initialize(nullptr)))
	{
		MSG_BOX("Failed to create LightManager");
		return nullptr;
	}

	return lightManager;
}
