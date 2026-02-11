#include "TagRegistry.h"
#include "Engine_Define.h"
#include "String_Helper.h"

wstring TagRegistry::Get_TagName(TAG tag) const
{
	const auto iter = m_TagToName.find(tag);
	if (iter != m_TagToName.end())
		return iter->second;

	return Helper::To_wString(magic_enum::enum_name(tag));
}

wstring TagRegistry::Get_TagName(uint32 tagBit) const
{
	return Get_TagName(static_cast<TAG>(tagBit));
}

TAG TagRegistry::Get_TagByName(const wstring& name) const
{
	auto it = m_NameToTag.find(name);
	if (it != m_NameToTag.end())
		return it->second;

	return TAG::TAG_0;
}

void TagRegistry::Set_TagName(TAG tag, const wstring& name)
{
	auto oldName = m_TagToName.find(tag);
	if (oldName != m_TagToName.end())
		m_NameToTag.erase(oldName->second);

	m_TagToName[tag] = name;
	m_NameToTag[name] = tag;
}

void TagRegistry::Set_TagName(uint32 tagBit, const wstring& name)
{
	Set_TagName(static_cast<TAG>(tagBit), name);
}

void TagRegistry::LoadFromFile(const wstring& path)
{
	Clear();
	for (size_t i = 0; i < ETOI(TAG::END); ++i)
	{
		TAG tag = static_cast<TAG>(1 << i);
		wstring defaultName = Helper::To_wString(magic_enum::enum_name(tag));
		m_NameToTag.emplace(defaultName, tag);
		m_TagToName.emplace(tag, defaultName);
	}

	// TODO: JSON 파싱 구현
	// nlohmann/json 사용 예정
}

void TagRegistry::SaveToFile(const wstring& path) const
{
	// TODO: JSON 저장 구현
}

void TagRegistry::Clear()
{
	m_NameToTag.clear();
	m_TagToName.clear();
}