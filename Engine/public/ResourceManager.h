#pragma once
#include "EngineManager.h"

NS_BEGIN(Engine)

class ResourceManager final : public EngineManager
{
	RTTR_ENABLE(EngineManager)
public:
	ResourceManager();
	~ResourceManager() override;

public:
	HRESULT Initialize_Prototype() override ;
	HRESULT Initialize(void* arg) override { return S_OK; };
	void On_Destroy() override {};
	void On_Disable() override {};
	void On_Enable() override {};
	void Set_Active(Bool isActive) override {};

public:
	void Load_Texture(const tChar* texturePath, uint32  numSRVs);
	const vector<ComPtr<ID3D11ShaderResourceView>>& Get_Textures();

public:
	HRESULT Add_ResourceTypeID(const wstring& resourcePath, uint32 typeID);
	uint32 Get_ResourceTypeID(const wstring& res);
	HRESULT Clear_Resources();

private:
	mutable std::recursive_mutex m_ResourceMutex;
	unordered_map<wstring, uint32>	m_ResourcePrototypes;
	
	unordered_map<uint32, vector<ComPtr<ID3D11ShaderResourceView>>> m_SRVs;
	
public:
	static Unique<ResourceManager> Create();
};

NS_END