#include "Object.h"
#include "SpdLogger.h"
#include "String_Helper.h"

Object::Object()
    : m_RuntimeObjectId(Create_RuntimeObjectId()) {}

Object::~Object()
{

}

HRESULT Object::Initialize_Prototype()
{
    const rttr::type runtimeType = rttr::type::get(*this);
    m_RuntimeTypeId = runtimeType.get_id();
    if (m_RuntimeTypeId == 0) {
        LOG_ERROR(L"Object Initialize Failed By RuntimeTypeId");
        MSG_BOX("Object Initialize Failed By RuntimeTypeId");
        return E_FAIL;
    }

    const string canonicalTypeKey = runtimeType.get_name().to_string();
    m_ObjectName = Helper::To_wString(canonicalTypeKey);
    if (m_ObjectName.empty()) {
        LOG_ERROR(L"GameObject Initialize Failed By Set Object Name");
        MSG_BOX("GameObject Initialize Failed By Set Object Name");
        return E_FAIL;
    }
    return S_OK;
}

void Object::Set_Active(Bool isActive)
{
    if (m_IsActive == isActive) return;

    if (isActive)
        On_Enable();
    else
        On_Disable();

    m_IsActive = isActive;
}

Bool Object::Is_Active() const
{
    return m_IsActive;
}

void Object::Assign_ReflectedIdentity(RuntimeTypeId runtimeTypeId, std::string_view registeredName)
{
	m_RuntimeTypeId = runtimeTypeId;
	m_ObjectName = Helper::To_wString(std::string{ registeredName });
}

