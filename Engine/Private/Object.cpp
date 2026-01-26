#include "Object.h"
#include "ID_Helper.h"
#include "ID_Helper.h"

Object::Object()
{
	m_ObjectID = Helper::Create_InstanceID();
	// TODO : 중복 제어
}

Object::~Object()
{
	OnDestroy();
}

void Object::Destroy(Shared<Object> object)
{
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

