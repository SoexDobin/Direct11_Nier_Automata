#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class Mesh;
class Material;
class Shader;

class ENGINE_DLL Model final : public Component
{
	RTTR_ENABLE(Component)
public:
	explicit Model();
	explicit Model(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Model(const Model& rhs);
	~Model() override = default;

public:
	size_t Get_NumMeshes() const { return m_NumMeshes; }

public:
	COMPONENT_TYPE Get_ComponentType() const override { return COMPONENT_TYPE::MODEL; }
	HRESULT Initialize_Prototype(MODEL type, const Char* modelFilePath, const Matrix& preLocalTransformMatrix);
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;

public:
	HRESULT Render(uint32 meshIndex);
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const Char* modelFilePath);
	HRESULT Bind_Material(const Shared<Shader>& shader, const Char* constantName, uint32 meshIndex, aiTextureType materialType, uint32 textureIndex = 0);

private:
	const aiScene* m_AIScene = { nullptr };
	Assimp::Importer m_Importer{};

	MODEL m_Type{ MODEL::END };
	Matrix m_PreLocalTransformMatrix{};

private:
	uint32 m_NumMeshes = {};
	vector<Shared<Mesh>> m_Meshes;

	uint32 m_NumMaterials = {};
	vector<Shared<Material>> m_Materials;

public:
	static Shared<Model> CreatePrototype();
	static Shared<Model> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, MODEL type, const Char* path, const Matrix& preLocalTransformMatrix = XMMatrixIdentity());
	Shared<Component> Clone(void* arg = nullptr) override;
};

NS_END