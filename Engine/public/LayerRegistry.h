#pragma once
#include  "Engine_Define.h"

NS_BEGIN(Engine)

class ENGINE_DLL LayerRegistry
{
	DECLARE_SINGLETON(LayerRegistry)
private:
	LayerRegistry() = default;
	~LayerRegistry() = default;

public:
	const wstring& Get_LayerName(LAYER layer) const;
	const wstring& Get_LayerName(uint32 layerBit) const;
	void Set_LayerName(LAYER layer, const wstring& name);
	void Set_LayerName(uint32 layerBit, const wstring& name);
	
	LAYER Get_LayerByName(const wstring& name) const;
	const map<LAYER, wstring>& Get_AllLayers() const { return m_LayerToName; }

public:
	void LoadFromFile(const wstring& path);
	void SaveToFile(const wstring& path) const;
	void Clear();

private:
	map<wstring, LAYER>	m_NameToLayer;
	map<LAYER, wstring>	m_LayerToName;
};

NS_END