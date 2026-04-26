#include "pch.h"
#include "AmusementParkLight.h"


AmusementParkLight::AmusementParkLight(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: LightGameObject{device, context} {}
AmusementParkLight::AmusementParkLight(const AmusementParkLight& rhs)
	: LightGameObject{rhs} {}

HRESULT AmusementParkLight::Initialize_Prototype()
{
	return LightGameObject::Initialize_Prototype();
}

HRESULT AmusementParkLight::Initialize(void* arg)
{
	if (arg)
	{
		auto& desc = *static_cast<AMUSEMENT_LIGHT_DESC*>(arg);
		m_Target = desc.target;
		m_Offset = desc.offset;
	}

	return LightGameObject::Initialize(arg);
}

void AmusementParkLight::On_Destroy()
{
	LightGameObject::On_Destroy();
}

void AmusementParkLight::On_Enable()
{
	LightGameObject::On_Enable();
}

void AmusementParkLight::On_Disable()
{
	LightGameObject::On_Disable();
}

void AmusementParkLight::Priority_Update(Float timeDelta)
{
	
}

void AmusementParkLight::Update(Float timeDelta)
{
		
}

void AmusementParkLight::Late_Update(Float timeDelta)
{
	if (!m_Target.expired())
	{
		
		auto targetPos = m_Target.lock()->Get_Transform()->Get_Position();
		m_Transform->Set_Position(targetPos + m_Offset);
	}
	
	LightGameObject::Late_Update(timeDelta);
}

Shared<AmusementParkLight> AmusementParkLight::Create(const ComPtr<ID3D11Device>& device,
                                                      const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<AmusementParkLight>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : AmusementParkLight");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> AmusementParkLight::Clone(void* arg)
{
	auto instance = make_shared<AmusementParkLight>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : AmusementParkLight");
		return nullptr;
	}

	return instance;
}


