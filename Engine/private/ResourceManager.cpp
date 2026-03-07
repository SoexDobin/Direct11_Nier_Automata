#include "ResourceManager.h"
#include <tchar.h>
#include "SpdLogger.h"

ResourceManager::ResourceManager(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) 
    : m_Device{ device }, m_Context{ context } {}

ResourceManager::~ResourceManager() {}

HRESULT ResourceManager::Initialize_Prototype()
{
	std::lock_guard<std::recursive_mutex> lock(m_ResourceMutex);

	return EngineManager::Initialize_Prototype();
}

HRESULT ResourceManager::Load_Texture(const tChar* texturePath, uint32 numSRVs)
{
    for (uint32 i = 0; i < numSRVs; ++i) {
        tChar szFullPath[MAX_PATH] = TEXT("");
        wsprintf(szFullPath, texturePath, i);

        tChar szDrive[MAX_PATH] = TEXT("");
        tChar szDir[MAX_PATH] = TEXT("");
        tChar szName[MAX_PATH] = TEXT("");
        tChar szExt[MAX_PATH] = TEXT("");

        _tsplitpath_s(szFullPath, szDrive, MAX_PATH, szDir, MAX_PATH, szName,
            MAX_PATH, szExt, MAX_PATH);

        HRESULT hr = {};
        ComPtr<ID3D11Resource> texture{ nullptr };
        ComPtr<ID3D11ShaderResourceView> srv{ nullptr };

        if (!lstrcmp(szExt, TEXT(".dds"))) {
            hr = CreateDDSTextureFromFile(m_Device.Get(), szFullPath,
                texture.GetAddressOf(),
                srv.GetAddressOf());
        }
        else if (!lstrcmp(szExt, TEXT(".tga"))) {
            MSG_BOX("TGA Texture Loading Not Supported Yet");
            return E_FAIL;
        }
        else {
            hr = CreateWICTextureFromFile(m_Device.Get(), szFullPath,
                texture.GetAddressOf(),
                srv.GetAddressOf());
        }

        if (FAILED(hr)) { return E_FAIL; }

        m_SRVs.emplace(texturePath, srv);
    }

    return S_OK;
}

const ComPtr<ID3D11ShaderResourceView>& ResourceManager::Get_Texture(const tChar* texturePath)
{
    if (m_SRVs.contains(texturePath) == false)
    {
        LOG_ERROR(L"{} is not texture path", texturePath);
    }

    return m_SRVs[texturePath];
}

const vector<ComPtr<ID3D11ShaderResourceView>>& ResourceManager::Get_Textures(const tChar* texturePath, uint32 numSRVs)
{
    std::vector<ComPtr<ID3D11ShaderResourceView>> textures;
    textures.reserve(numSRVs); 

    for (uint32 i = 0; i < numSRVs; ++i)
    {
        TCHAR fullPath[MAX_PATH];

        _stprintf_s(fullPath, MAX_PATH, TEXT("%s_%u.dds"), texturePath, i);

        ComPtr<ID3D11Resource> texture{ nullptr };
        ComPtr<ID3D11ShaderResourceView> srv{ nullptr };

        HRESULT hr = CreateDDSTextureFromFile(
            m_Device.Get(), fullPath, 
            texture.GetAddressOf(), 
            srv.GetAddressOf());

        if (SUCCEEDED(hr))
        {
            textures.push_back(srv);
        }
        else {
            hr = CreateWICTextureFromFile(
                m_Device.Get(), texturePath,
                texture.GetAddressOf(),
                srv.GetAddressOf());
        }
        
        if (FAILED(hr))
        {
            LOG_ERROR(L"Failed to find Textures Path : {}, numSRVs ", fullPath, numSRVs);
            MSG_BOX("Failed to find Textures");
            return EMPTY_VECTOR<ComPtr<ID3D11ShaderResourceView>>;
        }
    }

    return textures;
}

HRESULT ResourceManager::Clear_Resources() {
	std::lock_guard<std::recursive_mutex> lock(m_ResourceMutex);
    m_SRVs.clear();
	return S_OK;
}

Unique<ResourceManager> ResourceManager::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto resourceManager = make_unique<ResourceManager>(device, context);

	if (FAILED(resourceManager->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create ResourceManager");
		return nullptr;
	}

	return resourceManager;
}
