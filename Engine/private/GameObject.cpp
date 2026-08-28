#include "GameObject.h"
#include "Game.h"
#include "ID_Helper.h"
#include "Navigation.h"
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
    m_RuntimeTypeId = prototype.m_RuntimeTypeId;
    m_IsActive = prototype.m_IsActive;

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
    m_ObjectGuid = GAME_INSTANCE->Consume_RestoredObjectGuid();
    if (!m_ObjectGuid.Is_Valid())
        m_ObjectGuid = Create_ObjectGuid();
    if (!m_ObjectGuid.Is_Valid()) {
        LOG_ERROR(L"GameObject {} Initialize Failed By ObjectGuid", m_ObjectName);
        return E_FAIL;
    }

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

    if (FAILED(Add_Component(m_Transform)))
        return E_FAIL;

    return __super::Initialize(arg);
}

void GameObject::On_Destroy() {
	m_IsDestroy = true;

    for (auto &component : m_Components)
    {
        component.second->On_Destroy();
        Destroy(component.second);
    }
    m_Components.clear();

    for (auto &component : m_Scripts)
    {
        component.second->On_Destroy();
        Destroy(component.second);
    }
    m_Scripts.clear();

    m_Transform.reset();

    const vector<Shared<GameObject>> children = m_Children;
    for (const auto& child : children) {
        if (!child)
            continue;
        child->m_Parent.reset();
        child->m_StableChildKey.clear();
        child->Destroy_Subtree();
    }
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

  Object::On_Enable();
}

void GameObject::On_Disable() {
    if (!m_IsActive)
      return;

    for (auto &component : m_Components)
		component.second->Set_Active(false);

    for (auto &component : m_Scripts)
		component.second->Set_Active(false);

    Object::On_Disable();
}

void GameObject::Set_Active(Bool isActive)
{
	Object::Set_Active(isActive);
}

void GameObject::Priority_Update(Float timeDelta) {}

void GameObject::Update(Float timeDelta) {}

void GameObject::Late_Update(Float timeDelta) {}

void GameObject::Fixed_Update(Float fixedDelta) {}

HRESULT GameObject::Render() { return S_OK; }

void GameObject::Submit_RenderGroup() {}

HRESULT GameObject::Set_Parent(const Shared<GameObject>& parent) {
    return Set_Parent(parent, L"");
}

HRESULT GameObject::Set_Parent(const Shared<GameObject>& parent,
                               const wstring& stableChildKey,
                               size_t insertIndex) {
    if (!parent)
        return Remove_Parent();

    const Shared<GameObject> self = shared_from_this();
    if (parent == self)
        return E_INVALIDARG;

    for (Shared<GameObject> ancestor = parent; ancestor; ancestor = ancestor->Get_Parent()) {
        if (ancestor == self) {
            LOG_ERROR(L"Rejected cyclic GameObject parent relation");
            return E_FAIL;
        }
    }

    const Shared<GameObject> oldParent = m_Parent.lock();
    if (oldParent == parent)
        return E_FAIL;
    if (parent->Find_Child(m_ObjectGuid) ||
        (!stableChildKey.empty() && parent->Find_Child(stableChildKey)))
        return E_FAIL;

    if (oldParent && FAILED(oldParent->Remove_Child(self)))
        return E_FAIL;

    m_Parent = parent;
    m_StableChildKey = stableChildKey;
    const size_t targetIndex = min(insertIndex, parent->m_Children.size());
    parent->m_Children.insert(parent->m_Children.begin() + targetIndex, self);

    const auto markDirty = [](auto&& selfMark, const Shared<GameObject>& object) -> void {
        if (object->m_Transform)
            object->m_Transform->Set_Dirty();
        for (const auto& child : object->m_Children)
            if (child) selfMark(selfMark, child);
    };
    markDirty(markDirty, self);
    return S_OK;
}

HRESULT GameObject::Remove_Parent() {
    const Shared<GameObject> parent = m_Parent.lock();
    return parent ? parent->Remove_Child(shared_from_this()) : S_FALSE;
}

HRESULT GameObject::Add_Child(const Shared<GameObject> &child) {
    return Add_Child(child, L"");
}

HRESULT GameObject::Add_Child(const Shared<GameObject>& child,
                              const wstring& stableChildKey,
                              size_t insertIndex) {
    return child ? child->Set_Parent(shared_from_this(), stableChildKey, insertIndex) : E_INVALIDARG;
}

HRESULT GameObject::Add_Child(uint32 prototypeLevIndex, const wstring& registeredName,
                              const wstring& stableChildKey, void* arg) {
    if (registeredName.empty() || stableChildKey.empty() || !m_ObjectGuid.Is_Valid())
        return E_INVALIDARG;

    const string registeredNameUtf8 = Helper::To_String(registeredName);
    const ObjectGuid childObjectGuid = Derive_ChildObjectGuid(
        m_ObjectGuid, stableChildKey, registeredNameUtf8);
    if (!childObjectGuid.Is_Valid()) {
        LOG_ERROR(L"Failed to derive ObjectGuid for child {}", registeredName);
        return E_FAIL;
    }

    Shared<GameObject> child = GAME_INSTANCE->Instantiate_GameObject(
        registeredNameUtf8, prototypeLevIndex, arg, childObjectGuid);
    if (!child) {
        LOG_ERROR(L"Failed to create child {}", registeredName);
        return E_FAIL;
    }

    if (FAILED(Add_Child(child, stableChildKey))) {
        LOG_ERROR(L"Failed to attach child {} with StableChildKey {}", registeredName, stableChildKey);
        Object::Destroy(child);
        return E_FAIL;
    }

    return S_OK;
}

