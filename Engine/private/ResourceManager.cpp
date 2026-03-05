#include "ResourceManager.h"

#include "SpdLogger.h"

ResourceManager::ResourceManager() {}

ResourceManager::~ResourceManager() {}

HRESULT ResourceManager::Initialize_Prototype()
{
	std::lock_guard<std::recursive_mutex> lock(m_ResourceMutex);

	return EngineManager::Initialize_Prototype();
}

void ResourceManager::Load_Texture(const tChar* texturePath, uint32 numSRVs)
{
}

const vector<ComPtr<ID3D11ShaderResourceView>>& ResourceManager::Get_Textures()
{
}

HRESULT ResourceManager::Add_ResourceTypeID(const wstring& resourcePath, uint32 typeID)
{
	std::lock_guard<std::recursive_mutex> lock(m_ResourceMutex);

	if (m_ResourcePrototypes.contains(resourcePath))
	{
		LOG_ERROR(L"{} is Already Added ResourceTypeID", resourcePath);
		MSG_BOX("Already Added ResourceTypeID");
		return E_FAIL;
	}

	m_ResourcePrototypes.emplace(resourcePath, typeID);

	return S_OK;
}

uint32 ResourceManager::Get_ResourceTypeID(const wstring& resourcePath) {
	std::lock_guard<std::recursive_mutex> lock(m_ResourceMutex);

	if (m_ResourcePrototypes.contains(resourcePath) == false)
	{
		LOG_ERROR(L"{} is Not Contain ResourceTypeID", resourcePath);
		MSG_BOX("Not Contain ResourceTypeID");
		return E_FAIL;
	}

	return m_ResourcePrototypes[resourcePath];
}

HRESULT ResourceManager::Clear_Resources() {
	std::lock_guard<std::recursive_mutex> lock(m_ResourceMutex);
	m_ResourcePrototypes.clear();
	return S_OK;
}

Unique<ResourceManager> ResourceManager::Create()
{
	auto resourceManager = make_unique<ResourceManager>();

	if (FAILED(resourceManager->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create ResourceManager");
		return nullptr;
	}

	return resourceManager;
}
