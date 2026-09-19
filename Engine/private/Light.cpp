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
	/* DeferredShader pass index, not the light type: pass 0 is the Debug blit. */
	uint32 shaderPass{ ETOI(DEFERRED::DIRECTIONAL) };

	if (m_LightDesc.type == LIGHT::DIRECTIONAL)
	{
		if (FAILED(shader->Bind_RawValue(LightDirection, &m_LightDesc.direction, sizeof(m_LightDesc.direction))) ||
			FAILED(shader->Bind_RawValue("g_LightDiffuse", &m_LightDesc.diffuse, sizeof(m_LightDesc.diffuse))) ||
			FAILED(shader->Bind_RawValue("g_LightAmbient", &m_LightDesc.ambient, sizeof(m_LightDesc.ambient))))
		{
			return E_FAIL;
		}

		shaderPass = ETOI(DEFERRED::DIRECTIONAL);
	}
	else if (m_LightDesc.type == LIGHT::POINT)
	{
		
		shaderPass = ETOI(DEFERRED::POINT);
	}

	if (FAILED(shader->Begin(shaderPass)))
		return E_FAIL;

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
