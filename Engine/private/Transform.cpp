#include "Transform.h"

Transform::Transform(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: Component(device, context)	
{
}

Transform::Transform(const Shared<Transform>& prototype)
	: Component(prototype)
{
}

HRESULT Transform::Initialize_Prototype()
{
	return Component::Initialize_Prototype();
}

HRESULT Transform::Initialize(Shared<void> arg)
{
	Shared<TRANSFORM_DESC> desc = static_pointer_cast<TRANSFORM_DESC>(arg);

	// desc = ???

	return Component::Initialize(arg);
}

Shared<Transform> Transform::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	auto transform = make_shared<Transform>(device, context);

	if (FAILED(transform->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create Transform Prototype");
		return nullptr;
	}

	return transform;
}

Shared<Component> Transform::Clone(Shared<void> arg)
{
	return nullptr;
}
