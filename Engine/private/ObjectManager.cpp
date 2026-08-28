#include "ObjectManager.h"

#include "Game.h"
#include "GameObject.h"

HRESULT ObjectManager::Initialize(void *arg) {
	m_LayerMask = ETOI(LAYER::ALL_LAYER);
    m_LevelCount = *static_cast<uint32*>(arg);

	m_ObjectByLayer.resize(m_LevelCount);
	m_ObjectByType.resize(m_LevelCount);
	m_ObjectByObject.resize(m_LevelCount);
	m_ObjectByInstance.resize(m_LevelCount);

	return EngineManager::Initialize(arg);
}

void ObjectManager::On_Destroy() {
	Clear_AllGameObjects();
	EngineManager::On_Destroy();
}

void ObjectManager::PriorityUpdate(Float timeDelta) {
    auto curLevel = GAME_INSTANCE->Get_CurrentLevelIndex();
    const auto priorityUpdate = [timeDelta](auto&& self, const Shared<GameObject>& object) -> void {
        if (!object || object->Is_Destroy() || !object->Is_Active())
            return;
        object->Priority_Update(timeDelta);
        if (object->Is_Destroy())
            return;
        const vector<Shared<GameObject>> children = object->Get_Children();
        for (const auto& child : children)
            if (child && child->Get_Parent() == object) self(self, child);
    };

	for (auto& [layerBit, objects] : m_ObjectByLayer[0]) {
		for (auto& obj : objects) {
			if (obj && !obj->Has_Parent())
				priorityUpdate(priorityUpdate, obj);
		}
	}

	if (curLevel != 0) {
		for (auto& [layerBit, objects] : m_ObjectByLayer[curLevel]) {
			if ((m_LayerMask & layerBit) == 0) continue;
			for (size_t i = 0; i < objects.size(); ++i) {
				auto obj = objects[i];
				if (obj && !obj->Has_Parent())
					priorityUpdate(priorityUpdate, obj);
			}
		}
	}
}

void ObjectManager::Update(Float timeDelta) {
    auto curLevel = GAME_INSTANCE->Get_CurrentLevelIndex();
    const auto update = [timeDelta](auto&& self, const Shared<GameObject>& object) -> void {
        if (!object || object->Is_Destroy() || !object->Is_Active())
            return;
        object->Update(timeDelta);
        if (object->Is_Destroy())
            return;
        const vector<Shared<GameObject>> children = object->Get_Children();
        for (const auto& child : children)
            if (child && child->Get_Parent() == object) self(self, child);
    };

	for (auto& [layerBit, objects] : m_ObjectByLayer[0]) {
		for (auto& obj : objects) {
			if (obj && !obj->Has_Parent())
				update(update, obj);
		}
	}

	if (curLevel != 0) {
		for (auto& [layerBit, objects] : m_ObjectByLayer[curLevel]) {
			if ((m_LayerMask & layerBit) == 0) continue;
			for (size_t i = 0; i < objects.size(); ++i) {
				auto obj = objects[i];
				if (obj && !obj->Has_Parent())
					update(update, obj);
			}
		}
	}
}

void ObjectManager::LateUpdate(Float timeDelta) {
    auto curLevel = GAME_INSTANCE->Get_CurrentLevelIndex();
    const auto lateUpdate = [timeDelta](auto&& self, const Shared<GameObject>& object) -> void {
        if (!object || object->Is_Destroy() || !object->Is_Active())
            return;
        object->Late_Update(timeDelta);
        if (object->Is_Destroy())
            return;
        const vector<Shared<GameObject>> children = object->Get_Children();
        for (const auto& child : children)
            if (child && child->Get_Parent() == object) self(self, child);
    };

	for (auto& [layerBit, objects] : m_ObjectByLayer[0]) {
		for (auto& obj : objects) {
			if (obj && !obj->Has_Parent())
				lateUpdate(lateUpdate, obj);
		}
	}

	if (curLevel != 0) {
		for (auto& [layerBit, objects] : m_ObjectByLayer[curLevel]) {
			if ((m_LayerMask & layerBit) == 0) continue;
			for(size_t i = 0; i < objects.size(); ++i) {
				auto obj = objects[i];
				if (obj && !obj->Has_Parent())
					lateUpdate(lateUpdate, obj);
			}
		}
	}
}

