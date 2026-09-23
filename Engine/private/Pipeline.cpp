#include "Pipeline.h"
#include "Shader.h"

Pipeline::Pipeline()
{
}

HRESULT Pipeline::Bind_CameraPosition(const Shared<Shader>& shader, const Char* constantName)
{
	if (shader->Supports_CBuffer(ConstantBuffer::Camera))
		return Bind_CameraBuffer(shader);

	Float4 cameraPosition = Get_CamTransform();
	return shader->Bind_RawValue(constantName, &cameraPosition, sizeof(Float4));
}

HRESULT Pipeline::Bind_CameraBuffer(const Shared<Shader>& shader)
{
	if (nullptr == shader)
		return E_INVALIDARG;

	CameraCB cameraBuffer{};
	cameraBuffer.viewMatrix = m_TransformStateMatrices[ETOI(D3DTS::VIEW)];
	cameraBuffer.projMatrix = m_TransformStateMatrices[ETOI(D3DTS::PROJ)];
	cameraBuffer.viewInverseMatrix = m_TransformStateInverseMatrices[ETOI(D3DTS::VIEW)];
	cameraBuffer.projInverseMatrix = m_TransformStateInverseMatrices[ETOI(D3DTS::PROJ)];
	cameraBuffer.cameraPosition = Get_CamTransform();

	return shader->Bind_CBufferData(cameraBuffer);
}

HRESULT Pipeline::Bind_TransformMatrix(const Shared<Shader>& shader, const Char* constantName, D3DTS transformState)
{
	if (shader->Supports_CBuffer(ConstantBuffer::Camera))
		return Bind_CameraBuffer(shader);

	return shader->Bind_Matrix(constantName, &m_TransformStateMatrices[ETOI(transformState)]);
}

HRESULT Pipeline::Bind_TransformMatrix_Inverse(const Shared<Shader>& shader, const Char* constantName, D3DTS transformState)
{
	if (shader->Supports_CBuffer(ConstantBuffer::Camera))
		return Bind_CameraBuffer(shader);

	return shader->Bind_Matrix(constantName, &m_TransformStateInverseMatrices[ETOI(transformState)]);
}

void Pipeline::Update_Pipeline()
{
	for (uint32 i = 0; i < ETOI(D3DTS::END); ++i)
		m_TransformStateInverseMatrices[i] = m_TransformStateMatrices[i].Invert();

	// View마다 한 번 불리므로 프러스텀도 그 View의 카메라를 따라간다.
	// PROJ에서 카메라 공간 프러스텀을 만들고 VIEW의 역행렬(= 카메라 월드 행렬)로 월드 공간에 올린다.
	BoundingFrustum::CreateFromMatrix(m_Frustum, m_TransformStateMatrices[ETOI(D3DTS::PROJ)]);
	m_Frustum.Transform(m_Frustum, m_TransformStateInverseMatrices[ETOI(D3DTS::VIEW)]);
}

Bool Pipeline::Is_Visible(const BoundingBox& worldBounds) const
{
	return m_Frustum.Contains(worldBounds) != DISJOINT;
}

Unique<Pipeline> Pipeline::Create()
{
	auto pipeline = make_unique<Pipeline>();

	if (FAILED(pipeline->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create Pipeline");
		return nullptr;
	}

	return pipeline;
}
