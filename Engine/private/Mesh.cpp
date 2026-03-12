#include "Mesh.h"

#include "Game.h"
#include "SpdLogger.h"

Mesh::Mesh() : VIBuffer{} {}
Mesh::Mesh(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: VIBuffer{ device, context } {}
Mesh::Mesh(const Mesh& rhs)
	: VIBuffer{ rhs } {}


HRESULT Mesh::Initialize_Prototype(Bool isAnim, const MODEL_MESH& modelMesh, const Matrix& preTransformMatrix)
{
	m_MaterialIndex = modelMesh.materialIndex;
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
		vertices[i].position = meshData.vertices[i].position;
		vertices[i].normal   = meshData.vertices[i].normal;
		vertices[i].tangent  = meshData.vertices[i].tangent;
		vertices[i].texcoord = meshData.vertices[i].texcoord;

		// Pre-Transform 적용
		XMVECTOR vPos = XMLoadFloat3(&vertices[i].position);
		vPos = XMVector3TransformCoord(vPos, preTransformMatrix);
		XMStoreFloat3(&vertices[i].position, vPos);

		XMVECTOR vNorm = XMLoadFloat3(&vertices[i].normal);
		vNorm = XMVector3TransformNormal(vNorm, preTransformMatrix);
		XMStoreFloat3(&vertices[i].normal, XMVector3Normalize(vNorm));

		XMVECTOR vTangent = XMLoadFloat3(&vertices[i].tangent);
		vTangent = XMVector3TransformNormal(vTangent, preTransformMatrix);
		XMStoreFloat3(&vertices[i].tangent, XMVector3Normalize(vTangent));
	}

	m_NumVtxBuffers = 1;

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

HRESULT Mesh::Ready_VertexBuffer_For_Anim(const MODEL_MESH& meshData)
{
	m_VtxStride = sizeof(VTXANIMMESH);
	m_NumVertices = meshData.animVertices.size();
	m_NumVtxBuffers = 1;

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

	m_NumBones = meshData.numBones;

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