void ObjectManager::FixedUpdate(Float fixedDelta) {
    auto curLevel = GAME_INSTANCE->Get_CurrentLevelIndex();
    const auto fixedUpdate = [fixedDelta](auto&& self, const Shared<GameObject>& object) -> void {
        if (!object || object->Is_Destroy() || !object->Is_Active())
            return;
        object->Fixed_Update(fixedDelta);
        if (object->Is_Destroy())
            return;
        const vector<Shared<GameObject>> children = object->Get_Children();
        for (const auto& child : children)
            if (child && child->Get_Parent() == object) self(self, child);
    };

	for (auto& [layerBit, objects] : m_ObjectByLayer[0]) {
		for (auto& obj : objects) {
			if (obj && !obj->Has_Parent())
				fixedUpdate(fixedUpdate, obj);
		}
	}

	if (curLevel != 0) {
		for (auto& [layerBit, objects] : m_ObjectByLayer[curLevel]) {
			if ((m_LayerMask & layerBit) == 0) continue;
			for (size_t i = 0; i < objects.size(); ++i) {
				auto obj = objects[i];
				if (obj && !obj->Has_Parent())
					fixedUpdate(fixedUpdate, obj);
			}
		}
	}
}

void ObjectManager::Submit_RenderGroup() {
    auto curLevel = GAME_INSTANCE->Get_CurrentLevelIndex();
    const auto submitRenderGroup = [](auto&& self, const Shared<GameObject>& object) -> void {
        if (!object || object->Is_Destroy() || !object->Is_Active())
            return;
        object->Submit_RenderGroup();
        if (object->Is_Destroy())
            return;
        const vector<Shared<GameObject>> children = object->Get_Children();
        for (const auto& child : children)
            if (child && child->Get_Parent() == object) self(self, child);
    };

	for (auto& [layerBit, objects] : m_ObjectByLayer[0]) {
		for (auto& obj : objects) {
			if (obj && !obj->Has_Parent())
				submitRenderGroup(submitRenderGroup, obj);
		}
	}

	if (curLevel != 0) {
		for (auto& [layerBit, objects] : m_ObjectByLayer[curLevel]) {
			if ((m_LayerMask & layerBit) == 0) continue;
			for (auto& obj : objects) {
				if (obj && !obj->Has_Parent())
					submitRenderGroup(submitRenderGroup, obj);
			}
		}
	}
}

void ObjectManager::Cleanup_GameObjects(uint32 levIndex) {
    vector<Shared<GameObject>> garbages;
    for (auto &[layerBit, objects] : m_ObjectByLayer[levIndex]) {
		for (auto &obj : objects) {
			if (obj->Is_Destroy()) {
				garbages.push_back(obj);
			}
		}
    }

    for (auto &garbage : garbages) {
		garbage->On_Destroy();
    }

    for (auto &[layerBit, objects] : m_ObjectByLayer[levIndex]) {
		std::erase_if(objects, [&](const Shared<GameObject> &object) {
			if (object->Is_Destroy()) // 삭제 대상 처리
			{
                m_ObjectByInstance[levIndex].erase(object->Get_InstanceID());
                m_ObjectByGuid.erase(object->Get_ObjectGuid());
                m_ObjectByRuntimeId.erase(object->Get_RuntimeObjectId());
                if (m_ObjectByObject[levIndex].contains(object->Get_ObjectID())) {
                    auto& objectVec = m_ObjectByObject[levIndex][object->Get_ObjectID()];
                    std::erase(objectVec, object);
                }
				if (m_ObjectByType[levIndex].contains(object->Get_TypeID())) {
					auto& typeVec = m_ObjectByType[levIndex][object->Get_TypeID()];
					std::erase(typeVec, object);
				}
				return true;
			}
			return false;
		});
    }
}

