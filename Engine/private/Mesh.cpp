#include "Mesh.h"

#include "Game.h"
#include "SkeletalMesh.h"
#include "SpdLogger.h"
#include "StaticMesh.h"

Mesh::Mesh() : VIBuffer{} {}
Mesh::Mesh(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: VIBuffer{ device, context } {}
Mesh::Mesh(const Mesh& rhs)
	: VIBuffer{ rhs } {}

HRESULT Mesh::Initialize_Prototype(const MODEL_MESH& modelMesh, const Matrix& preTransformMatrix)
{
	m_MeshName = modelMesh.name;
	m_MaterialIndex = modelMesh.materialIndex;

	m_NumVtxBuffers = 1;
	m_NumIndices = modelMesh.indices.size();
	m_IndexStride = 4;
	m_PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	if (FAILED(Ready_VertexBuffer(modelMesh, preTransformMatrix)))
	{
		LOG_ERROR(L"Failed to Create Model Buffer {}", Helper::To_wString(modelMesh.name));
		return E_FAIL;
	}

	return Ready_IndexBuffer(modelMesh);
}

HRESULT Mesh::Ready_IndexBuffer(const MODEL_MESH& modelMesh)
{
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
		delete[] indices;
		return E_FAIL;
	}

	delete[] indices;

	return S_OK;
}

HRESULT Mesh::Ready_VertexBuffer(const MODEL_MESH& /*meshData*/, const Matrix& /*preTransformMatrix*/)
{
	// 기반 Mesh는 정점 형식을 모른다. 하위 타입이 반드시 재정의한다.
	LOG_ERROR(L"Mesh has no vertex format; use StaticMesh or SkeletalMesh");
	return E_FAIL;
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

Shared<Mesh> Mesh::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, Bool isAnim, const MODEL_MESH& meshData, const Matrix& preTransformMatrix)
{
	Shared<Mesh> mesh = isAnim ?
		static_pointer_cast<Mesh>(make_shared<SkeletalMesh>(device, context)) :
		static_pointer_cast<Mesh>(make_shared<StaticMesh>(device, context));

	if (FAILED(mesh->Initialize_Prototype(meshData, preTransformMatrix)))
	{
		MSG_BOX("Failed to Created : Mesh");
		return nullptr;
	}

	return mesh;
}
