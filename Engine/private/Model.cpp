#include "Model.h"
#include "Mesh.h"

Model::Model() : Component{} {}
Model::Model(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Component{device, context} {}

Model::Model(const Model& rhs)
	: Component{ rhs }, m_NumMeshes{ rhs.m_NumMeshes }, m_Meshes{ rhs.m_Meshes } {}

HRESULT Model::Initialize_Prototype(const wstring& path)
{
	return Component::Initialize_Prototype();
}

HRESULT Model::Initialize(void* arg)
{
	return Component::Initialize(arg);
}

void Model::On_Destroy()
{
	m_Meshes.clear();
	Component::On_Destroy();
}

HRESULT Model::Render()
{
	for (auto& mesh : m_Meshes)
	{
		if (FAILED(mesh->Render()))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT Model::Ready_Meshes()
{
	// TODO : Assimp로 모델 로드 후 Mesh 생성하는 방식은 어떻게 버리지
	// m_NumMeshes = m_pAIScene->mNumMeshes;

	for (uint32 i = 0; i < m_NumMeshes; ++i)
	{
		auto mesh = Mesh::Create(m_Device, m_Context, nullptr);
		if (mesh == nullptr)
			return E_FAIL;

		m_Meshes.push_back(mesh);
	}

	return S_OK;
}

Shared<Model> Model::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, const wstring& path)
{
	auto model = make_shared<Model>(device, context);

	if (FAILED(model->Initialize_Prototype(path)))
	{
		MSG_BOX("Failed To Create Model");
		return nullptr;
	}

	return model;
}

Shared<Component> Model::Clone(void* arg)
{
	auto model = make_shared<Model>(*this);

	if (FAILED(model->Initialize(arg)))
	{
		MSG_BOX("Failed To Clone Model");
		return nullptr;
	}

	return model;
}
