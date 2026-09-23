#pragma once
#include "Mesh.h"

NS_BEGIN(Engine)

class Shader;
class Bone;

// 본 스키닝 메시. 정점은 bind space로 남고 pre-transform은 본을 통해 적용된다.
class SkeletalMesh final : public Mesh
{
	RTTR_ENABLE(Mesh)
public:
	explicit SkeletalMesh();
	explicit SkeletalMesh(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit SkeletalMesh(const SkeletalMesh& rhs);
	~SkeletalMesh() override = default;

public:
	void Fill_BoneMatrices(const vector<Shared<Bone>>& bones); // 현재 위치에기반 월드로의 본 캡쳐
	const Matrix* Get_BoneMatrices() const { return m_BoneMatrices; }
	uint32 Get_NumMeshBones() const { return m_NumBones; }
	HRESULT Bind_BoneMatrices(const Shared<Shader>& shader, const Char* constantName, const vector<Shared<Bone>>& Bones);

protected:
	HRESULT Ready_VertexBuffer(const MODEL_MESH& meshData, const Matrix& preTransformMatrix) override;

private:
	uint32				m_NumBones{};
	vector<uint32>		m_BoneIndices;
	Matrix				m_BoneMatrices[MODEL_BONE_MAX] = {};
	vector<Matrix>		m_OffsetMatrices;

public:
	Shared<Component> Clone(void* arg = nullptr) override { return nullptr; };
};

NS_END
