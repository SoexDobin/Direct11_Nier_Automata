#include "Component.h"
#include "ID_Helper.h"
#include "SpdLogger.h"
#include "String_Helper.h"

Component::Component()
{
}

Component::Component(const ComPtr<ID3D11Device> &pDevice, const ComPtr<ID3D11DeviceContext> &context)
    : m_Device(pDevice), m_Context(context) {}

Component::Component(const Component& prototype)
    : Component{prototype.m_Device, prototype.m_Context}
{
	m_ObjectName = prototype.m_ObjectName;
	m_DescID.m_typeID = prototype.m_DescID.m_typeID;
}

HRESULT Component::Initialize_Prototype() {
	return __super::Initialize_Prototype();
}

HRESULT Component::Initialize(void *arg) {
  Helper::CreateID(Helper::OBJECT_ID_INSTANCE, m_DescID);
  if (m_DescID.m_instanceID == 0) {
    LOG_ERROR(L"Component {} Initialize Failed By InstanceID", m_ObjectName);
    MSG_BOX("Component Initialize Failed By InstanceID");
    return E_FAIL;
  }

  return __super::Initialize(arg);
}
