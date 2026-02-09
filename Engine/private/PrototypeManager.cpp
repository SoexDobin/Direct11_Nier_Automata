#include "PrototypeManager.h"

#include "Game.h"
#include "GameObject.h"
#include "ScriptComponent.h"
#include "SpdLogger.h"
#include "String_Helper.h"
#include "Type_Helper.h"

uint32 PrototypeManager::Get_TypeByName(const wstring& name)
{
    uint32 level = Game::GetInstance()->Get_CurrentLevelIndex();

    if (!m_TypesByName[level].contains(name))
    {
        LOG_WARN(L"{} : Failed To Get {} To typeID", name, m_ObjectName);
        MSG_BOX("Failed To Get TypeID");
    }

    return m_TypesByName[level][name];
}

const wstring& PrototypeManager::Get_NameByType(uint32 typeID)
{
    uint32 level = Game::GetInstance()->Get_CurrentLevelIndex();

    if (!m_NameByTypes[level].contains(typeID) || m_NameByTypes[level][typeID].empty())
    {
        LOG_WARN(L"{} : Failed To Get TypeID:{} To Name", typeID, m_ObjectName);
        MSG_BOX("Failed To Get TypeName");
    }

    return m_NameByTypes[level][typeID];
}

HRESULT PrototypeManager::Initialize(const Shared<void> &arg) 
{
    m_LevelCount = *static_pointer_cast<uint32>(arg);

    m_NameByTypes.shrink_to_fit();
    m_TypesByName.shrink_to_fit();
    m_GameObjects.shrink_to_fit();
    m_Components.shrink_to_fit();

    m_NameByTypes.resize(m_LevelCount);
    m_TypesByName.resize(m_LevelCount);
    m_GameObjects.resize(m_LevelCount);
    m_Components.resize(m_LevelCount);

    for (auto& prototype : m_NameByTypes)
        prototype.clear();
    for (auto &prototype : m_TypesByName)
		prototype.clear();
    for (auto &prototype : m_GameObjects)
		prototype.clear();
    for (auto &prototype : m_Components)
		prototype.clear();

    return S_OK;
}

void PrototypeManager::On_Destroy() 
{
    for (auto& prototype : m_NameByTypes)
        prototype.clear();
    for (auto &prototype : m_TypesByName)
		prototype.clear();
    for (auto &prototype : m_GameObjects)
		prototype.clear();
    for (auto &prototype : m_Components)
        prototype.clear();

    m_NameByTypes.shrink_to_fit();
    m_TypesByName.shrink_to_fit();
    m_GameObjects.shrink_to_fit();
    m_Components.shrink_to_fit();
}

HRESULT PrototypeManager::Add_Prototype(uint32 levIndex,
                                        const Shared<Object> &object,
                                        const Shared<void> &arg) 
{
    if (!Validate_Level(levIndex)) return E_FAIL;

    PROTOTYPE prototype = object->Get_Prototype();
    if (Find_Prototype(prototype, levIndex, object->Get_TypeID()) != nullptr) 
    {
        LOG_ERROR(L"{}: Already Added Prototype", m_ObjectName);
		MSG_BOX("Already Added Prototype");
		return E_FAIL;
    }

    if (object->Get_Name() != L"")
    {
        LOG_WARN(L"Object No Name TypeID:{}", object->Get_TypeID());
        m_NameByTypes[levIndex].emplace(object->Get_TypeID(), object->Get_Name());
        m_TypesByName[levIndex].emplace(object->Get_Name(), object->Get_TypeID());
    }

    if (prototype == PROTOTYPE::GAMEOBJECT)
		m_GameObjects[levIndex].emplace(object->Get_TypeID(), static_pointer_cast<GameObject>(object));
    else if (prototype == PROTOTYPE::COMPONENT)
        m_Components[levIndex].emplace(object->Get_TypeID(), static_pointer_cast<Component>(object));

    return S_OK;
}

HRESULT PrototypeManager::Clear_Prototypes(uint32 levIndex) 
{
    if (!Validate_Level(levIndex)) return E_FAIL;

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
    if (!Validate_Level(levIndex)) return nullptr;
    if (!m_NameByTypes[levIndex].contains(typeID))
    {
        LOG_ERROR(L"{}: Failed To Find TypeID", m_ObjectName);
        MSG_BOX("Failed To Find TypeID");
        return nullptr;
    }
		

    if (prototype == PROTOTYPE::GAMEOBJECT) 
    {
		auto it = m_GameObjects[levIndex].find(typeID);
		if (it != m_GameObjects[levIndex].end()) 
            return it->second;
      
    } else if (prototype == PROTOTYPE::COMPONENT) 
    {
        auto it = m_Components[levIndex].find(typeID);
        if (it != m_Components[levIndex].end())
            return it->second;
    }

    return nullptr;
}

Shared<Object> PrototypeManager::Find_Prototype(PROTOTYPE prototype,
                                                uint32 levIndex,
                                                const wstring &typeName) const 
{
    if (!Validate_Level(levIndex)) return nullptr;
    if (m_TypesByName[levIndex].contains(typeName) == false) 
    {
        LOG_ERROR(L"{}: No Class Name At Prototype", m_ObjectName);
		MSG_BOX("No Class Name At Prototype \n Check Reflection or ObjectName");
		return nullptr;
    }

    return Find_Prototype(prototype, levIndex, m_TypesByName[levIndex].at(typeName));
}

HRESULT PrototypeManager::Create_Reflection(
    const ComPtr<ID3D11Device> &device,
    const ComPtr<ID3D11DeviceContext> &context) 
{
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

    if (FAILED(prototypeManager->Initialize(reinterpret_cast<const Shared<uint32> &>(levCount)))) 
    {
        MSG_BOX("Failed To Create PrototypeManager");
        return nullptr;
    }

    return prototypeManager;
}

Bool PrototypeManager::Validate_Level(uint32 levIndex) const
{
    if (levIndex >= m_LevelCount)
    {
        LOG_ERROR(L"{}: Out Of Level", m_ObjectName);
        MSG_BOX("Out Of Level Count");
        return false;
    }

    return true;
}
