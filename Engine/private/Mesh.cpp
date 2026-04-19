#include "Mesh.h"

#include "Bone.h"
#include "Game.h"
#include "SpdLogger.h"

Mesh::Mesh() : VIBuffer{} {}
Mesh::Mesh(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: VIBuffer{ device, context } {}
Mesh::Mesh(const Mesh& rhs)
	: VIBuffer{ rhs } {}

void Mesh::Fill_BoneMatrices(const vector<Shared<Bone>>& bones)
{
	for (uint32 i = 0; i < m_NumBones; ++i)
	{
		m_BoneMatrices[i] = m_OffsetMatrices[i] * (*bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrixPtr());
	}
}

HRESULT Mesh::Initialize_Prototype(Bool isAnim, const MODEL_MESH& modelMesh, const Matrix& preTransformMatrix)
{
	m_MaterialIndex = modelMesh.materialIndex;
	m_BoneIndices = modelMesh.boneIndices;
	m_OffsetMatrices = modelMesh.offsetMatrices;
	m_NumBones = static_cast<uint32>(m_BoneIndices.size());

	m_NumVtxBuffers = 1;
	m_NumIndices = modelMesh.indices.size();
	m_IndexStride = 4;
	m_PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	HRESULT		hr = false == isAnim ?
				Ready_VertexBuffer_For_NonAnim(modelMesh, preTransformMatrix) :
				Ready_VertexBuffer_For_Anim(modelMesh);

	if (FAILED(hr))
	{
		LOG_ERROR(L"Failed to Create Model Buffer {}", Helper::To_wString(modelMesh.name));
		return E_FAIL;
	}

	D3D11_BUFFER_DESC	IndexBufferDesc{};
	IndexBufferDesc.ByteWidth = m_IndexStride * m_NumIndices;
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.StructureByteStride = m_IndexStride;
	IndexBufferDesc.CPUAccessFlags = 0;
	IndexBufferDesc.MiscFlags = 0;

	uint32 numIndices{};
	uint32* indices = new uint32[m_NumIndices];

	for (size_t i = 0; i < m_NumIndices; i++)
	{
		indices[numIndices++] = modelMesh.indices[i];
	}

	m_RawIndices.resize(m_NumIndices);
	for (size_t i = 0; i < m_NumIndices; ++i)
	{
		m_RawIndices[i] = static_cast<int32>(indices[i]);
	}

	D3D11_SUBRESOURCE_DATA		indexInitialData{};
	indexInitialData.pSysMem = indices;

	if (FAILED(m_Device->CreateBuffer(&IndexBufferDesc, &indexInitialData, &m_IB)))
	{
		LOG_ERROR(L"Failed to Create Mesh indices");
		MSG_BOX("Failed to Create Mesh indices");
		return E_FAIL;
	}

	delete[] indices;

	return S_OK;
}

HRESULT Mesh::Initialize_Prototype()
{
	return VIBuffer::Initialize_Prototype();
}

HRESULT Mesh::Initialize(void* arg)
{
	return VIBuffer::Initialize(arg);
}

void Mesh::On_Destroy()
{
	VIBuffer::On_Destroy();
}

HRESULT Mesh::Bind_BoneMatrices(const Shared<Shader>& shader, const Char* constantName, const vector<Shared<Bone>>& Bones)
{
	ZeroMemory(m_BoneMatrices, sizeof(Matrix) * MODEL_BONE_MAX);

	for (uint32 i = 0;  i < m_NumBones; ++i)
	{
		m_BoneMatrices[i] = m_OffsetMatrices[i] * *Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrixPtr();
	}

	return shader->Bind_Matrices(constantName, m_BoneMatrices, m_NumBones);
}

HRESULT Mesh::Ready_VertexBuffer_For_NonAnim(const MODEL_MESH& meshData, const Matrix& preTransformMatrix)
{
	m_VtxStride = sizeof(VTXMESH);
	m_NumVertices = meshData.vertices.size();

	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.ByteWidth = m_VtxStride * m_NumVertices;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.StructureByteStride = m_VtxStride;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	VTXMESH* vertices = new VTXMESH[m_NumVertices];
	for (size_t i = 0; i < m_NumVertices; i++)
	{
		Vector3 position = meshData.vertices[i].position;
		position = XMVector3TransformCoord(position, preTransformMatrix);
		vertices[i].position = position;

		Vector3 normal = meshData.vertices[i].normal;
		normal = XMVector3TransformNormal(normal, preTransformMatrix);
		vertices[i].normal= XMVector3Normalize(normal);

		Vector3 tangent = meshData.vertices[i].tangent;
		tangent = XMVector3TransformNormal(tangent, preTransformMatrix);
		vertices[i].tangent = XMVector3Normalize(tangent);

		vertices[i].texcoord = meshData.vertices[i].texcoord;
	}

	m_RawPosition.reserve(m_NumVertices * 3);
	for (uint32 i = 0; i < m_NumVertices; ++i)
	{
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

	Compute_CullingSphere();

	return S_OK;
}

HRESULT Mesh::Ready_VertexBuffer_For_Anim(const MODEL_MESH& meshData)
{
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
	for (size_t i = 0; i < m_NumVertices; i++)
	{
		vertices[i] = meshData.animVertices[i];
	}

	m_NumBones = meshData.boneIndices.size();

	m_RawPosition.reserve(m_NumVertices * 3);
	for (uint32 i = 0; i < m_NumVertices; ++i)
	{
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

	Compute_CullingSphere();

	return S_OK;
}

void Mesh::Compute_CullingSphere()
{
	BoundingSphere::CreateFromPoints(m_LocalCullingSphere, m_NumVertices, reinterpret_cast<XMFLOAT3*>(m_RawPosition.data()), sizeof(XMFLOAT3));
}

Shared<Mesh> Mesh::CreatePrototype()
{
	auto mesh = make_shared<Mesh>(
		GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context());

	return mesh;
}

Shared<Mesh> Mesh::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, Bool isAnim, const MODEL_MESH& meshData, const Matrix& preTransformMatrix)
{
	auto mesh = make_shared<Mesh>(device, context);

	if (FAILED(mesh->Initialize_Prototype(isAnim, meshData, preTransformMatrix)))
	{
		MSG_BOX("Failed to Created : Mesh");
		return nullptr;
	}

	return mesh;
}