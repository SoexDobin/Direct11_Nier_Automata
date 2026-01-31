#include "Object.h"
#include "ID_Helper.h"
#include "Type_Helper.h"
#include "String_Helper.h"

Object::Object()
{

}

Object::~Object()
{
	// 소멸시 자식의 On_Destroy 호출
	// ReSharper disable once CppVirtualFunctionCallInsideCtor
	On_Destroy();
    Object::On_Destroy();
}

HRESULT Object::Initialize(Shared<void> arg)
{
    Helper::CreateID(Helper::OBJECT_ID_TYPE, m_ObjectID);
    m_ObjectName = Helper::To_wString(Helper::Get_Type(this).get_name().to_string());

	return S_OK;
}

void Object::Destroy(Shared<Object> object)
{
	object->m_IsDestroy = true;
    object.reset();
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

