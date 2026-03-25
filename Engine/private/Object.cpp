#include "Object.h"
#include "ID_Helper.h"
#include "SpdLogger.h"
#include "String_Helper.h"

Object::Object() {}

Object::~Object()
{

}

HRESULT Object::Initialize_Prototype()
{
    m_DescID.m_typeID = rttr::type::get(*this).get_id();
    if (m_DescID.m_typeID == 0) {
        LOG_ERROR(L"GameObject Initialize Failed By Set TypeID");
        MSG_BOX("GameObject Initialize Failed By Set TypeID");
        return E_FAIL;
    }

    m_ObjectName = Helper::To_wString(rttr::type::get(*this).get_name().to_string());
    if (m_ObjectName.empty()) {
        LOG_ERROR(L"GameObject Initialize Failed By Set Object Name");
        MSG_BOX("GameObject Initialize Failed By Set Object Name");
        return E_FAIL;
    }

    m_DescID.m_objectID = Helper::CreateInstanceID();
    if (m_DescID.m_objectID == 0) {
        LOG_ERROR(L"GameObject Initialize Failed By Set ObjectID");
        MSG_BOX("GameObject Initialize Failed By Set ObjectID");
        return E_FAIL;
    }

    return S_OK;
}

HRESULT Object::Initialize_Prototype(const wstring& prototypeTag)
{
    m_DescID.m_objectID = Helper::Create_FixedObjectID(prototypeTag, m_ObjectName);
    if (m_DescID.m_objectID == 0) {
        LOG_ERROR(L"GameObject Initialize Failed By Set ObjectID");
        MSG_BOX("GameObject Initialize Failed By Set ObjectID");
        return E_FAIL;
    }

    return S_OK;
}

void Object::Destroy(const Shared<Object>& object)
{
	object->m_IsDestroy = true;
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

