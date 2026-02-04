#include "Component.h"
#include "ID_Helper.h"
#include "Type_Helper.h"
#include "String_Helper.h"

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
	if (m_ObjectDesc.typeID == 0)
	{
		MSG_BOX("Component Initialize Failed By TypeID");
		return E_FAIL;
	}

	m_ObjectName = Helper::To_wString(Helper::Get_Type(this).get_name().to_string());
	if (m_ObjectName.empty())
	{
		MSG_BOX("Component Initialize Failed By Class Name");
		return E_FAIL;
	}

	return __super::Initialize_Prototype();
}

HRESULT Component::Initialize(const Shared<void>& arg)
{
	Helper::CreateID(Helper::OBJECT_ID_UNIQUE, m_ObjectDesc);
	if (m_ObjectDesc.uniqueID == 0)
	{
		MSG_BOX("Component Initialize Failed By UniqueID");
		return E_FAIL;
	}

	return __super::Initialize(arg);
}
