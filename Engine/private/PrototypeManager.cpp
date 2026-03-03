#include "PrototypeManager.h"
#include "Game.h"
#include "GameObject.h"
#include "SpdLogger.h"
#include "ScriptComponent.h"
#include "String_Helper.h"

#include "Engine_Define.h"

uint32 PrototypeManager::Get_TypeByName(const wstring &name) {
    std::lock_guard<std::recursive_mutex> lock(m_PrototypeMutex);
  uint32 level = Game::GetInstance()->Get_CurrentLevelIndex();

  if (!m_TypesByName[level].contains(name)) {
    LOG_WARN(L"{} : Failed To Get {} To typeID", name, m_ObjectName);
    MSG_BOX("Failed To Get TypeID");
  }

  return m_TypesByName[level][name];
}

const wstring &PrototypeManager::Get_NameByType(uint32 typeID) {
    std::lock_guard<std::recursive_mutex> lock(m_PrototypeMutex);
  uint32 level = Game::GetInstance()->Get_CurrentLevelIndex();

  if (!m_NameByTypes[level].contains(typeID) ||
      m_NameByTypes[level][typeID].empty()) {
    LOG_WARN(L"{} : Failed To Get TypeID:{} To Name", typeID, m_ObjectName);
    MSG_BOX("Failed To Get TypeName");
  }

  return m_NameByTypes[level][typeID];
}

HRESULT PrototypeManager::Initialize(void* arg) {
    m_LevelCount = arg == nullptr ? 0 : *static_cast<uintptr_t*>(arg);

    m_NameByTypes.resize(m_LevelCount);
    m_TypesByName.resize(m_LevelCount);
    m_GameObjects.resize(m_LevelCount);
    m_Components.resize(m_LevelCount);

    return S_OK;
}

void PrototypeManager::On_Destroy() {
  for (uint32 i = 0; i < m_LevelCount; ++i) {
    m_NameByTypes[i].clear();
    m_TypesByName[i].clear();
    m_GameObjects[i].clear();
    m_Components[i].clear();
  }

  m_NameByTypes.clear();
  m_TypesByName.clear();
  m_GameObjects.clear();
  m_Components.clear();
}

HRESULT PrototypeManager::Add_Prototype(uint32 levIndex,
                                        const Shared<Object> &object,
                                        void *arg) 
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

    if (!object->Get_Name().empty()) {
        m_NameByTypes[levIndex].emplace(object->Get_TypeID(), object->Get_Name());
        m_TypesByName[levIndex].emplace(object->Get_Name(), object->Get_TypeID());
    } else MSG_BOX("Miss Type Name");

    if (prototype == PROTOTYPE::GAMEOBJECT) {
		m_GameObjects[levIndex].emplace(object->Get_TypeID(), static_pointer_cast<GameObject>(object));
    } else if (prototype == PROTOTYPE::COMPONENT) {
		m_Components[levIndex].emplace(object->Get_TypeID(), static_pointer_cast<Component>(object));
    }

    return S_OK;
}

HRESULT PrototypeManager::Clear_Prototypes()
{
    for (auto container : m_NameByTypes)
        container.clear();
    m_NameByTypes.shrink_to_fit();

    for (auto container : m_TypesByName)
        container.clear();
    m_TypesByName.shrink_to_fit();

    for (auto container : m_GameObjects)
        container.clear();
    m_GameObjects.shrink_to_fit();

    for (auto container : m_Components)
        container.clear();
    m_Components.shrink_to_fit();

    m_NameByTypes.resize(m_LevelCount);
    m_TypesByName.resize(m_LevelCount);
    m_GameObjects.resize(m_LevelCount);
    m_Components.resize(m_LevelCount);

    return S_OK;
}

HRESULT PrototypeManager::Clear_Prototypes(uint32 levIndex) {
    if (!Validate_Level(levIndex)) {
      return E_FAIL;
    }

    m_NameByTypes[levIndex].clear();
    m_TypesByName[levIndex].clear();
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

Shared<Object> PrototypeManager::Find_Prototype(PROTOTYPE prototype,
                                                uint32 levIndex,
                                                const wstring &typeName) const 
{
    std::lock_guard<std::recursive_mutex> lock(m_PrototypeMutex);
    if (!Validate_Level(levIndex)) {
		return nullptr;
    }

	if (m_TypesByName[levIndex].contains(typeName)) {
		return Find_Prototype(prototype, levIndex, m_TypesByName[levIndex].at(typeName));
	}

	return nullptr;
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
