#pragma once
#include "EngineManager.h"
#include "Texture.h"
#include "Shader.h"

NS_BEGIN(Engine)

class ResourceManager final : public EngineManager
{
	RTTR_ENABLE(EngineManager)
public:
	ResourceManager(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	~ResourceManager() override;

public:
	HRESULT Initialize_Prototype(uint32 levCount) ;
	HRESULT Initialize(void* arg) override { return S_OK; }
	void On_Destroy() override;
	void On_Disable() override {};
	void On_Enable() override {};
	void Set_Active(Bool isActive) override {};

public:
	HRESULT Load_Texture(uint32 levIndex, const tChar* texturePath, uint32 numSRVs, const wstring& descriptionTag);
	const Texture::TEXTURE_DESC* Get_TextureDescByTag(uint32 levIndex, const wstring& descriptionTag);
	const ComPtr<ID3D11ShaderResourceView>& Get_Texture(uint32 levIndex, const tChar* texturePath);

public:
	HRESULT Load_Shader(uint32 levIndex, const tChar* texturePath, const D3D11_INPUT_ELEMENT_DESC* elements, uint32 numElements, const wstring& descriptionTag);
	Shared<Shader> Get_Shader(uint32 levIndex, const tChar* vertexTag);

public:
	HRESULT Clear_AllResources();
	HRESULT Clear_Resource(uint32 levIndex);

private:
	uint32 m_LevelCount{};
	ComPtr<ID3D11Device> m_Device{nullptr};
	ComPtr<ID3D11DeviceContext> m_Context{ nullptr };

	vector<unordered_map<wstring, Shared<Shader>>> m_Shaders;
	vector<unordered_map<wstring, Texture::TEXTURE_DESC>> m_TextureDescTags;
	vector<unordered_map<wstring, ComPtr<ID3D11ShaderResourceView>>> m_SRVs;
	mutable std::recursive_mutex m_ResourceMutex;
	
public:
	static Unique<ResourceManager> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, uint32 levCount);
};

NS_END
