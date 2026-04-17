#include "Light.h"

#include "Shader.h"
#include "VIBuffer_Rect.h"

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

HRESULT Light::Render(const Shared<Shader>& shader, const Shared<VIBuffer_Rect>& buffer)
{
	uint32 shaderPass{ 0 };

	if (m_LightDesc.type == LIGHT::DIRECTIONAL)
	{
		if (FAILED(shader->Bind_RawValue(LightDirection, &m_LightDesc.direction, sizeof(m_LightDesc.direction))))
		{
			return E_FAIL;
		}

		shaderPass = ETOI(DEFERRED::DIRECTIONAL);
	}
	else if (m_LightDesc.type == LIGHT::POINT)
	{
		
		shaderPass = ETOI(DEFERRED::POINT);
	}

	shader->Begin(shaderPass);

	return buffer->Render();
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
