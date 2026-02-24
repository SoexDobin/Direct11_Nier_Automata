#include "PrototypeManager.h"
#include "Game.h"
#include "GameObject.h"
#include "SpdLogger.h"
#include "ScriptComponent.h"
#include "String_Helper.h"
#include "LayerRegistry.h"
#include "TagRegistry.h"
#include "SpdLogger.h"

#include "Engine_Define.h"

uint32 PrototypeManager::Get_TypeByName(const wstring &name) {
  uint32 level = Game::GetInstance()->Get_CurrentLevelIndex();

  if (!m_TypesByName[level].contains(name)) {
    LOG_WARN(L"{} : Failed To Get {} To typeID", name, m_ObjectName);
    MSG_BOX("Failed To Get TypeID");
  }

  return m_TypesByName[level][name];
}

const wstring &PrototypeManager::Get_NameByType(uint32 typeID) {
  uint32 level = Game::GetInstance()->Get_CurrentLevelIndex();

  if (!m_NameByTypes[level].contains(typeID) ||
      m_NameByTypes[level][typeID].empty()) {
    LOG_WARN(L"{} : Failed To Get TypeID:{} To Name", typeID, m_ObjectName);
    MSG_BOX("Failed To Get TypeName");
  }

  return m_NameByTypes[level][typeID];
}

HRESULT PrototypeManager::Initialize(void* arg) {
  m_LevelCount = arg == nullptr ? 0 : reinterpret_cast<uintptr_t>(arg);

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
                                        void *arg) {
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
  }

  if (prototype == PROTOTYPE::GAMEOBJECT) {
    m_GameObjects[levIndex].emplace(object->Get_TypeID(),
                                    static_pointer_cast<GameObject>(object));
  } else if (prototype == PROTOTYPE::COMPONENT) {
    m_Components[levIndex].emplace(object->Get_TypeID(),
                                   static_pointer_cast<Component>(object));
  }

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
                                                uint32 typeID) const {
  if (!Validate_Level(levIndex)) {
    return nullptr;
  }

  if (prototype == PROTOTYPE::GAMEOBJECT) {
    auto it = m_GameObjects[levIndex].find(typeID);
    if (it != m_GameObjects[levIndex].end()) {
      return it->second;
    }
  } else if (prototype == PROTOTYPE::COMPONENT) {
    auto it = m_Components[levIndex].find(typeID);
    if (it != m_Components[levIndex].end()) {
      return it->second;
    }
  }

  return nullptr;
}

Shared<Object> PrototypeManager::Find_Prototype(PROTOTYPE prototype,
                                                uint32 levIndex,
                                                const wstring &typeName) const {
  if (!Validate_Level(levIndex)) {
    return nullptr;
  }

  if (m_TypesByName[levIndex].contains(typeName)) {
    return Find_Prototype(prototype, levIndex,
                          m_TypesByName[levIndex].at(typeName));
  }

  return nullptr;
}

HRESULT PrototypeManager::Create_Reflection(
    const ComPtr<ID3D11Device> &device,
    const ComPtr<ID3D11DeviceContext> &context) {
  type type_GameObject = type::get<GameObject>();
  type type_Component = type::get<Component>();

  auto allTypes = type::get_types();

  /* [Pass 1] Component 먼저 등록 (의존성 해결) */
  for (auto &t : allTypes) {
    if (t.is_derived_from(type_Component) &&
        !t.is_derived_from(type_GameObject))
      Register_Type(t, PROTOTYPE::COMPONENT, device, context);
  }

  /* [Pass 2] GameObject 등록 (Component가 이미 등록됨) */
  for (auto &t : allTypes) {
    if (t.is_derived_from(type_GameObject))
      Register_Type(t, PROTOTYPE::GAMEOBJECT, device, context);
  }

  return S_OK;
}

void PrototypeManager::Register_Type(
    rttr::type type, PROTOTYPE protoType, const ComPtr<ID3D11Device> &device,
    const ComPtr<ID3D11DeviceContext> &context) {
  if (type.get_name().empty())
    return;

  // 1. RTTR 생성자를 통해 객체 생성 시도
  variant result = type.create({device, context});
  if (!result.is_valid()) {
    // 인자 없는 생성자 시도 (의존성은 나중에 주입)
    result = type.create();
    if (!result.is_valid()) {
      LOG_WARN(L"[RTTR] No valid constructor found for: {}",
               Helper::To_wString(type.get_name().to_string()));
      return;
    }
  }

  Shared<Object> prototype = result.get_value<Shared<Object>>();
  if (!prototype)
    return;

  wstring typeName = Helper::To_wString(type.get_name().to_string());
  uint32 typeID = prototype->Get_TypeID();

  for (size_t i = 0; i < m_LevelCount; ++i)
  {
      m_NameByTypes[i].emplace(typeID, typeName);
      m_TypesByName[i].emplace(typeName, typeID);

      if (protoType == PROTOTYPE::GAMEOBJECT)
          m_GameObjects[i].emplace(typeID, static_pointer_cast<GameObject>(prototype));
      else if (protoType == PROTOTYPE::COMPONENT)
          m_Components[i].emplace(typeID, static_pointer_cast<Component>(prototype));
  }
    
}

Unique<PrototypeManager> PrototypeManager::Create(uint32 levCount) {
  auto prototypeManager = make_unique<PrototypeManager>();

  if (FAILED(prototypeManager->Initialize(reinterpret_cast<void *>(levCount)))) {
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
