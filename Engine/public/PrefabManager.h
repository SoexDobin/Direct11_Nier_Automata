#pragma once

#include "Engine_Define.h"

NS_BEGIN(Engine)

class GameObject;

class PrefabManager final
{
	NO_COPY(PrefabManager)

public:
	PrefabManager() = default;
	~PrefabManager() = default;

	HRESULT Register_Prefab(PrefabGuid prefabGuid, const wstring& filePath);
	HRESULT Unregister_Prefab(PrefabGuid prefabGuid);
	wstring Find_PrefabPath(PrefabGuid prefabGuid) const;
	HRESULT SerializePrefabDocument(PrefabGuid prefabGuid, uint32 levIndex) const;
	HRESULT DeSerializePrefabDocument(PrefabGuid prefabGuid) const;
	HRESULT Prepare_RestoredObjectGuid(ObjectGuid objectGuid);
	ObjectGuid Consume_RestoredObjectGuid() noexcept;

private:
	unordered_map<PrefabGuid, wstring, GuidHash> m_PrefabPaths;
	ObjectGuid m_PendingObjectGuid{};
};

NS_END
