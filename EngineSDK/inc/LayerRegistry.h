#pragma once
#include  "Engine_Define.h"

NS_BEGIN(Engine)

typedef struct ENGINE_DLL LayerMask final
{
	LayerMask() = default;

	uint32 Get_Layer() const { return m_Layer; }
	void Set_Layer(const LAYER layer) { m_Layer = ETOI(layer); }
	uint32 Get_Mask() const { return m_Mask; }
	void Set_Mask(const LAYER mask) { m_Mask = ETOI(mask); }

	void Add(LAYER layer) { m_Mask |= ETOI(layer); }
	void Add(uint32 layer) { m_Mask |= layer; }
	void Remove(LAYER layer) { m_Mask &= ~ETOI(layer); }
	void Remove(uint32 layer) { m_Mask &= ~layer; }
	Bool Has(LAYER layer) const { return (m_Mask & ETOI(layer)) != 0; }
	Bool Has(uint32 layer) const { return (m_Mask & layer) != 0; }

private:
	uint32 m_Layer = { 0x00000000 };
	uint32 m_Mask = { 0xFFFFFFFF };
} LAYER_MASK;

class ENGINE_DLL LayerRegistry
{
	DECLARE_SINGLETON(LayerRegistry)
private:
	LayerRegistry() = default;
	~LayerRegistry() = default;

public:
	wstring Get_LayerName(LAYER layer) const;
	wstring Get_LayerName(uint32 layerBit) const;
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