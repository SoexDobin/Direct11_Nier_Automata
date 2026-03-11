#include "ResourceManager.h"
#include <tchar.h>
#include "SpdLogger.h"
#include "Game.h"

ResourceManager::ResourceManager(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) 
    : m_Device{ device }, m_Context{ context } {}

ResourceManager::~ResourceManager() {}

HRESULT ResourceManager::Initialize_Prototype(uint32 levCount)
{
	std::lock_guard<std::recursive_mutex> lock(m_ResourceMutex);

    m_LevelCount = levCount;
    m_Shaders.resize(m_LevelCount);
    m_TextureDescTags.resize(m_LevelCount);
    m_SRVs.resize(m_LevelCount);

	return EngineManager::Initialize_Prototype();
}

void ResourceManager::On_Destroy()
{
    for (auto& shaders : m_Shaders)
        shaders.clear();
    m_Shaders.clear();

    for (auto& textureDescs : m_TextureDescTags)
        textureDescs.clear();
    m_TextureDescTags.clear();
    for (auto& SRVs : m_SRVs)
        SRVs.clear();
    m_SRVs.clear();

	EngineManager::On_Destroy();
}

HRESULT ResourceManager::Load_Texture(uint32 levIndex, const tChar* texturePath, uint32 numSRVs, const wstring& descriptionTag)
{
    Texture::TEXTURE_DESC desc{};
    desc.m_levIndex = levIndex;
    desc.m_FilePath = texturePath;
    desc.m_NumSRVs = numSRVs;
    desc.m_TextureTag = descriptionTag;

    m_TextureDescTags[levIndex].emplace(descriptionTag, desc);

    for (uint32 i = 0; i < numSRVs; ++i) {
    	tChar szFullPath[MAX_PATH] = TEXT("");
        wsprintf(szFullPath, texturePath, i);

        if (m_SRVs[levIndex].contains(szFullPath))
            continue;

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

        m_SRVs[levIndex].emplace(texturePath, srv);
    }

    return S_OK;
}

const Texture::TEXTURE_DESC* ResourceManager::Get_TextureDescByTag(uint32 levIndex, const wstring& descriptionTag)
{
    if (m_TextureDescTags[0].contains(descriptionTag))
        return &m_TextureDescTags[0][descriptionTag];

    if (false == m_TextureDescTags[levIndex].contains(descriptionTag))
    {
        LOG_ERROR(L" \"{}\" has no Texture Description", descriptionTag);
        return nullptr;
    }

    return &m_TextureDescTags[levIndex][descriptionTag];
}


const ComPtr<ID3D11ShaderResourceView>& ResourceManager::Get_Texture(uint32 levIndex, const tChar* texturePath)
{
    if (m_SRVs[0].contains(texturePath))
        return m_SRVs[0][texturePath];

    if (m_SRVs[levIndex].contains(texturePath) == false)
    {
        LOG_ERROR(L"{} is not texture path", texturePath);
    }

    return m_SRVs[levIndex][texturePath];
}

HRESULT ResourceManager::Load_Shader(uint32 levIndex, const tChar* shaderPath, const D3D11_INPUT_ELEMENT_DESC* elements, uint32 numElements, const wstring& descriptionTag)
{
    auto shader = Shader::Create(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context(), shaderPath, elements, numElements);
    if (nullptr == shader)
    {
        LOG_ERROR(L"Failed to Create Shader Resource. Path : {}", shaderPath);
    }

    m_Shaders[levIndex].emplace(descriptionTag, shader);
    Shader::SHADER_DESC desc{};
    desc.m_VertexTag = shaderPath;
    desc.m_elementsDesc = elements;
    desc.m_numPasses = numElements;

    return S_OK;
}

Shared<Shader> ResourceManager::Get_Shader(uint32 levIndex, const tChar* vertexTag)
{
    if (m_Shaders[levIndex].contains(vertexTag) == false)
    {
        LOG_ERROR(L"{} is not shader path", vertexTag);
    }

    return m_Shaders[levIndex][vertexTag];
}

HRESULT ResourceManager::Clear_AllResources() {
    m_Shaders.clear();
    m_TextureDescTags.clear();
    m_SRVs.clear();

    m_Shaders.resize(m_LevelCount);
    m_TextureDescTags.resize(m_LevelCount);
    m_SRVs.resize(m_LevelCount);

	return S_OK;
}

HRESULT ResourceManager::Clear_Resource(uint32 levIndex)
{
    m_Shaders[levIndex].clear();
    m_TextureDescTags[levIndex].clear();
    m_SRVs[levIndex].clear();

    return S_OK;
}

Unique<ResourceManager> ResourceManager::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, uint32 levCount)
{
	auto resourceManager = make_unique<ResourceManager>(device, context);

	if (FAILED(resourceManager->Initialize_Prototype(levCount)))
	{
		MSG_BOX("Failed to Create ResourceManager");
		return nullptr;
	}

	return resourceManager;
}

