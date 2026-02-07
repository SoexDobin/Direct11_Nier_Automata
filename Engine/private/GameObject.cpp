#include "GameObject.h"
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

void GameObject::Priority_Update(Float timeDelta) {}

void GameObject::Update(Float timeDelta) {}

void GameObject::Late_Update(Float timeDelta) {}

void GameObject::Fixed_Update(Float fixedDelta) {}

HRESULT GameObject::Render() { return S_OK; }
