#include "PrefabManager.h"
#include "Game.h"

using namespace Engine;

HRESULT PrefabManager::Register_Prefab(PrefabGuid prefabGuid, const wstring& filePath)
{
	if (!prefabGuid.Is_Valid() || filePath.empty())
		return E_INVALIDARG;

	const auto [it, inserted] = m_PrefabPaths.emplace(prefabGuid, filePath);
	if (inserted)
		return S_OK;
	return it->second == filePath ? S_FALSE : E_FAIL;
}

HRESULT PrefabManager::Unregister_Prefab(PrefabGuid prefabGuid)
{
	if (!prefabGuid.Is_Valid())
		return E_INVALIDARG;
	return m_PrefabPaths.erase(prefabGuid) == 1 ? S_OK : S_FALSE;
}

wstring PrefabManager::Find_PrefabPath(PrefabGuid prefabGuid) const
{
	const auto it = m_PrefabPaths.find(prefabGuid);
	return it == m_PrefabPaths.end() ? wstring{} : it->second;
}

HRESULT PrefabManager::SerializePrefabDocument(PrefabGuid prefabGuid, uint32 levIndex) const
{
	const wstring filePath = Find_PrefabPath(prefabGuid);
	return !filePath.empty()
		? GAME_INSTANCE->SerializeLevel(levIndex, filePath)
		: E_FAIL;
}

HRESULT PrefabManager::DeSerializePrefabDocument(PrefabGuid prefabGuid) const
{
	const wstring filePath = Find_PrefabPath(prefabGuid);
	return !filePath.empty()
		? GAME_INSTANCE->DeSerializeLevel(filePath)
		: E_FAIL;
}

HRESULT PrefabManager::Prepare_RestoredObjectGuid(ObjectGuid objectGuid)
{
	if (!objectGuid.Is_Valid())
		return E_INVALIDARG;
	if (m_PendingObjectGuid.Is_Valid())
		return E_FAIL;
	m_PendingObjectGuid = objectGuid;
	return S_OK;
}

ObjectGuid PrefabManager::Consume_RestoredObjectGuid() noexcept
{
	const ObjectGuid objectGuid = m_PendingObjectGuid;
	m_PendingObjectGuid = {};
	return objectGuid;
}
