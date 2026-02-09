#include "Object.h"

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

