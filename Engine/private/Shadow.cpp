#include "Shadow.h"
#include "Shader.h"
#include "Transform.h"

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

HRESULT Shadow::Update_ShadowLight(const Vector4 &LightDirection)
{
	Vector3 TargetPos = Vector3(0.f, 0.f, 0.f);
	if (m_ShadowTarget != nullptr)
	{
		TargetPos = m_ShadowTarget->Get_Position();
	}

	XMVECTOR TargetPosVec = XMLoadFloat3(&TargetPos);
	XMVECTOR LightDirVec = XMVector3Normalize(XMLoadFloat4(&LightDirection));

	Float ShadowDistance = 100.0f; 
	XMVECTOR EyePos = TargetPosVec - (LightDirVec * ShadowDistance);

	XMStoreFloat4x4(&m_TransformMatrices[ETOI(D3DTS::VIEW)],
		XMMatrixLookAtLH(EyePos, TargetPosVec, XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	// 직교 투영(Orthographic)을 사용하면 w가 1.0으로 고정되므로 그림자가 렌더링되지 않았던 것입니다.
	// 따라서 다시 PerspectiveFovLH로 돌려놓습니다.
	// FOV를 45도에서 25도로 줄여 타겟 주변의 해상도를 높입니다.
	XMStoreFloat4x4(&m_TransformMatrices[ETOI(D3DTS::PROJ)],
		XMMatrixPerspectiveFovLH(XMConvertToRadians(35.f), 1.0f, 0.1f, 1000.0f));

	return S_OK;
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
