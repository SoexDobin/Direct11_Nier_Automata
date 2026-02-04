#include "PrototypeManager.h"
#include "GameObject.h"
#include "ScriptComponent.h"
#include "String_Helper.h"
#include "Type_Helper.h"

HRESULT PrototypeManager::Initialize(const Shared<void> &arg) {
  m_LevelCount = *static_pointer_cast<uint32>(arg);

  m_Types.shrink_to_fit();
  m_TypesByName.shrink_to_fit();
  m_GameObjects.shrink_to_fit();
  m_Components.shrink_to_fit();

  m_Types.resize(m_LevelCount);
  m_TypesByName.resize(m_LevelCount);
  m_GameObjects.resize(m_LevelCount);
  m_Components.resize(m_LevelCount);

  for (auto &prototype : m_TypesByName)
    prototype.clear();
  for (auto &prototype : m_GameObjects)
    prototype.clear();
  for (auto &prototype : m_Components)
    prototype.clear();

  return S_OK;
}

void PrototypeManager::On_Destroy() {
  for (auto &prototype : m_TypesByName)
    prototype.clear();
  for (auto &prototype : m_GameObjects)
    prototype.clear();
  for (auto &prototype : m_Components)
    prototype.clear();

  m_Types.shrink_to_fit();
  m_TypesByName.shrink_to_fit();
  m_GameObjects.shrink_to_fit();
  m_Components.shrink_to_fit();
}

HRESULT PrototypeManager::Add_Prototype(uint32 levIndex,
                                        const Shared<Object> &object,
                                        const Shared<void> &arg) {
  if (levIndex >= m_LevelCount) {
    MSG_BOX("Out Of Level Count");
    return E_FAIL;
  }

  PROTOTYPE prototype = object->Get_Prototype();
  if (Find_Prototype(prototype, levIndex, object->Get_TypeID()) != nullptr) {
    MSG_BOX("Already Added Prototype");
    return E_FAIL;
  }

  m_Types[levIndex].emplace(object->Get_TypeID());
  if (object->Get_Name() != L"")
    m_TypesByName[levIndex].emplace(object->Get_Name(), object->Get_TypeID());
  if (prototype == PROTOTYPE::GAMEOBJECT)
    m_GameObjects[levIndex].emplace(object->Get_TypeID(),
                                    static_pointer_cast<GameObject>(object));
  else if (prototype == PROTOTYPE::COMPONENT)
    m_Components[levIndex].emplace(object->Get_TypeID(),
                                   static_pointer_cast<Component>(object));

  return S_OK;
}

HRESULT PrototypeManager::Clear_Prototypes(uint32 levIndex) {
  if (levIndex >= m_LevelCount)
    return E_FAIL;

  m_Types[levIndex].clear();
  m_TypesByName[levIndex].clear();
  m_GameObjects[levIndex].clear();
  m_Components[levIndex].clear();

  return S_OK;
}

template <typename T>
constexpr Shared<T>
PrototypeManager::Clone_Prototype(PROTOTYPE prototype, uint32 levIndex,
                                  uint32 typeID, const Shared<void> &arg) {
  if (Shared<Object> instance =
          Clone_Prototype(prototype, levIndex, typeID, arg))
    return static_pointer_cast<T>(instance);

  return nullptr;
}

Shared<Object> PrototypeManager::Clone_Prototype(PROTOTYPE prototype,
                                                 uint32 levIndex, uint32 typeID,
                                                 const Shared<void> &arg) {
  if (levIndex >= m_LevelCount) {
    MSG_BOX("Out Of Level Count");
    return nullptr;
  }

  if (Shared<Object> instance = Find_Prototype(prototype, levIndex, typeID)) {
    if (SUCCEEDED(instance->Initialize(arg)))
      return instance;

    MSG_BOX("Failed To Clone Instance");
    return nullptr;
  }

  MSG_BOX("Failed To Clone Instance");
  return nullptr;
}

Shared<Object> PrototypeManager::Clone_Prototype(PROTOTYPE prototype,
                                                 uint32 levIndex,
                                                 const wstring &typeName,
                                                 const Shared<void> &arg) {
  if (levIndex >= m_LevelCount) {
    MSG_BOX("Out Of Level Count");
    return nullptr;
  }

  if (m_TypesByName[levIndex].contains(typeName) == false) {
    // TODO : critical
    MSG_BOX("No Class Name At Prototype \n Check Reflection or ObjectName");
    return nullptr;
  }

  return Clone_Prototype(prototype, levIndex, m_TypesByName[levIndex][typeName],
                         arg);
}

Shared<Object> PrototypeManager::Find_Prototype(PROTOTYPE prototype,
                                                uint32 levIndex,
                                                uint32 typeID) const {
  if (levIndex >= m_LevelCount)
    return nullptr;
  if (!m_Types[levIndex].contains(typeID))
    return nullptr;

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
  if (m_TypesByName[levIndex].contains(typeName) == false) {
    // TODO : critical
    MSG_BOX("No Class Name At Prototype \n Check Reflection or ObjectName");
    return nullptr;
  }

  return Find_Prototype(prototype, levIndex,
                        m_TypesByName[levIndex].at(typeName));
}

HRESULT PrototypeManager::Create_Reflection(
    const ComPtr<ID3D11Device> &device,
    const ComPtr<ID3D11DeviceContext> &context) {
  type type_GameObject = Helper::Get_Type<GameObject>();
  type type_Component = Helper::Get_Type<Component>();

  for (auto &type : type::get_types()) {
    Bool isDerivedObj = type.is_derived_from(type_GameObject);
    Bool isDerivedCom = type.is_derived_from(type_Component);

    if (!isDerivedObj && !isDerivedCom)
      continue;

    method createMethod = type.get_method("Create");
    if (createMethod.is_valid() == false)
      continue;

    variant result = createMethod.invoke({}, device, context);
    if (result.is_valid() == false) {
      // TODO : 리플랙션한 클래스 출력
      continue;
    } else {
      // TODO : 리플랙션 못한 클래스 출력
    }

    Shared<Object> prototype = result.get_value<Shared<Object>>();
    if (prototype == nullptr)
      continue;

    std::wstring typeName = Helper::To_wString(type.get_name().to_string());

    // for (size_t i = 0; i < m_LevelCount; ++i)
    //{
    //	m_Prototypes[i].emplace(prototype->Get_TypeID(), prototype);
    //	m_TypeNames[i].emplace(typeName, prototype->Get_TypeID());
    // }
  }
  return S_OK;
}

Unique<PrototypeManager> PrototypeManager::Create(uint32 levCount) {
  auto prototypeManager = make_unique<PrototypeManager>();

  if (FAILED(prototypeManager->Initialize(
          reinterpret_cast<const Shared<uint32> &>(levCount)))) {
    MSG_BOX("Failed To Create PrototypeManager");
    return nullptr;
  }

  return prototypeManager;
}
