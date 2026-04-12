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

	for (auto& [layerBit, objects] : m_ObjectByLayer[0]) {
		for (auto& obj : objects) {
			if (!obj->Is_Destroy() && obj->Is_Active())
				obj->Priority_Update(timeDelta);
		}
	}

	if (curLevel != 0) {
		for (auto& [layerBit, objects] : m_ObjectByLayer[curLevel]) {
			if ((m_LayerMask & layerBit) == 0) continue;
			for (size_t i = 0; i < objects.size(); ++i) {
				auto obj = objects[i];
				if (!obj->Is_Destroy() && obj->Is_Active())
					obj->Priority_Update(timeDelta);
			}
		}
	}
}

void ObjectManager::Update(Float timeDelta) {
    auto curLevel = GAME_INSTANCE->Get_CurrentLevelIndex();

	for (auto& [layerBit, objects] : m_ObjectByLayer[0]) {
		for (auto& obj : objects) {
			if (!obj->Is_Destroy() && obj->Is_Active())
				obj->Update(timeDelta);
		}
	}

	if (curLevel != 0) {
		for (auto& [layerBit, objects] : m_ObjectByLayer[curLevel]) {
			if ((m_LayerMask & layerBit) == 0) continue;
			for (size_t i = 0; i < objects.size(); ++i) {
				auto obj = objects[i];
				if (!obj->Is_Destroy() && obj->Is_Active())
					obj->Update(timeDelta);
			}
		}
	}
}

void ObjectManager::LateUpdate(Float timeDelta) {
    auto curLevel = GAME_INSTANCE->Get_CurrentLevelIndex();

	for (auto& [layerBit, objects] : m_ObjectByLayer[0]) {
		for (auto& obj : objects) {
			if (!obj->Is_Destroy() && obj->Is_Active())
				obj->Late_Update(timeDelta);
		}
	}

	if (curLevel != 0) {
		for (auto& [layerBit, objects] : m_ObjectByLayer[curLevel]) {
			if ((m_LayerMask & layerBit) == 0) continue;
			for(size_t i = 0; i < objects.size(); ++i) {
				auto obj = objects[i];
				if (!obj->Is_Destroy() && obj->Is_Active())
					obj->Late_Update(timeDelta);
			}
		}
	}
}

void ObjectManager::FixedUpdate(Float fixedDelta) {
    auto curLevel = GAME_INSTANCE->Get_CurrentLevelIndex();

	for (auto& [layerBit, objects] : m_ObjectByLayer[0]) {
		for (auto& obj : objects) {
			if (!obj->Is_Destroy() && obj->Is_Active())
				obj->Fixed_Update(fixedDelta);
		}
	}

	if (curLevel != 0) {
		for (auto& [layerBit, objects] : m_ObjectByLayer[curLevel]) {
			if ((m_LayerMask & layerBit) == 0) continue;
			for (size_t i = 0; i < objects.size(); ++i) {
				auto obj = objects[i];
				if (!obj->Is_Destroy() && obj->Is_Active())
					obj->Fixed_Update(fixedDelta);
			}
		}
	}
}

void ObjectManager::Submit_RenderGroup() {
    auto curLevel = GAME_INSTANCE->Get_CurrentLevelIndex();

	for (auto& [layerBit, objects] : m_ObjectByLayer[0]) {
		for (auto& obj : objects) {
			if (!obj->Is_Destroy() && obj->Is_Active())
				obj->Submit_RenderGroup();
		}
	}

	if (curLevel != 0) {
		for (auto& [layerBit, objects] : m_ObjectByLayer[curLevel]) {
			if ((m_LayerMask & layerBit) == 0) continue;
			for (auto& obj : objects) {
				if (!obj->Is_Destroy() && obj->Is_Active())
					obj->Submit_RenderGroup();
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
			}
			layer.second.clear();
		}
		m_ObjectByLayer[i].clear();
		m_ObjectByType[i].clear();
		m_ObjectByObject[i].clear();
		m_ObjectByInstance[i].clear();
	}

	m_LayerMask = ETOI(LAYER::ALL_LAYER);
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

const unordered_map<uint32, Shared<GameObject>>& ObjectManager::Get_GameObjects(uint32 levIndex) {
	return m_ObjectByInstance[levIndex];
}

Unique<ObjectManager> ObjectManager::Create(uint32 levCount) {
	auto objectManager = make_unique<ObjectManager>();

	if (FAILED(objectManager->Initialize(&levCount))) {
		MSG_BOX("Failed To Create ObjectManager");
		return nullptr;
	}

	return objectManager;
}