HRESULT ObjectManager::Add_GameObject(uint32 levIndex, const Shared<GameObject>& object) {
    if (levIndex >= m_LevelCount || !object || !object->Get_ObjectGuid().Is_Valid() ||
        object->Get_RuntimeObjectId() == 0) {
        return E_INVALIDARG;
    }

    if (const auto guidIt = m_ObjectByGuid.find(object->Get_ObjectGuid());
        guidIt != m_ObjectByGuid.end() && !guidIt->second.expired()) {
        return E_FAIL;
    }

    if (const auto runtimeIt = m_ObjectByRuntimeId.find(object->Get_RuntimeObjectId());
        runtimeIt != m_ObjectByRuntimeId.end() && !runtimeIt->second.expired()) {
        return E_FAIL;
    }

    if (object->Get_InstanceID() == 0 ||
        m_ObjectByInstance[levIndex].contains(object->Get_InstanceID())) {
        return E_FAIL;
    }

    m_ObjectByGuid[object->Get_ObjectGuid()] = object;
    m_ObjectByRuntimeId[object->Get_RuntimeObjectId()] = object;
    m_ObjectByLayer[levIndex][object->Get_LayerMask().Get_Layer()].push_back(object);
    m_ObjectByType[levIndex][object->Get_TypeID()].push_back(object);
    m_ObjectByObject[levIndex][object->Get_ObjectID()].push_back(object);
    m_ObjectByInstance[levIndex].emplace(object->Get_InstanceID(), object);

    return S_OK;
}

HRESULT ObjectManager::Clear_GameObjects(uint32 levIndex) {
	if (levIndex >= m_LevelCount) return E_FAIL;

    for (auto &layer : m_ObjectByLayer[levIndex]) {
		for (auto &obj : layer.second) {
			if (!obj->Is_Destroy()) {
				obj->On_Destroy();
				Object::Destroy(obj);
			}
			m_ObjectByGuid.erase(obj->Get_ObjectGuid());
			m_ObjectByRuntimeId.erase(obj->Get_RuntimeObjectId());
		}
		layer.second.clear();
    }

	m_ObjectByLayer[levIndex].clear();
    m_ObjectByType[levIndex].clear();
    m_ObjectByObject[levIndex].clear();
    m_ObjectByInstance[levIndex].clear();

    m_LayerMask = ETOI(LAYER::ALL_LAYER);
    return S_OK;
}

HRESULT ObjectManager::Clear_AllGameObjects()
{
	for (uint32 i = 0; i < m_LevelCount; ++i)
	{
		for (auto& layer : m_ObjectByLayer[i]) {
			for (auto& obj : layer.second) {
				if (!obj->Is_Destroy()) {
					obj->On_Destroy();
					Object::Destroy(obj);
				}
				m_ObjectByGuid.erase(obj->Get_ObjectGuid());
				m_ObjectByRuntimeId.erase(obj->Get_RuntimeObjectId());
			}
			layer.second.clear();
		}
		m_ObjectByLayer[i].clear();
		m_ObjectByType[i].clear();
		m_ObjectByObject[i].clear();
		m_ObjectByInstance[i].clear();
	}

	m_LayerMask = ETOI(LAYER::ALL_LAYER);
	m_ObjectByGuid.clear();
	m_ObjectByRuntimeId.clear();
	return S_OK;
}

Shared<GameObject> ObjectManager::Find_ObjectByType(uint32 levIndex, uint32 typeID) {
    if (!m_ObjectByType[levIndex].contains(typeID) || m_ObjectByType[levIndex][typeID].empty()) {
		MSG_BOX("Failed To Find GameObject By TypeID");
		return nullptr;
    }

    return m_ObjectByType[levIndex][typeID].front();
}

const vector<Shared<GameObject>>& ObjectManager::Find_ObjectsByTypes(uint32 levIndex, uint32 typeID) {
    if (!m_ObjectByType[levIndex].contains(typeID) || m_ObjectByType[levIndex][typeID].empty()) {
        MSG_BOX("Failed To Find GameObject By TypeID");
        return EMPTY_VECTOR<Shared<GameObject>>;
    }

    return m_ObjectByType[levIndex][typeID];
}

Shared<GameObject> ObjectManager::Find_ObjectByObjectID(uint32 levIndex, uint32 objectID) {
    if (!m_ObjectByObject[levIndex].contains(objectID)) {
        // LOG_WARN(L"[ObjectManager] Failed To Find GameObject By ObjectID: {} in Level {}", objectID, levIndex);
        return nullptr;
    }

    return m_ObjectByObject[levIndex][objectID].front();
}

const vector<Shared<GameObject>>& ObjectManager::Find_ObjectsByObjectID(uint32 levIndex, uint32 objectID) {
    if (!m_ObjectByObject[levIndex].contains(objectID)) {
        // LOG_WARN(L"[ObjectManager] Failed To Find GameObjects By ObjectID: {} in Level {}", objectID, levIndex);
        return EMPTY_VECTOR<Shared<GameObject>>;
    }

    return m_ObjectByObject[levIndex][objectID];
}


