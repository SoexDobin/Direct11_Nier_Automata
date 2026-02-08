#include "GameObject.h"

#include "Transform.h"
#include "Game.h"

#include "ID_Helper.h"
#include "Type_Helper.h"
#include "String_Helper.h"
#include "SpdLogger.h"


GameObject::GameObject(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& context)
	: m_Device(pDevice), m_Context(context)
{

}

GameObject::GameObject(const Shared<GameObject>& prototype)
	: m_Device(prototype->m_Device), m_Context(prototype->m_Context),
	m_LayerMask(prototype->m_LayerMask), 
	m_TagMask(prototype->m_TagMask)
{
	
	
}

HRESULT GameObject::Initialize_Prototype()
{
	Helper::CreateID(Helper::OBJECT_ID_TYPE, m_ObjectDesc);
	if (m_ObjectDesc.m_typeID == 0)
	{
		LOG_ERROR(L"GameObject Initialize Failed By Set TypeID");
		MSG_BOX("GameObject Initialize Failed By Set TypeID");
		return E_FAIL;
	}

	m_ObjectName = Helper::To_wString(Helper::Get_Type(this).get_name().to_string());
	if (m_ObjectName.empty())
	{
		LOG_ERROR(L"GameObject Initialize Failed By Set Object Name");
		MSG_BOX("GameObject Initialize Failed By Set Object Name");
		return E_FAIL;
	}

	return __super::Initialize_Prototype();
}

HRESULT GameObject::Initialize(const Shared<void>& arg)
{
	Helper::CreateID(Helper::OBJECT_ID_UNIQUE, m_ObjectDesc);
	if (m_ObjectDesc.m_objectID == 0)
	{
		LOG_ERROR(L"GameObject {} Initialize Failed By ObjectID", m_ObjectName);
		MSG_BOX("GameObject Initialize Failed By ObjectID");
		return E_FAIL;
	}

	return __super::Initialize(arg);
}

void GameObject::On_Destroy()
{
	for (auto& component : m_Components)
		Destroy(component.second);
	m_Components.clear();

	for (auto& component : m_Scripts)
		Destroy(component.second);
	m_Scripts.clear();

	for (auto& child : m_Children)
		Destroy(child);
	m_Children.clear();

	if (auto parent = m_Parent.lock())
		parent->Remove_Child(shared_from_this());

	m_Parent.reset();
	Destroy(m_Transform);
	//TODO : 부모 객체 제어

	Object::On_Destroy();
}

void GameObject::On_Enable()
{
	if (m_IsActive) return;

	for (auto& component : m_Components)
		component.second->Set_Active(true);

	for (auto& component : m_Scripts)
		component.second->Set_Active(true);

	for (auto& child : m_Children)
		child->Set_Active(true);

	m_Transform->Set_Active(true);

	Object::On_Enable();
}

void GameObject::On_Disable()
{
	if (!m_IsActive) return;

	for (auto& component : m_Components)
		component.second->Set_Active(false);

	for (auto& component : m_Scripts)
		component.second->Set_Active(false);

	for (auto& child : m_Children)
		child->Set_Active(false);

	m_Transform->Set_Active(false);

	Object::On_Disable();
}

void GameObject::Priority_Update(Float timeDelta) {}

void GameObject::Update(Float timeDelta) {}

void GameObject::Late_Update(Float timeDelta) {}

void GameObject::Fixed_Update(Float fixedDelta) {}

HRESULT GameObject::Render() { return S_OK; }

HRESULT GameObject::Set_Parent(const Shared<GameObject>& parent)
{

	return S_OK;
}

HRESULT GameObject::Remove_Parent()
{

	return S_OK;
}

HRESULT GameObject::Add_Child(const Shared<GameObject>& child)
{

	return S_OK;
}

HRESULT GameObject::Remove_Child(const Shared<GameObject>& child)
{

	return S_OK;
}

Shared<GameObject> GameObject::Get_Parent() const
{
	return m_Parent.lock();
}

const vector<Shared<GameObject>>& GameObject::Get_Children() const
{
	return m_Children;
}
