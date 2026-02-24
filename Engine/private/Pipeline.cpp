#include "Pipeline.h"
#include "Shader.h"

Pipeline::Pipeline()
{
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
