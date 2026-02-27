#include "VIBuffer.h"

VIBuffer::VIBuffer()
	: Component{}
{
}

VIBuffer::VIBuffer(ComPtr<ID3D11Device> device,
                   ComPtr<ID3D11DeviceContext> context)
    : Component(device, context) {}

VIBuffer::VIBuffer(const Shared<VIBuffer> &rhs)
    : Component{rhs}, m_VB{rhs->m_VB}, m_IB{rhs->m_IB},
      m_NumVtxBuffers{rhs->m_NumVtxBuffers}, m_NumVertices{rhs->m_NumVertices},
      m_VtxStride{rhs->m_VtxStride}, m_NumIndices{rhs->m_NumIndices},
      m_IndexStride{rhs->m_IndexStride}, m_PrimitiveType{rhs->m_PrimitiveType} {
}

HRESULT VIBuffer::Initialize_Prototype() {
	return Component::Initialize_Prototype();
}

HRESULT VIBuffer::Initialize(void *arg) { return Component::Initialize(arg); }


HRESULT VIBuffer::Bind_Resources() {
    ComPtr<ID3D11Buffer> vertexBuffer[] = { m_VB, };
    uint32 vtxStrides[] = { m_VtxStride, };
    uint32 offsets[] = { 0, };

    m_Context->IASetVertexBuffers(0, m_NumVtxBuffers, vertexBuffer->GetAddressOf(), vtxStrides, offsets);
    m_Context->IASetIndexBuffer(m_IB.Get(), 2 == m_IndexStride ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
    m_Context->IASetPrimitiveTopology(m_PrimitiveType);

    ComPtr<ID3D11InputLayout> inputLayout = {nullptr};

    return S_OK;
}

HRESULT VIBuffer::Render() {
    m_Context->DrawIndexed(m_NumIndices, 0, 0);

    return S_OK;
}