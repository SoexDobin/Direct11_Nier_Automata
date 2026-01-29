#include "Object.h"
#include "ID_Helper.h"

Object::Object()
{
    Helper::CreateID(Helper::OBJECT_ID_TYPE, m_ObjectID);
    
}

Object::~Object()
{
	// ReSharper disable once CppVirtualFunctionCallInsideCtor
	OnDestroy();
    Object::OnDestroy();
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
        OnEnable();
    else
        OnDisable();

    m_IsActive = isActive;
}

Bool Object::Is_Active() const
{
    return m_IsActive;
}

