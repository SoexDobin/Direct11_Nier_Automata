#include "Mesh.h"

#include "Game.h"
#include "SpdLogger.h"

Mesh::Mesh() : VIBuffer{} {}
Mesh::Mesh(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: VIBuffer{ device, context } {}
Mesh::Mesh(const Mesh& rhs)
	: VIBuffer{ rhs } {}


HRESULT Mesh::Initialize_Prototype(const aiMesh* aiMesh, const Matrix& preTransformMatrix)
{
	m_MaterialIndex = aiMesh->mMaterialIndex;

	m_NumVtxBuffers = 1;
	m_NumVertices = aiMesh->mNumVertices;
	m_VtxStride = sizeof(VTXMESH);
	m_NumIndices = aiMesh->mNumFaces * 3;
	m_IndexStride = 4;
	m_PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


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
		memcpy(&vertices[i].position, &aiMesh->mVertices[i], sizeof(Float3));
		XMStoreFloat3(&vertices[i].position,
			XMVector3TransformCoord(vertices[i].position, preTransformMatrix));

		memcpy(&vertices[i].normal, &aiMesh->mNormals[i], sizeof(Float3));
		XMStoreFloat3(&vertices[i].normal,
			XMVector3TransformNormal(vertices[i].normal, preTransformMatrix));

		memcpy(&vertices[i].tangent, &aiMesh->mTangents[i], sizeof(Float3));
		XMStoreFloat3(&vertices[i].tangent,
			XMVector3TransformCoord(vertices[i].tangent, preTransformMatrix));

		memcpy(&vertices[i].texcoord, &aiMesh->mTextureCoords[0][i], sizeof(Float2));
	}

	D3D11_SUBRESOURCE_DATA vertexInitialData{};
	vertexInitialData.pSysMem = vertices;

	if (FAILED(m_Device->CreateBuffer(&vertexBufferDesc, &vertexInitialData, &m_VB)))
	{
		LOG_ERROR(L"Failed to Create Mesh vertices");
		MSG_BOX("Failed to Create Mesh vertices");
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

	for (size_t i = 0; i < aiMesh->mNumFaces; i++)
	{
		aiFace aiFace = aiMesh->mFaces[i];

		indices[numIndices++] = aiFace.mIndices[0];
		indices[numIndices++] = aiFace.mIndices[1];
		indices[numIndices++] = aiFace.mIndices[2];
	}

	D3D11_SUBRESOURCE_DATA			indexInitialData{};
	indexInitialData.pSysMem = indices;

	if (FAILED(m_Device->CreateBuffer(&IndexBufferDesc, &indexInitialData, &m_IB)))
	{
		LOG_ERROR(L"Failed to Create Mesh indices");
		MSG_BOX("Failed to Create Mesh indices");
		return E_FAIL;
	}


	delete[] vertices;
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

Shared<Mesh> Mesh::CreatePrototype()
{
	auto mesh = make_shared<Mesh>(
		GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context());

	return mesh;
}

Shared<Mesh> Mesh::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, const aiMesh* aiMesh, const Matrix& preTransformMatrix)
{
	auto mesh = make_shared<Mesh>(device, context);

	if (FAILED(mesh->Initialize_Prototype(aiMesh, preTransformMatrix)))
	{
		MSG_BOX("Failed to Created : Mesh");
		return nullptr;
	}

	return mesh;
}

Shared<Component> Mesh::Clone(void* arg)
{
	auto  mesh = make_shared<Mesh>(*this);

	if (FAILED(mesh->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : Mesh");
		return nullptr;
	}

	return mesh;
}
