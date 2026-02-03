#include "GameObject.h"
#include "ID_Helper.h"
#include "Type_Helper.h"
#include "String_Helper.h"

GameObject::GameObject(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& context)
	: m_Device(pDevice), m_Context(context)
{

}

GameObject::GameObject(const Shared<GameObject>& prototype)
	: GameObject(prototype->m_Device, prototype->m_Context)
{

}

HRESULT GameObject::Initialize_Prototype()
{
	Helper::CreateID(Helper::OBJECT_ID_TYPE, m_ObjectDesc);
	if (m_ObjectDesc.typeID == 0)
	{
		MSG_BOX("GameObject Initialize Failed By Set TypeID");
		return E_FAIL;
	}

	m_ObjectName = Helper::To_wString(Helper::Get_Type(this).get_name().to_string());
	if (m_ObjectName.empty())
	{
		MSG_BOX("GameObject Initialize Failed By Set Class Name");
		return E_FAIL;
	}

	return __super::Initialize_Prototype();
}

HRESULT GameObject::Initialize(Shared<void> arg)
{
	Helper::CreateID(Helper::OBJECT_ID_UNIQUE, m_ObjectDesc);
	if (m_ObjectDesc.uniqueID == 0)
	{
		MSG_BOX("Component Initialize Failed By UniqueID");
		return E_FAIL;
	}

	return __super::Initialize(arg);
}

void GameObject::Priority_Update(Float timeDelta) {}

void GameObject::Update(Float timeDelta) {}

void GameObject::Late_Update(Float timeDelta) {}

HRESULT GameObject::Render() { return S_OK; }
