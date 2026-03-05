#include "PrototypeManager.h"
#include "Game.h"
#include "GameObject.h"
#include "SpdLogger.h"
#include "ScriptComponent.h"

#include "Engine_Define.h"

HRESULT PrototypeManager::Initialize(void* arg) {
    m_LevelCount = arg == nullptr ? 0 : *static_cast<uintptr_t*>(arg);

    m_GameObjects.resize(m_LevelCount);
    m_Components.resize(m_LevelCount);

    return S_OK;
}

void PrototypeManager::On_Destroy() {
  for (uint32 i = 0; i < m_LevelCount; ++i) {
    m_GameObjects[i].clear();
    m_Components[i].clear();
  }

  m_GameObjects.clear();
  m_Components.clear();
}

HRESULT PrototypeManager::Add_Prototype(uint32 levIndex, const Shared<Object> &object) 
{
    std::lock_guard<std::recursive_mutex> lock(m_PrototypeMutex);
	if (!Validate_Level(levIndex)) {
		return E_FAIL;
	}

    PROTOTYPE prototype = object->Get_Prototype();
    if (Find_Prototype(prototype, levIndex, object->Get_TypeID()) != nullptr) {
        LOG_ERROR(L"{}: Already Added Prototype", m_ObjectName);
        MSG_BOX("Already Added Prototype");
        return E_FAIL;
    }

    if (prototype == PROTOTYPE::GAMEOBJECT) {
		m_GameObjects[levIndex].emplace(object->Get_TypeID(), static_pointer_cast<GameObject>(object));
        m_TypeObjects[levIndex][object->Get_TypeID()].push_back(object);
    } else if (prototype == PROTOTYPE::COMPONENT) {
		m_Components[levIndex].emplace(object->Get_TypeID(), static_pointer_cast<Component>(object));
        m_TypeObjects[levIndex][object->Get_TypeID()].push_back(object);
    }

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

    m_GameObjects.resize(m_LevelCount);
    m_Components.resize(m_LevelCount);

    return S_OK;
}

HRESULT PrototypeManager::Clear_Prototypes(uint32 levIndex) {
    if (!Validate_Level(levIndex)) {
      return E_FAIL;
    }

    m_GameObjects[levIndex].clear();
    m_Components[levIndex].clear();

    return S_OK;
}

Shared<Object> PrototypeManager::Find_Prototype(PROTOTYPE prototype,
                                                uint32 levIndex,
                                                uint32 typeID) const
{
    std::lock_guard<std::recursive_mutex> lock(m_PrototypeMutex);
	if (!Validate_Level(levIndex)) {
		return nullptr;
	}

	if (prototype == PROTOTYPE::GAMEOBJECT) {
		if (m_GameObjects[levIndex].contains(typeID))
		{
			return m_GameObjects[levIndex].at(typeID);
		}
        return nullptr;
	} 
	if (prototype == PROTOTYPE::COMPONENT) {
		if (m_Components[levIndex].contains(typeID))
		{
			return m_Components[levIndex].at(typeID);
		}
        return nullptr;
	}

    return nullptr;
}

const vector<Shared<Object>>& PrototypeManager::Find_Prototypes(uint32 levIndex, uint32 typeID)
{
    std::lock_guard<std::recursive_mutex> lock(m_PrototypeMutex);
    static const vector<Shared<Object>> empty_object;
    if (!Validate_Level(levIndex)) {
        return empty_object;
    }

    if (m_TypeObjects[levIndex].contains(typeID))
    {
        return m_TypeObjects[levIndex][typeID];
    }

    return empty_object;
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
