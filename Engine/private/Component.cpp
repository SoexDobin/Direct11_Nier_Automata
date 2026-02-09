#include "Component.h"

#include "ID_Helper.h"
#include "Type_Helper.h"
#include "String_Helper.h"
#include "SpdLogger.h"

Component::Component(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& context)
	: m_Device(pDevice), m_Context(context)
{
}

Component::Component(const Shared<Component>& prototype)
	: Component(prototype->m_Device, prototype->m_Context)
{

}

HRESULT Component::Initialize_Prototype()
{
	Helper::CreateID(Helper::OBJECT_ID_TYPE, m_ObjectDesc);
	if (m_ObjectDesc.m_typeID == 0)
	{
		LOG_ERROR(L"Component Initialize Failed By Set TypeID");
		MSG_BOX("Component Initialize Failed By Set TypeID");
		return E_FAIL;
	}

	m_ObjectName = Helper::To_wString(Helper::Get_Type(this).get_name().to_string());
	if (m_ObjectName.empty())
	{
		LOG_ERROR(L"Component Initialize Failed By Set Object Name");
		MSG_BOX("Component Initialize Failed By Set Object Name");
		return E_FAIL;
	}

	return __super::Initialize_Prototype();
}

HRESULT Component::Initialize(const Shared<void>& arg)
{
	Helper::CreateID(Helper::OBJECT_ID_UNIQUE, m_ObjectDesc);
	if (m_ObjectDesc.m_objectID == 0)
	{
		LOG_ERROR(L"Component {} Initialize Failed By ObjectID", m_ObjectName);
		MSG_BOX("Component Initialize Failed By ObjectID");
		return E_FAIL;
	}

	return __super::Initialize(arg);
}
