#include "Object.h"
#include "ID_Helper.h"
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
        LOG_ERROR(L"GameObject Initialize Failed By Set TypeID");
        MSG_BOX("GameObject Initialize Failed By Set TypeID");
        return E_FAIL;
    }

    const string canonicalTypeKey = runtimeType.get_name().to_string();
    m_ObjectName = Helper::To_wString(canonicalTypeKey);
    if (m_ObjectName.empty()) {
        LOG_ERROR(L"GameObject Initialize Failed By Set Object Name");
        MSG_BOX("GameObject Initialize Failed By Set Object Name");
        return E_FAIL;
    }
    m_DescID.m_typeID = static_cast<uint32>(m_RuntimeTypeId);

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
	if (nullptr == object)
	{
		LOG_WARN(L"Ignored destroy request for a null object");
		return;
	}

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

void Object::Assign_ReflectedIdentity(RuntimeTypeId runtimeTypeId, std::string_view registeredName)
{
	m_RuntimeTypeId = runtimeTypeId;
	m_DescID.m_typeID = static_cast<uint32>(runtimeTypeId);
	m_ObjectName = Helper::To_wString(std::string{ registeredName });
}

