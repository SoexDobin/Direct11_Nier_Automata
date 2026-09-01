#include "PrefabManager.h"
#include "Game.h"
#include "LevelSerializer.h"
#include <fstream>

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

HRESULT PrefabManager::Register_PrefabDocument(const wstring& filePath,
	PrefabGuid& outPrefabGuid)
{
	outPrefabGuid = {};
	if (filePath.empty())
		return E_INVALIDARG;

	try
	{
		ifstream input(filePath);
		if (!input.is_open())
			return E_FAIL;

		nlohmann::json document;
		input >> document;
		PrefabGuid prefabGuid{};
		if (!document.is_object() || document.value("schemaVersion", 0u) != 2u ||
			document.value("documentType", string{}) != "Prefab" ||
			!document.contains("prefabGuid") || !document["prefabGuid"].is_string() ||
			!Try_Parse_PrefabGuid(document["prefabGuid"].get<string>(), prefabGuid))
			return E_FAIL;

		const HRESULT registerResult = Register_Prefab(prefabGuid, filePath);
		if (FAILED(registerResult))
			return registerResult;
		outPrefabGuid = prefabGuid;
		return S_OK;
	}
	catch (const std::exception&)
	{
		return E_FAIL;
	}
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

HRESULT PrefabManager::SerializePrefabDocument(PrefabGuid prefabGuid, uint32 levIndex,
	const Shared<GameObject>& selectedRoot) const
{
	const wstring filePath = Find_PrefabPath(prefabGuid);
	return !filePath.empty() && selectedRoot
		? GAME_INSTANCE->m_LevelSerializer->SerializePrefab(
			prefabGuid, levIndex, selectedRoot, filePath)
		: E_FAIL;
}

HRESULT PrefabManager::DeSerializePrefabDocument(PrefabGuid prefabGuid, uint32 levIndex,
	Shared<GameObject>& outRoot) const
{
	outRoot.reset();
	const wstring filePath = Find_PrefabPath(prefabGuid);
	return !filePath.empty()
		? GAME_INSTANCE->m_LevelSerializer->DeSerializePrefab(
			prefabGuid, levIndex, filePath, outRoot)
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
