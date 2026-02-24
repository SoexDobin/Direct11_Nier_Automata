#pragma once
#include "EngineManager.h"

NS_BEGIN(Engine)

class Shader;

class Pipeline final : public EngineManager
{
public:
	Pipeline();
	~Pipeline() override = default;
	
public:
	Matrix Get_Transform(D3DTS transformState) const
	{
		return m_TransformStateMatrices[ETOI(transformState)];
	}
	void Set_Transform(D3DTS transformState, Matrix transformStateMatrix)
	{
		m_TransformStateMatrices[ETOI(transformState)] = transformStateMatrix;
	}
	Vector4 Get_CamTransform() const
	{
		Vector4 v4 = Vector4(m_TransformStateMatrices[ETOI(D3DTS::VIEW)].Translation());
		v4.w = 1.f;
		return v4;
	}

public:
	HRESULT Bind_TransformMatrix(const Shared<Shader>& shader, const Char* constantName, D3DTS transformState);
	HRESULT Bind_TransformMatrix_Inverse(const Shared<Shader>& shader, const Char* constantName, D3DTS transformState);
	void Update_Pipeline();

private:
	Matrix m_TransformStateMatrices[ETOI(D3DTS::END)] = {};
	Matrix m_TransformStateInverseMatrices[ETOI(D3DTS::END)] = {};

public:
	static Unique<Pipeline> Create();
};

NS_END