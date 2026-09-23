#include "SkeletalMesh.h"

#include "Bone.h"
#include "Shader.h"
#include "SpdLogger.h"

SkeletalMesh::SkeletalMesh() : Mesh{} {}
SkeletalMesh::SkeletalMesh(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Mesh{ device, context } {}
SkeletalMesh::SkeletalMesh(const SkeletalMesh& rhs)
	: Mesh{ rhs } {}

void SkeletalMesh::Fill_BoneMatrices(const vector<Shared<Bone>>& bones)
{
	for (uint32 i = 0; i < m_NumBones; ++i)
	{
		m_BoneMatrices[i] = m_OffsetMatrices[i] * (*bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrixPtr());
	}
}

HRESULT SkeletalMesh::Bind_BoneMatrices(const Shared<Shader>& shader, const Char* constantName, const vector<Shared<Bone>>& Bones)
{
	ZeroMemory(m_BoneMatrices, sizeof(Matrix) * MODEL_BONE_MAX);

	for (uint32 i = 0; i < m_NumBones; ++i)
	{
		m_BoneMatrices[i] = m_OffsetMatrices[i] * *Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrixPtr();
	}

	return shader->Bind_Matrices(constantName, m_BoneMatrices, m_NumBones);
}

HRESULT SkeletalMesh::Ready_VertexBuffer(const MODEL_MESH& meshData, const Matrix& /*preTransformMatrix*/)
{
	m_BoneIndices = meshData.boneIndices;
	m_OffsetMatrices = meshData.offsetMatrices;
	m_NumBones = static_cast<uint32>(m_BoneIndices.size());

	m_VtxStride = sizeof(VTXANIMMESH);
	m_NumVertices = meshData.animVertices.size();

	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = m_VtxStride * m_NumVertices;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.StructureByteStride = m_VtxStride;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	VTXANIMMESH* vertices = new VTXANIMMESH[m_NumVertices];
	m_RawPosition.clear();
	m_RawPosition.reserve(m_NumVertices * 3);
	for (size_t i = 0; i < m_NumVertices; i++)
	{
		vertices[i] = meshData.animVertices[i];
		m_RawPosition.push_back(vertices[i].position.x);
		m_RawPosition.push_back(vertices[i].position.y);
		m_RawPosition.push_back(vertices[i].position.z);
	}

	D3D11_SUBRESOURCE_DATA vertexInitialData{};
	vertexInitialData.pSysMem = vertices;

	if (FAILED(m_Device->CreateBuffer(&vertexBufferDesc, &vertexInitialData, &m_VB)))
	{
		LOG_ERROR(L"Failed to Create Mesh vertices");
		MSG_BOX("Failed to Create Mesh vertices");
		delete[] vertices;
		return E_FAIL;
	}

	delete[] vertices;
	return S_OK;
}
