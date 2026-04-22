#include "Pipeline.h"
#include "Shader.h"

Pipeline::Pipeline()
{
}

HRESULT Pipeline::Bind_CameraPosition(const Shared<Shader>& shader, const Char* constantName)
{
	Float4 cameraPosition = Get_CamTransform();
	return shader->Bind_RawValue(constantName, &cameraPosition, sizeof(Float4));
}

HRESULT Pipeline::Bind_TransformMatrix(const Shared<Shader>& shader, const Char* constantName, D3DTS transformState)
{
	return shader->Bind_Matrix(constantName, &m_TransformStateMatrices[ETOI(transformState)]);
}

HRESULT Pipeline::Bind_TransformMatrix_Inverse(const Shared<Shader>& shader, const Char* constantName, D3DTS transformState)
{
	return shader->Bind_Matrix(constantName, &m_TransformStateInverseMatrices[ETOI(transformState)]);
}

void Pipeline::Update_Pipeline()
{
	for (uint32 i = 0; i < ETOI(D3DTS::END); ++i)
		m_TransformStateInverseMatrices[i] = m_TransformStateMatrices[i].Invert();

	auto inv = m_TransformStateInverseMatrices[ETOI(D3DTS::VIEW)].Translation();

	m_CameraPosition = Vector4{ inv.x, inv.y, inv.z, 1.f };
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
