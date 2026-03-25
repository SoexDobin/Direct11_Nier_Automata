#include "PartObject.h"

PartObject::PartObject() : GameObject{} {}
PartObject::PartObject(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) 
	: GameObject{device, context } {}
PartObject::PartObject(const PartObject& rhs)
	: GameObject{ rhs } {}
HRESULT PartObject::Initialize_Prototype()
{
	return GameObject::Initialize_Prototype();
}

HRESULT PartObject::Initialize(void* arg)
{

	GameObject::Initialize(arg);

	if (arg)
	{
		auto desc = *static_cast<PARTOBJECT_DESC*>(arg);
		m_ParentMatrix = desc.parentMatrix;
	}
	else return E_FAIL;


	return S_OK;
}