void GameObject::Post_Load()
{

    // 2. 소속 컴포넌트들의 참조 필드 해결
    for (auto& [id, comp] : m_Components)
    {
        if (!comp) continue;
        // 네비게이션 위치 후처리
        if (comp->Get_ComponentType() == COMPONENT_TYPE::NAVIGATION)
        {
            static_pointer_cast<Navigation>(comp)->Compute_CurrentCellByPosition(m_Transform->Get_Position());
        }
    }
}

HRESULT GameObject::Remove_Child(const Shared<GameObject> &child) {
    if (!child)
        return E_INVALIDARG;

    const auto it = find(m_Children.begin(), m_Children.end(), child);
    if (it == m_Children.end() || child->m_Parent.lock().get() != this)
        return E_FAIL;

    m_Children.erase(it);
    child->m_Parent.reset();
    child->m_StableChildKey.clear();

    const auto markDirty = [](auto&& selfMark, const Shared<GameObject>& object) -> void {
        if (object->m_Transform)
            object->m_Transform->Set_Dirty();
        for (const auto& nestedChild : object->m_Children)
            if (nestedChild) selfMark(selfMark, nestedChild);
    };
    markDirty(markDirty, child);
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

Shared<GameObject> GameObject::Find_Child(ObjectGuid objectGuid) const {
    if (!objectGuid.Is_Valid())
        return nullptr;

    const auto it = find_if(m_Children.begin(), m_Children.end(), [objectGuid](const Shared<GameObject>& child) {
        return child && child->Get_ObjectGuid() == objectGuid;
    });
    return it == m_Children.end() ? nullptr : *it;
}

Shared<GameObject> GameObject::Find_Child(const wstring& stableChildKey) const {
    if (stableChildKey.empty())
        return nullptr;

    const auto it = find_if(m_Children.begin(), m_Children.end(), [&stableChildKey](const Shared<GameObject>& child) {
        return child && child->Get_StableChildKey() == stableChildKey;
    });
    return it == m_Children.end() ? nullptr : *it;
}

void GameObject::Destroy_Subtree() {
    const vector<Shared<GameObject>> children = m_Children;
    for (const auto& child : children) {
        if (child)
            child->Destroy_Subtree();
    }
    Object::Destroy(shared_from_this());
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
    if (m_Scripts.empty())
        return EMPTY_VECTOR<Shared<ScriptComponent>>;

    vector<Shared<ScriptComponent>> scripts;
    for (auto& [objectID, script] : m_Scripts)
    {
        scripts.push_back(script);
    }

    return scripts;
}

Bool GameObject::Has_Component(RuntimeTypeId runtimeTypeId) const
{
    if (runtimeTypeId == 0)
        return false;

    for (const auto& [instanceID, component] : m_Components) {
        if (component && component->Get_RuntimeTypeId() == runtimeTypeId)
            return true;
    }

    for (const auto& [instanceID, script] : m_Scripts) {
        if (script && script->Get_RuntimeTypeId() == runtimeTypeId)
            return true;
    }

    return false;
}

HRESULT GameObject::Add_Component(const Shared<Component>& component) {
    if (!component || component->Get_RuntimeTypeId() == 0)
        return E_INVALIDARG;

    if (Has_Component(component->Get_RuntimeTypeId())) {
        LOG_WARN(L"Already Added Component RuntimeTypeId {}", component->Get_Name());
        return E_FAIL;
    }

    if (const Shared<GameObject> owner = component->Get_Owner(); owner && owner.get() != this) {
        LOG_ERROR(L"Component {} already belongs to another GameObject", component->Get_Name());
        return E_FAIL;
    }

    uint32 instID = component->Get_InstanceID();
    if (instID == 0 || m_Components.contains(instID) || m_Scripts.contains(instID)) {
        LOG_ERROR(L"{} has an invalid or duplicate runtime instance id: {}", component->Get_Name(), instID);
        return E_FAIL;
    }

    if (component->Get_ComponentType() == COMPONENT_TYPE::SCRIPT) {
        const auto [it, inserted] = m_Scripts.emplace(instID, static_pointer_cast<ScriptComponent>(component));
        if (!inserted)
            return E_FAIL;
    }
    else {
        const auto [it, inserted] = m_Components.emplace(instID, component);
        if (!inserted)
            return E_FAIL;
    }

    component->Set_Owner(shared_from_this());

    return S_OK;
}


Shared<Component> GameObject::Add_Component(uint32 levIndex, uint32 objectID, void* arg)
{
    Shared<Component> newComponent = GAME_INSTANCE->Instantiate<Component>(objectID, levIndex, arg);
    if (newComponent && FAILED(Add_Component(newComponent))) {
        Object::Destroy(newComponent);
        return nullptr;
    }
    else
        if (!newComponent) LOG_ERROR(L"Failed to Add Component {}", objectID);
    
    return newComponent;
}

Shared<Component> GameObject::Add_Component(uint32 levIndex, const wstring& prototypeTag, void* arg)
{
    Shared<Component> newComponent = static_pointer_cast<Component>(GAME_INSTANCE->Instantiate(prototypeTag, levIndex, arg));
    if (newComponent && FAILED(Add_Component(newComponent))) {
        Object::Destroy(newComponent);
        return nullptr;
    }
    else
        if (!newComponent) LOG_ERROR(L"Failed to Add Component {}", prototypeTag);

    return newComponent;
}
