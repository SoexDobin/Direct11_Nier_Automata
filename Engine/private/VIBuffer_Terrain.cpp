#include "VIBuffer_Terrain.h"

VIBuffer_Terrain::VIBuffer_Terrain()
	: VIBuffer{}
{
}

VIBuffer_Terrain::VIBuffer_Terrain(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: VIBuffer { device, context }
{
}

VIBuffer_Terrain::VIBuffer_Terrain(const Shared<VIBuffer_Terrain>& rhs)
	: VIBuffer { rhs }, m_NumVerticesX{rhs->m_NumVerticesX}, m_NumVerticesZ{rhs->m_NumVerticesZ}
{
}

VIBuffer_Terrain::~VIBuffer_Terrain()
{
}

HRESULT VIBuffer_Terrain::Initialize_Prototype()
{
	return VIBuffer::Initialize_Prototype();
}

HRESULT VIBuffer_Terrain::Initialize(void* arg)
{
	return VIBuffer::Initialize(arg);
}

Shared<VIBuffer_Terrain> VIBuffer_Terrain::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto bufferTerrain = make_shared<VIBuffer_Terrain>(device, context);

	if (FAILED(bufferTerrain->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Create VIBuffer_Terrain");
		return nullptr;
	}

	return bufferTerrain;
}

Shared<Component> VIBuffer_Terrain::Clone(void* arg)
{
	auto bufferTerrain = make_shared<VIBuffer_Terrain>(*this);

	if (FAILED(bufferTerrain->Initialize(arg)))
	{
		MSG_BOX("Failed To Clone VIBuffer_Terrain");
		return nullptr;
	}

	return bufferTerrain;
}
