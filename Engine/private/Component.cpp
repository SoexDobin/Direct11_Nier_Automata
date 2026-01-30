#include "Component.h"

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
	return S_OK;
}

HRESULT Component::Initialize(Shared<void> arg)
{
	return S_OK;
}
