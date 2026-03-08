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
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override { return S_OK; }
	void On_Destroy() override;
	void On_Disable() override {};
	void On_Enable() override {};
	void Set_Active(Bool isActive) override {};

public:
	HRESULT Load_Texture(const tChar* texturePath, uint32 numSRVs, const wstring& descriptionTag);
	const Texture::TEXTURE_DESC& Get_TextureDescByTag(const wstring& descriptionTag);
	const ComPtr<ID3D11ShaderResourceView>& Get_Texture(const tChar* texturePath);
	const vector<ComPtr<ID3D11ShaderResourceView>> Get_Textures(const tChar* texturePath, uint32 numSRVs);

public:
	HRESULT Load_Shader(const tChar* texturePath, const D3D11_INPUT_ELEMENT_DESC* elements, uint32 numElements, const wstring& descriptionTag);
	Shared<Shader> Get_Shader(const tChar* vertexTag);

public:
	HRESULT Clear_Resources();

private:
	ComPtr<ID3D11Device> m_Device{nullptr};
	ComPtr<ID3D11DeviceContext> m_Context{ nullptr };

	unordered_map<wstring, Shared<Shader>> m_Shaders;

	unordered_map<wstring, Texture::TEXTURE_DESC> m_TextureDescTags;
	unordered_map<wstring, ComPtr<ID3D11ShaderResourceView>> m_SRVs;
	mutable std::recursive_mutex m_ResourceMutex;
	
public:
	static Unique<ResourceManager> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
};

NS_END
