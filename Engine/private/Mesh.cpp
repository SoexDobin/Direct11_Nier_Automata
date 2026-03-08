#include "Mesh.h"

Mesh::Mesh() : VIBuffer{} {}
Mesh::Mesh(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: VIBuffer{ device, context } {}
Mesh::Mesh(const Mesh& rhs)
	: VIBuffer{ rhs } {}

HRESULT Mesh::Initialize_Prototype(void* aiMesh)
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

Shared<Mesh> Mesh::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, void* aiMesh)
{
	auto mesh = make_shared<Mesh>(device, context);

	if (FAILED(mesh->Initialize_Prototype(aiMesh)))
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