Shared<GameObject> ObjectManager::Find_ByInstanceID(uint32 levIndex, uint32 instanceID)
{
    if (!m_ObjectByInstance[levIndex].contains(instanceID)) {
        // LOG_WARN(L"[ObjectManager] Failed To Find GameObject By InstanceID: {} in Level {}", instanceID, levIndex);
        return nullptr;
    }

    return m_ObjectByInstance[levIndex][instanceID];
}

Shared<GameObject> ObjectManager::Find_ByObjectGuid(ObjectGuid objectGuid) const
{
    if (!objectGuid.Is_Valid())
        return nullptr;

    const auto it = m_ObjectByGuid.find(objectGuid);
    return it == m_ObjectByGuid.end() ? nullptr : it->second.lock();
}

Shared<GameObject> ObjectManager::Find_ByRuntimeObjectId(RuntimeObjectId runtimeObjectId) const
{
    if (runtimeObjectId == 0)
        return nullptr;

    const auto it = m_ObjectByRuntimeId.find(runtimeObjectId);
    return it == m_ObjectByRuntimeId.end() ? nullptr : it->second.lock();
}

vector<Shared<GameObject>> ObjectManager::Find_AllByRuntimeTypeId(RuntimeTypeId runtimeTypeId) const
{
    vector<Shared<GameObject>> result;
    if (runtimeTypeId == 0)
        return result;

    result.reserve(m_ObjectByGuid.size());
    for (const auto& [objectGuid, weakObject] : m_ObjectByGuid) {
        if (const Shared<GameObject> object = weakObject.lock();
            object && !object->Is_Destroy() && object->Get_RuntimeTypeId() == runtimeTypeId) {
            result.push_back(object);
        }
    }
    std::ranges::sort(result, {}, [](const Shared<GameObject>& object) {
        return object->Get_ObjectGuid();
    });
    return result;
}

HRESULT ObjectManager::Destroy(ObjectGuid objectGuid)
{
    const Shared<GameObject> object = Find_ByObjectGuid(objectGuid);
    if (!object)
        return E_FAIL;

    object->Destroy_Subtree();
    return S_OK;
}

HRESULT ObjectManager::Remove_GameObject(uint32 levIndex, const Shared<GameObject>& object) {
    if (levIndex >= m_LevelCount || !object)
        return E_INVALIDARG;

    m_ObjectByGuid.erase(object->Get_ObjectGuid());
    m_ObjectByRuntimeId.erase(object->Get_RuntimeObjectId());
    m_ObjectByInstance[levIndex].erase(object->Get_InstanceID());

    if (auto layerIt = m_ObjectByLayer[levIndex].find(object->Get_LayerMask().Get_Layer());
        layerIt != m_ObjectByLayer[levIndex].end()) {
        std::erase(layerIt->second, object);
        if (layerIt->second.empty())
            m_ObjectByLayer[levIndex].erase(layerIt);
    }

    if (auto typeIt = m_ObjectByType[levIndex].find(object->Get_TypeID());
        typeIt != m_ObjectByType[levIndex].end()) {
        std::erase(typeIt->second, object);
        if (typeIt->second.empty())
            m_ObjectByType[levIndex].erase(typeIt);
    }

    if (auto objectIt = m_ObjectByObject[levIndex].find(object->Get_ObjectID());
        objectIt != m_ObjectByObject[levIndex].end()) {
        std::erase(objectIt->second, object);
        if (objectIt->second.empty())
            m_ObjectByObject[levIndex].erase(objectIt);
    }

    object->On_Destroy();
    Object::Destroy(object);
    return S_OK;
}

const unordered_map<uint32, Shared<GameObject>>& ObjectManager::Get_GameObjects(uint32 levIndex) {
	return m_ObjectByInstance[levIndex];
}

Shared<GameObject> ObjectManager::Find_ObjectByObjectTag(uint32 levIndex, const wstring& tagName)
{
	if (levIndex >= m_LevelCount)
		return nullptr;
	
	for (auto& pair : m_ObjectByInstance[levIndex])
	{
		if (pair.second->Get_Name() == tagName)
		{
			return pair.second;
		}
	}
	return nullptr;
}

Unique<ObjectManager> ObjectManager::Create(uint32 levCount) {
	auto objectManager = make_unique<ObjectManager>();

	if (FAILED(objectManager->Initialize(&levCount))) {
		MSG_BOX("Failed To Create ObjectManager");
		return nullptr;
	}

	return objectManager;
}
