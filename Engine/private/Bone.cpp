#include "Bone.h"

#include "SpdLogger.h"

Bone::Bone() : Component{}
{
}

Bone::Bone(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Component{device, context}
{
}

Bone::Bone(const Bone& rhs)
	: Component{ rhs }
{
}

void Bone::On_Destroy()
{
	Component::On_Destroy();
}

void Bone::On_Disable()
{
	Component::On_Disable();
}

void Bone::On_Enable()
{
	Component::On_Enable();
}

void Bone::Set_Active(Bool isActive)
{
	Component::Set_Active(isActive);
}

HRESULT Bone::Initialize_Prototype(const MODEL_BONE& modelBone)
{
	return Component::Initialize_Prototype();
}

HRESULT Bone::Initialize_Prototype()
{
	return Component::Initialize_Prototype();
}

HRESULT Bone::Initialize(void* arg)
{
	return Component::Initialize(arg);
}

Shared<Bone> Bone::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, const MODEL_BONE& boneData)
{
	auto bone = make_shared<Bone>(device, context);

	if (FAILED(bone->Initialize_Prototype(boneData)))
	{
		LOG_ERROR(L"Failed to Created : Bone Path");
		MSG_BOX("Failed to Created : Bone");
	}
	return bone;
}

Shared<Component> Bone::Clone(void* arg)
{
	return nullptr;
}
