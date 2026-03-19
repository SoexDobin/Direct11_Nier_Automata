#include "GameObject.h"
#include "Game.h"
#include "ID_Helper.h"
#include "ScriptComponent.h"
#include "SpdLogger.h"
#include "Transform.h"

GameObject::GameObject() {}

GameObject::GameObject(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
    : m_Device(device), m_Context(context) {}
GameObject::GameObject(const GameObject& prototype)
    : m_Device(prototype.m_Device), m_Context(prototype.m_Context),
      m_LayerMask(prototype.m_LayerMask), m_TagMask(prototype.m_TagMask) 
{
    m_ObjectName = prototype.m_ObjectName;
    m_DescID.m_typeID = prototype.m_DescID.m_typeID;
    m_DescID.m_objectID = prototype.m_DescID.m_objectID;

    // TODO : Clone 시점에 부모 자식 관계는 어떻게 할 것인지 고민 필요
    // TODO : Prototype의 자식들은 어떻게 할 것인지 고민 필요
    // TODO : 이전 속성, 상태들은 어떻게 할거인지 고민 필요

	m_Transform = Get_Component<Transform>();
}

HRESULT GameObject::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT GameObject::Initialize(void *arg) {
    Helper::CreateID(Helper::OBJECT_ID_INSTANCE, m_DescID);
    if (m_DescID.m_instanceID == 0) {
        LOG_ERROR(L"Component {} Initialize Failed By InstanceID", m_ObjectName);
        MSG_BOX("Component Initialize Failed By InstanceID");
        return E_FAIL;
    }

    m_Transform = Transform::Create(m_Device, m_Context);
    if (nullptr == m_Transform)
		return E_FAIL;

    if (FAILED(m_Transform->Initialize(nullptr)))
		return E_FAIL;

    m_Transform->Set_Owner(shared_from_this());

    if (Get_Component<Transform>() == nullptr)
		m_Components.emplace(ETOI(COMPONENT_TYPE::TRANSFORM), m_Transform);

    return __super::Initialize(arg);
}

void GameObject::On_Destroy() {
	m_IsDestroy = true;

    for (auto &component : m_Components)
		Destroy(component.second);
    m_Components.clear();

    for (auto &component : m_Scripts)
		Destroy(component.second);
    m_Scripts.clear();

    Destroy(m_Transform);

    for (auto &child : m_Children)
		Destroy(child);
    m_Children.clear();

    if (auto parent = m_Parent.lock())
		parent->Remove_Child(shared_from_this());
    m_Parent.reset();

    Object::On_Destroy();
}

void GameObject::On_Enable() {
  if (m_IsActive)
    return;

  for (auto &component : m_Components)
    component.second->Set_Active(true);

  for (auto &component : m_Scripts)
    component.second->Set_Active(true);

  m_Transform->Set_Active(true);

  for (auto &child : m_Children)
    child->Set_Active(true);

  Object::On_Enable();
}

void GameObject::On_Disable() {
  if (!m_IsActive)
    return;

  for (auto &component : m_Components)
    component.second->Set_Active(false);

  for (auto &component : m_Scripts)
    component.second->Set_Active(false);

  m_Transform->Set_Active(false);

  for (auto &child : m_Children)
    child->Set_Active(false);

  Object::On_Disable();
}

void GameObject::Set_Active(Bool isActive)
{
	__super::Set_Active(isActive);
}

void GameObject::Priority_Update(Float timeDelta) {}

void GameObject::Update(Float timeDelta) {}

void GameObject::Late_Update(Float timeDelta) {}

void GameObject::Fixed_Update(Float fixedDelta) {}

HRESULT GameObject::Render() { return S_OK; }

void GameObject::Submit_RenderGroup() {}

HRESULT GameObject::Set_Parent(const Shared<GameObject> &parent) {

  if (!m_Parent.expired()) {
    auto oldParent = m_Parent.lock();
    if (oldParent == parent) {
      LOG_WARN(L"Already Regist Parent {}", oldParent->Get_Name());
      return S_OK;
    }

    oldParent->Remove_Child(shared_from_this());
    oldParent.reset();
  }

  m_Parent = parent;
  if (parent) {
    if (FAILED(parent->Add_Child(shared_from_this())))
      return E_FAIL;
  }

  m_Transform->Set_Dirty();

  return S_OK;
}

HRESULT GameObject::Remove_Parent() {
  m_Transform->Set_Dirty();
  return Set_Parent(nullptr);
}

HRESULT GameObject::Add_Child(const Shared<GameObject> &child) {
  if (!child)
    return E_FAIL;

  for (auto &registeredChild : m_Children)
    if (registeredChild == child)
      return S_OK;

  m_Children.push_back(child);
  m_Transform->Set_Dirty();

  if (child->Get_Parent() != shared_from_this()) {
    child->Set_Parent(shared_from_this());
  }
  return S_OK;
}

void GameObject::Post_Load(const unordered_map<uint32, Shared<GameObject>>& instanceMap)
{
	// 1. 자기 자신(GameObject)의 RTTR 프로퍼티 중 참조 필드 해결
	rttr::type type = rttr::type::get(*this);
	for (auto& prop : type.get_properties())
	{
		if (prop.get_metadata("SaveData") == "GameObject")
		{
			rttr::variant var = prop.get_value(*this);
			uint32 targetObjectID = 0;

			// 파일에서 로드된 ObjectID 추출
			if (var.is_type<uint32>()) targetObjectID = var.get_value<uint32>();
			else if (var.is_type<int>()) targetObjectID = static_cast<uint32>(var.get_value<int>());

			if (targetObjectID != 0)
			{
				auto it = instanceMap.find(targetObjectID);
				if (it != instanceMap.end())
				{
					rttr::type propType = prop.get_type();
					// A. SharedPtr을 기대하는 경우
					if (propType == rttr::type::get<Shared<GameObject>>())
					{
						prop.set_value(*this, it->second);
					}
					// B. InstanceID(런타임 ID)를 기대하는 경우 (Camera 타겟 등)
					else if (propType == rttr::type::get<uint32>() || propType == rttr::type::get<int>())
					{
						prop.set_value(*this, targetObjectID);
					}
					
					LOG_INFO(L"Resolved Reference: {} -> {} (ObjectID: {})", Get_Name(), it->second->Get_Name(), targetObjectID);
				}
			}
		}
	}

	// 2. 소속 컴포넌트들의 참조 필드 해결
    for (auto& [id, comp] : m_Components)
    {
        if (!comp) continue;

        rttr::type compType = rttr::type::get(*comp);
        for (auto& prop : compType.get_properties())
        {
            if (prop.get_metadata("SaveData") == "GameObject")
            {
                rttr::variant var = prop.get_value(*comp);
                uint32 targetObjectID = 0;

                if (var.is_type<uint32>()) targetObjectID = var.get_value<uint32>();
                else if (var.is_type<int>()) targetObjectID = static_cast<uint32>(var.get_value<int>());

                if (targetObjectID != 0)
                {
                    auto it = instanceMap.find(targetObjectID);
                    if (it != instanceMap.end())
                    {
                        rttr::type propType = prop.get_type();
                        if (propType == rttr::type::get<Shared<GameObject>>())
                        {
                            prop.set_value(*comp, it->second);
                        }
                        else if (propType == rttr::type::get<uint32>() || propType == rttr::type::get<int>())
                        {
                            prop.set_value(*comp, targetObjectID);
                        }

                    }
                }
            }
        }
    }
}

HRESULT GameObject::Remove_Child(const Shared<GameObject> &child) {
  if (m_IsDestroy)
    return S_OK;

  auto it = std::find(m_Children.begin(), m_Children.end(), child);
  if (it == m_Children.end())
    return E_FAIL;

  m_Children.erase(it);
  return S_OK;
}

Shared<GameObject> GameObject::Get_Parent() const {
  if (m_Parent.expired())
    return nullptr;
  return m_Parent.lock();
}

const vector<Shared<GameObject>> &GameObject::Get_Children() const {
  return m_Children;
}

Shared<Component> GameObject::Get_Component(uint32 objectID) {
  if (m_Scripts.contains(objectID)) {
    return m_Scripts[objectID];
  }

  for (const auto &component : m_Components) {
    if (component.second->Get_ObjectID() == objectID)
      return component.second;
  }

  return nullptr;
}

vector<Shared<Component>> GameObject::Get_Components()
{
    if (m_Components.empty())
        return EMPTY_VECTOR<Shared<Component>>;

    vector<Shared<Component>> components;
    for (auto& [objectID, component]: m_Components)
    {
        components.push_back(component);
    }

    return components;
}

vector<Shared<ScriptComponent>> GameObject::Get_Scripts()
{
    if (m_Components.empty())
        return EMPTY_VECTOR<Shared<ScriptComponent>>;

    vector<Shared<ScriptComponent>> scripts;
    for (auto& [objectID, script] : m_Scripts)
    {
        scripts.push_back(script);
    }

    return scripts;
}

HRESULT GameObject::Add_Component(const Shared<Component>& component) {
    if (!component) return E_FAIL;

    uint32 instID = component->Get_InstanceID();
    uint32 objectID = component->Get_ObjectID();
    uint32 typeID = component->Get_TypeID();

    if (component->Get_ComponentType() == COMPONENT_TYPE::SCRIPT) {
        for (auto& [instanceID, script] : m_Scripts) {
            if (objectID == script->Get_ObjectID())
            {
                LOG_WARN(L"Already Added Script {}", component->Get_Name());
                return E_FAIL;
            }
            if (typeID == script->Get_TypeID())
            {
                LOG_WARN(L"Already Added Script By TypeID {}", component->Get_Name());
                return E_FAIL;
            }
        }

        if (!m_Scripts.contains(instID)) {
            m_Scripts.emplace(instID, static_pointer_cast<ScriptComponent>(component));
        }
        else LOG_ERROR(L"{} is same id value : {} ", component->Get_Name(), instID);
    }
    else {
        for (auto& [instanceID, comp] : m_Components) {
            if (objectID == comp->Get_ObjectID())
            {
                LOG_WARN(L"Already Added Component By ObjectID {}", component->Get_Name());
                return E_FAIL;
            }
            if (typeID == comp->Get_TypeID())
            {
                LOG_WARN(L"Already Added Component By TypeID {}", component->Get_Name());
                return E_FAIL;
            }
        }

        if (!m_Components.contains(instID)) {
            m_Components.emplace(instID, component);
        }
        else LOG_ERROR(L"{} is same id value : {} ", component->Get_Name(), instID);
    }

    component->Set_Owner(shared_from_this());
    return S_OK;
}


Shared<Component> GameObject::Add_Component(uint32 objectID, void* arg)
{
    uint32 levIndex = GAME_INSTANCE->Get_CurrentLevelIndex();

    Shared<Component> newComponent = GAME_INSTANCE->Instantiate<Component>(objectID, levIndex, arg);
    if (newComponent) 
        Add_Component(newComponent);
    else
        LOG_ERROR(L"Failed to Add Component {}", objectID);
    
    return newComponent;
}

Shared<Component> GameObject::Add_Component(const wstring& prototypeTag, void* arg)
{
    uint32 levIndex = GAME_INSTANCE->Get_CurrentLevelIndex();

    Shared<Component> newComponent = static_pointer_cast<Component>(GAME_INSTANCE->Instantiate(prototypeTag, levIndex, arg));
    if (newComponent)
        Add_Component(newComponent);
    else
        LOG_ERROR(L"Failed to Add Component {}", prototypeTag);

    return newComponent;
}
