#include "Model.h"

#include "Game.h"
#include "Material.h"
#include "Mesh.h"

Model::Model() : Component{} {}
Model::Model(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Component{device, context} {}

Model::Model(const Model& rhs)
	: Component{ rhs }, 
	m_NumMeshes{ rhs.m_NumMeshes }, m_Meshes{ rhs.m_Meshes },
	m_NumMaterials{ rhs.m_NumMaterials }, m_Materials{ rhs.m_Materials },
	m_Type{ rhs.m_Type }, 
	m_PreLocalTransformMatrix{ rhs.m_PreLocalTransformMatrix },
	m_AIScene{ rhs.m_AIScene } {}

HRESULT Model::Initialize_Prototype(MODEL type, const Char* modelFilePath, const Matrix& preLocalTransformMatrix)
{
	uint32 flag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast };

	if (MODEL::NONANIM == type)
		flag |= aiProcess_PreTransformVertices;

	m_AIScene = m_Importer.ReadFile(modelFilePath, flag);
	if (nullptr == m_AIScene)
		return E_FAIL;

	m_Type = type;
	m_PreLocalTransformMatrix = preLocalTransformMatrix;

	if (FAILED(Ready_Meshes()))
		return E_FAIL;

	if (FAILED(Ready_Materials(modelFilePath)))
		return E_FAIL;

	return Component::Initialize_Prototype();
}

HRESULT Model::Initialize_Prototype()
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
	m_Materials.clear();
	Component::On_Destroy();
}

HRESULT Model::Render(uint32 meshIndex)
{
	m_Meshes[meshIndex]->Bind_Resources();
	m_Meshes[meshIndex]->Render();

	return S_OK;
}

HRESULT Model::Ready_Meshes()
{
	m_NumMeshes = m_AIScene->mNumMeshes;

	for (size_t i = 0; i < m_NumMeshes; ++i)
	{
		auto mesh = Mesh::Create(m_Device, m_Context, m_AIScene->mMeshes[i], XMLoadFloat4x4(&m_PreLocalTransformMatrix));
		if (mesh == nullptr)
			return E_FAIL;

		m_Meshes.push_back(mesh);
	}

	return S_OK;
}

HRESULT Model::Ready_Materials(const Char* modelFilePath)
{
	m_NumMaterials = m_AIScene->mNumMaterials;

	for (size_t i = 0; i < m_NumMaterials; ++i)
	{
		auto material = Material::Create(m_Device, m_Context, m_AIScene->mMaterials[i], modelFilePath);
		if (material == nullptr)
			return E_FAIL;

		m_Materials.push_back(material);
	}

	return S_OK;
}

HRESULT Model::Bind_Material(const Shared<Shader>& shader, const Char* constantName, uint32 meshIndex, aiTextureType materialType, uint32 textureIndex)
{
	return m_Materials[m_Meshes[meshIndex]->Get_MaterialIndex()]->Bind_Material(shader, constantName, materialType, textureIndex);
}

Shared<Model> Model::CreatePrototype()
{
	auto model = make_shared<Model>(
		GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context());

	//if (FAILED(model->Initialize_Prototype(type, path, preLocalTransformMatrix)))
	//{
	//	MSG_BOX("Failed To Create Model");
	//	return nullptr;
	//}

	return model;
}

Shared<Model> Model::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, MODEL type, const Char* path, const Matrix& preLocalTransformMatrix)
{
	auto model = make_shared<Model>(device, context);

	if (FAILED(model->Initialize_Prototype(type, path, preLocalTransformMatrix)))
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
