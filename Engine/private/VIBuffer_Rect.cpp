#include "VIBuffer_Rect.h"

VIBuffer_Rect::VIBuffer_Rect(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
	: VIBuffer{ device, context }
{
}

VIBuffer_Rect::VIBuffer_Rect(const VIBuffer_Rect& rhs)
	: VIBuffer { rhs }
{
}

HRESULT VIBuffer_Rect::Initialize_Prototype()
{
	m_NumVtxBuffers = 1;
	m_NumVertices = 4;
	m_VtxStride = sizeof(VTXTEX);
	m_NumIndices = 6;
	m_IndexStride = 2;
	m_PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	D3D11_BUFFER_DESC vtxBufferDesc = {};
	vtxBufferDesc.ByteWidth = m_VtxStride * m_NumVertices;
	vtxBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vtxBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vtxBufferDesc.StructureByteStride = m_VtxStride;
	vtxBufferDesc.CPUAccessFlags = 0;
	vtxBufferDesc.MiscFlags = 0;

	VTXTEX* vertices = new VTXTEX[m_NumVertices];
	vertices[0].position = Float3(-0.5f, 0.5f, 0.f);
	vertices[0].texCoord = Float2(0.f, 0.f);

	vertices[1].position = Float3(0.5f, 0.5f, 0.f);
	vertices[1].texCoord = Float2(1.f, 0.f);

	vertices[2].position = Float3(0.5f, -0.5f, 0.f);
	vertices[2].texCoord = Float2(1.f, 1.f);

	vertices[3].position = Float3(-0.5f, -0.5f, 0.f);
	vertices[3].texCoord = Float2(0.f, 1.f);

	D3D11_SUBRESOURCE_DATA	vtxInitialData = {};
	vtxInitialData.pSysMem = vertices;

	if (FAILED(m_Device->CreateBuffer(&vtxBufferDesc, &vtxInitialData, m_VB.GetAddressOf())))
		return E_FAIL;

	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.ByteWidth = m_IndexStride * m_NumIndices;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.StructureByteStride = m_IndexStride;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	uShort* indices = new uShort[m_NumIndices];
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	D3D11_SUBRESOURCE_DATA	indexInitialData = {};
	indexInitialData.pSysMem = indices;

	if (FAILED(m_Device->CreateBuffer(&indexBufferDesc, &indexInitialData, m_IB.GetAddressOf())))
		return E_FAIL;

	delete[] vertices;
	delete[] indices;

	return VIBuffer::Initialize_Prototype();;
}

HRESULT VIBuffer_Rect::Initialize(const Shared<void>& arg)
{
	return VIBuffer::Initialize(arg);
}

Shared<VIBuffer_Rect> VIBuffer_Rect::Create(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
{
	auto viBufferRect = make_shared<VIBuffer_Rect>(device, context);

	if (FAILED(viBufferRect->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Rect");
	}
	return viBufferRect;
}

Shared<Component> VIBuffer_Rect::Clone(const Shared<void>& arg)
{
	auto viBufferRect = make_shared<VIBuffer_Rect>(*this);

	if (FAILED(viBufferRect->Initialize(arg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Rect");
	}

	return viBufferRect;
}
