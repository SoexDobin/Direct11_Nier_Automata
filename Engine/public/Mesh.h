#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class Shader;
class Bone;

class Mesh final : public VIBuffer
{
	RTTR_ENABLE(VIBuffer)
public:
	explicit Mesh();
	explicit Mesh(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Mesh(const Mesh& rhs);
	~Mesh() override = default;

public:
	COMPONENT_TYPE Get_ComponentType() const override { return COMPONENT_TYPE::MESH; }
	HRESULT Initialize_Prototype(Bool isAnim, const MODEL_MESH& meshData, const Matrix& preTransformMatrix);
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg = nullptr) override;
	void On_Destroy() override;

public:
	uint32 Get_MaterialIndex() const { return m_MaterialIndex; }
	HRESULT Bind_BoneMatrices(const Shared<Shader>& shader, const Char* constantName, const vector<Shared<Bone>>& Bones);

private:
	HRESULT Ready_VertexBuffer_For_NonAnim(const MODEL_MESH& meshData, const Matrix& preTransformMatrix);
	HRESULT Ready_VertexBuffer_For_Anim(const MODEL_MESH& meshData);

private:
	uint32				m_MaterialIndex{};
	uint32				m_NumBones{};
	vector<uint32>		m_BoneIndices;
	Matrix				m_BoneMatrices[512] = {};
	vector<Matrix>		m_OffsetMatrices;

public:
	static Shared<Mesh> CreatePrototype();
	static Shared<Mesh> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, Bool isAnim, const MODEL_MESH& meshData, const Matrix& preTransformMatrix);
	Shared<Component> Clone(void* arg = nullptr) override { return nullptr; };
};

NS_END