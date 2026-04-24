#include "Shadow.h"
#include "Shader.h"

HRESULT Shadow::Add_ShadowLight(const SHADOW_LIGHT_DESC& ShadowLightDesc)
{
    XMStoreFloat4x4(&m_TransformMatrices[ETOI(D3DTS::VIEW)],
        XMMatrixLookAtLH(XMLoadFloat4(&ShadowLightDesc.eye), XMLoadFloat4(&ShadowLightDesc.at), XMVectorSet(0.f, 1.f, 0.f, 0.f)));

    XMStoreFloat4x4(&m_TransformMatrices[ETOI(D3DTS::PROJ)],
        XMMatrixPerspectiveFovLH(ShadowLightDesc.fovy, ShadowLightDesc.aspect, ShadowLightDesc.nearPlane, ShadowLightDesc.farPlane));

    return S_OK;
}

HRESULT Shadow::Bind_TransformMatrix(const Shared<Shader>& shader, const Char* constantName, D3DTS transformState) const
{
    return shader->Bind_Matrix(constantName, &m_TransformMatrices[ETOI(transformState)]);
}

Unique<Shadow> Shadow::Create()
{
	auto instance = make_unique<Shadow>();

	if (FAILED(instance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Shadow");
		return nullptr;
	}

	return instance;
}
