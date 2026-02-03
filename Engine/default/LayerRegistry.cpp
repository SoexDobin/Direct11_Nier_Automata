#include "LayerRegistry.h"
#include "Engine_Define.h"
#include "String_Helper.h"

IMPLEMENT_SINGLETON(LayerRegistry)

const wstring& LayerRegistry::Get_LayerName(LAYER layer) const
{
	const auto iter = m_LayerToName.find(layer);

	if (iter != m_LayerToName.end())
		return iter->second;

	return Helper::To_wString(magic_enum::enum_name(layer));
}
const wstring& LayerRegistry::Get_LayerName(uint32 layerBit) const
{
	return Get_LayerName(static_cast<LAYER>(layerBit));
}

LAYER LayerRegistry::Get_LayerByName(const wstring& name) const
{
	auto it = m_NameToLayer.find(name);
	if (it != m_NameToLayer.end())
		return it->second;

	return LAYER::LAYER0;
}
// 레이어 이름 설정 (중복 처리)
void LayerRegistry::Set_LayerName(LAYER layer, const wstring& name)
{
	// 기존 이름 제거 (있으면)
	auto oldName = m_LayerToName.find(layer);
	if (oldName != m_LayerToName.end())
		m_NameToLayer.erase(oldName->second);

	// 새 이름 설정
	m_LayerToName[layer] = name;
	m_NameToLayer[name] = layer;
}

void LayerRegistry::Set_LayerName(uint32 layerBit, const wstring& name)
{
	Set_LayerName(static_cast<LAYER>(layerBit), name);
}

void LayerRegistry::LoadFromFile(const wstring& path)
{
	Clear();

	m_NameToLayer[Helper::To_wString(magic_enum::enum_name(LAYER::LAYER0))] = LAYER::LAYER0;
	m_LayerToName[LAYER::LAYER0] = Helper::To_wString(magic_enum::enum_name(LAYER::LAYER0));
	for (size_t i = 0; i < ETOI(LAYER::END); ++i)
	{
		LAYER layer = static_cast<LAYER>(1 << i);
		wstring defaultName = Helper::To_wString(magic_enum::enum_name(layer));

		m_NameToLayer.emplace(defaultName, layer);
		m_LayerToName.emplace(layer, defaultName);
	}

	// TODO: JSON 파싱 구현
	// nlohmann/json 사용 예정
}

void LayerRegistry::SaveToFile(const wstring& path) const
{
	// TODO: JSON 저장 구현
}

void LayerRegistry::Clear()
{
	m_NameToLayer.clear();
	m_LayerToName.clear();
}