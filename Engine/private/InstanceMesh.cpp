#include "InstanceMesh.h"

#include "SpdLogger.h"

InstanceMesh::InstanceMesh() : StaticMesh{} {}
InstanceMesh::InstanceMesh(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: StaticMesh{ device, context } {}
InstanceMesh::InstanceMesh(const InstanceMesh& rhs)
	: StaticMesh{ rhs }
	, m_InstanceStride{ rhs.m_InstanceStride }
{}

HRESULT InstanceMesh::Set_Instances(const vector<Matrix>& worldMatrices)
{
	m_NumInstances = static_cast<uint32>(worldMatrices.size());
	if (0 == m_NumInstances)
		return S_OK;

	if (m_NumInstances > m_InstanceCapacity)
	{
		D3D11_BUFFER_DESC instanceBufferDesc{};
		instanceBufferDesc.ByteWidth = m_InstanceStride * m_NumInstances;
		instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		instanceBufferDesc.StructureByteStride = m_InstanceStride;
		instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		instanceBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA instanceInitialData{};
		instanceInitialData.pSysMem = worldMatrices.data();

		m_VBInstance.Reset();
		if (FAILED(m_Device->CreateBuffer(&instanceBufferDesc, &instanceInitialData, &m_VBInstance)))
		{
			LOG_ERROR(L"Failed to Create InstanceMesh instances {}", Helper::To_wString(m_MeshName));
			m_NumInstances = 0;
			return E_FAIL;
		}

		m_InstanceCapacity = m_NumInstances;
		m_NumVtxBuffers = 2;
		return S_OK;
	}

	D3D11_MAPPED_SUBRESOURCE mapped{};
	if (FAILED(m_Context->Map(m_VBInstance.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
	{
		LOG_ERROR(L"Failed to Map InstanceMesh instances {}", Helper::To_wString(m_MeshName));
		return E_FAIL;
	}

	memcpy(mapped.pData, worldMatrices.data(), static_cast<size_t>(m_InstanceStride) * m_NumInstances);
	m_Context->Unmap(m_VBInstance.Get(), 0);

	return S_OK;
}

HRESULT InstanceMesh::Bind_Resources()
{
	if (!m_VBInstance)
		return StaticMesh::Bind_Resources();

	ID3D11Buffer* buffers[] = { m_VB.Get(), m_VBInstance.Get() };
	uint32 strides[] = { m_VtxStride, m_InstanceStride };
	uint32 offsets[] = { 0, 0 };

	m_Context->IASetVertexBuffers(0, m_NumVtxBuffers, buffers, strides, offsets);

	if (m_IB)
	{
		DXGI_FORMAT fmt = m_IndexStride == 2 ?
			DXGI_FORMAT_R16_UINT :
			DXGI_FORMAT_R32_UINT;
		m_Context->IASetIndexBuffer(m_IB.Get(), fmt, 0);
	}

	m_Context->IASetPrimitiveTopology(m_PrimitiveType);
	return S_OK;
}

HRESULT InstanceMesh::Render()
{
	if (0 == m_NumInstances || !m_VBInstance)
		return S_OK;

	m_Context->DrawIndexedInstanced(m_NumIndices, m_NumInstances, 0, 0, 0);
	Count_Draw(m_NumIndices, m_NumInstances);

	return S_OK;
}

void InstanceMesh::On_Destroy()
{
	m_VBInstance.Reset();
	m_InstanceCapacity = 0;
	m_NumInstances = 0;
	StaticMesh::On_Destroy();
}
