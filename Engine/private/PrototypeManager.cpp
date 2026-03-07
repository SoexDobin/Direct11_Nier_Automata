#include "PrototypeManager.h"
#include "Game.h"
#include "GameObject.h"
#include "SpdLogger.h"
#include "ScriptComponent.h"

#include "Engine_Define.h"

HRESULT PrototypeManager::Initialize(void* arg) {
    m_LevelCount = arg == nullptr ? 0 : *static_cast<uintptr_t*>(arg);

    m_ObjectsID.resize(m_LevelCount);
    m_GameObjects.resize(m_LevelCount);
    m_Components.resize(m_LevelCount);

    return S_OK;
}

void PrototypeManager::On_Destroy() {
    for (uint32 i = 0; i < m_LevelCount; ++i) {
        m_GameObjects[i].clear();
        m_Components[i].clear();
        m_ObjectsID[i].clear();
    }

    m_ObjectsID.clear();
    m_GameObjects.clear();
    m_Components.clear();
}

uint32 PrototypeManager::Get_ObjectIDFromPrototypeTag(const wstring& prototypeTag)
{
    if (false == m_ObjectsID[GAME_INSTANCE->Get_CurrentLevelIndex()].contains(prototypeTag))
    {
        LOG_ERROR(L"{} has no ObjectID", prototypeTag);
        return 0;
    }

    return m_ObjectsID[GAME_INSTANCE->Get_CurrentLevelIndex()].at(prototypeTag);
}

const wstring& PrototypeManager::Get_PrototypeTagFromObjectID(uint32 objectID)
{
    for (auto objectsID : m_ObjectsID[GAME_INSTANCE->Get_CurrentLevelIndex()])
    {
        if (objectsID.second == objectID)
            return objectsID.first;
    }

    return nullptr;
}

HRESULT PrototypeManager::Add_Prototype(uint32 levIndex, const Shared<Object>& object, const wstring& prototypeTag)
{
    std::lock_guard<std::recursive_mutex> lock(m_PrototypeMutex);

	if (!Validate_Level(levIndex)) {
		return E_FAIL;
	}

    PROTOTYPE prototype = object->Get_Prototype();
    if (Find_Prototype(prototype, levIndex, object->Get_ObjectID()) != nullptr) {
        LOG_ERROR(L"{}: Already Added Prototype", m_ObjectName);
        MSG_BOX("Already Added Prototype");
        return E_FAIL;
    }

    wstring finalTag = prototypeTag;
    if (finalTag.empty()) {
        finalTag = object->Get_Name();
    }
    wstring baseTag = finalTag;
    int index = 0;

    while (m_ObjectsID[levIndex].find(finalTag) != m_ObjectsID[levIndex].end()) {
        finalTag = baseTag + L"_" + to_wstring(index);
        index++;
    }

    if (prototype == PROTOTYPE::GAMEOBJECT) {
		m_GameObjects[levIndex].emplace(object->Get_ObjectID(), static_pointer_cast<GameObject>(object));
    } else if (prototype == PROTOTYPE::COMPONENT) {
		m_Components[levIndex].emplace(object->Get_ObjectID(), static_pointer_cast<Component>(object));
    }
    m_ObjectsID[levIndex].emplace(prototypeTag, object->Get_ObjectID());


    return S_OK;
}

HRESULT PrototypeManager::Clear_Prototypes()
{
    for (auto container : m_GameObjects)
        container.clear();
    m_GameObjects.shrink_to_fit();
    for (auto container : m_Components)
        container.clear();
    m_Components.shrink_to_fit();
    for (auto container : m_ObjectsID)
        container.clear();
    m_ObjectsID.shrink_to_fit();

    m_GameObjects.resize(m_LevelCount);
    m_Components.resize(m_LevelCount);
    m_ObjectsID.resize(m_LevelCount);

    return S_OK;
}

HRESULT PrototypeManager::Clear_Prototypes(uint32 levIndex) {
    if (!Validate_Level(levIndex)) {
      return E_FAIL;
    }

    m_GameObjects[levIndex].clear();
    m_Components[levIndex].clear();
    m_ObjectsID[levIndex].clear();

    return S_OK;
}

Shared<Object> PrototypeManager::Find_Prototype(PROTOTYPE prototype,
                                                uint32 levIndex,
                                                uint32 objectID) const {
    std::lock_guard<std::recursive_mutex> lock(m_PrototypeMutex);
	if (!Validate_Level(levIndex)) {
		return nullptr;
	}

    if (prototype == PROTOTYPE::GAMEOBJECT) {
        if (m_GameObjects[0].contains(objectID)) {
            return m_GameObjects[0].at(objectID);
        }
    }
    else if (prototype == PROTOTYPE::COMPONENT) {
        if (m_Components[0].contains(objectID)) {
            return m_Components[0].at(objectID);
        }
    }

	if (prototype == PROTOTYPE::GAMEOBJECT) {
		if (m_GameObjects[levIndex].contains(objectID)) {
			return m_GameObjects[levIndex].at(objectID);
		}
	} 
	else if (prototype == PROTOTYPE::COMPONENT) {
		if (m_Components[levIndex].contains(objectID)) {
			return m_Components[levIndex].at(objectID);
		}
	}

    return nullptr;
}

Shared<Object> PrototypeManager::Find_Prototype(PROTOTYPE prototype, 
												uint32 levIndex, 
												const wstring& prototypeTag) const
{
    std::lock_guard<std::recursive_mutex> lock(m_PrototypeMutex);
    if (!Validate_Level(levIndex)) {
        return nullptr;
    }
    if (prototypeTag.empty()) return nullptr;

    if (false == m_ObjectsID[levIndex].contains(prototypeTag))
    {
		MSG_BOX("Can not found such prototypeTag");
        LOG_ERROR(L"Can not found such prototypeTag : {}", prototypeTag);
        return nullptr;
    }
    
    return Find_Prototype(prototype, levIndex, m_ObjectsID[levIndex].at(prototypeTag));
}

Unique<PrototypeManager> PrototypeManager::Create(uint32 levCount) {
    auto prototypeManager = make_unique<PrototypeManager>();

    if (FAILED(prototypeManager->Initialize(&levCount))) {
          MSG_BOX("Failed To Create PrototypeManager");
          return nullptr;
    }

    return prototypeManager;
}

Bool PrototypeManager::Validate_Level(uint32 levIndex) const {
  if (levIndex >= m_LevelCount) {
    LOG_ERROR(L"{}: Out Of Level", m_ObjectName);
    MSG_BOX("Out Of Level Count");
    return false;
  }

  return true;
}
