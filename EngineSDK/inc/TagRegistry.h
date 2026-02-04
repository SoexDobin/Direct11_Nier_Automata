#pragma once
#include "Engine_Define.h"
NS_BEGIN(Engine)

typedef struct ENGINE_DLL TagMask final
{
	TagMask() = default;

public:
	uint32 Get_Tags() const { return m_Tag; }
	void Set_Tags(uint32 tags) { m_Tag = tags; }

	void Add(TAG tag) { m_Tag |= ETOI(tag); }
	void Add(uint32 tag) { m_Tag |= tag; }
	void Remove(TAG tag) { m_Tag &= ~ETOI(tag); }
	void Remove(uint32 tag) { m_Tag &= ~tag; }
	Bool Has(TAG tag) const { return (m_Tag & ETOI(tag)) != 0; }
	Bool Has(uint32 tag) const { return (m_Tag & tag) != 0; }
	Bool HasAny(uint32 tags) const { return (m_Tag & tags) != 0; }
	Bool HasAll(uint32 tags) const { return (m_Tag & tags) == tags; }
	void Clear() { m_Tag = 0; }

private:
	uint32 m_Tag = { 0x00000000 };

} TAG_MASK;

class ENGINE_DLL TagRegistry
{
	DECLARE_SINGLETON(TagRegistry)

private:
	TagRegistry() = default;
	~TagRegistry() = default;

public:
	wstring Get_TagName(TAG tag) const;
	wstring Get_TagName(uint32 tagBit) const;

	void Set_TagName(TAG tag, const wstring& name);
	void Set_TagName(uint32 tagBit, const wstring& name);

	TAG Get_TagByName(const wstring& name) const;

	const map<TAG, wstring>& Get_AllTags() const { return m_TagToName; }
public:
	void LoadFromFile(const wstring& path);
	void SaveToFile(const wstring& path) const;
	void Clear();
private:
	map<wstring, TAG>	m_NameToTag;
	map<TAG, wstring>	m_TagToName;
};
NS_END