#include "Light.h"

Light::Light()
{
}

HRESULT Light::Initialize_Prototype(const LIGHT_DESC& desc)
{
	memcpy(&m_LightDesc, &desc, sizeof(LIGHT_DESC));
	return S_OK;
}

HRESULT Light::Initialize(void* arg)
{
	return Object::Initialize(arg);
}

Shared<Light> Light::Create(const LIGHT_DESC& desc)
{
	auto light = make_shared<Light>();

	if (FAILED(light->Initialize_Prototype(desc)))
	{
		MSG_BOX("Failed to Created : Light");
		return nullptr;
	}
	if (FAILED(light->Initialize(nullptr)))
		{
		MSG_BOX("Failed to Created : Light");
		return nullptr;
	}

	return light;
}
