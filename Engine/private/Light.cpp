#include "Light.h"

#include "Shader.h"
#include "VIBuffer_Rect.h"

Light::Light()
{
}

HRESULT Light::Initialize_Prototype(const LIGHT_DESC& desc)
{
	memcpy(&m_LightDesc, &desc, sizeof(LIGHT_DESC));

	if (m_LightDesc.diffuse.x > 1.f || m_LightDesc.diffuse.y > 1.f || m_LightDesc.diffuse.z > 1.f || m_LightDesc.diffuse.w > 1.f)
	{
		m_LightDesc.diffuse = m_LightDesc.diffuse / 255.f;
	}

	if (m_LightDesc.ambient.x > 1.f || m_LightDesc.ambient.y > 1.f || m_LightDesc.ambient.z > 1.f || m_LightDesc.ambient.w > 1.f)
	{
		m_LightDesc.ambient = m_LightDesc.ambient / 255.f;
	}

	if (m_LightDesc.specular.x > 1.f || m_LightDesc.specular.y > 1.f || m_LightDesc.specular.z > 1.f || m_LightDesc.specular.w > 1.f)
	{
		m_LightDesc.specular = m_LightDesc.specular / 255.f;
	}

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
		if (FAILED(shader->Bind_RawValue(DiffuseLight, &m_LightDesc.diffuse, sizeof(m_LightDesc.diffuse)))) return E_FAIL;
		if (FAILED(shader->Bind_RawValue(AmbientLight, &m_LightDesc.ambient, sizeof(m_LightDesc.ambient)))) return E_FAIL;
		if (FAILED(shader->Bind_RawValue(SpecularLight, &m_LightDesc.specular, sizeof(m_LightDesc.specular)))) return E_FAIL;

		shaderPass = ETOI(DEFERRED::DIRECTIONAL);
	}
	else if (m_LightDesc.type == LIGHT::POINT)
	{
		if (FAILED(shader->Bind_RawValue(LightPosition, &m_LightDesc.position, sizeof(m_LightDesc.position)))) return E_FAIL;
		if (FAILED(shader->Bind_RawValue(LightRange, &m_LightDesc.range, sizeof(m_LightDesc.range)))) return E_FAIL;
		if (FAILED(shader->Bind_RawValue(DiffuseLight, &m_LightDesc.diffuse, sizeof(m_LightDesc.diffuse)))) return E_FAIL;
		if (FAILED(shader->Bind_RawValue(AmbientLight, &m_LightDesc.ambient, sizeof(m_LightDesc.ambient)))) return E_FAIL;
		if (FAILED(shader->Bind_RawValue(SpecularLight, &m_LightDesc.specular, sizeof(m_LightDesc.specular)))) return E_FAIL;
		
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
