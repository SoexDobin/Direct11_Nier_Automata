#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class Mesh;
class Material;
class Bone;
class Animation;
class Shader;

class ENGINE_DLL Model final : public Component
{
	RTTR_ENABLE(Component)
public:
	typedef struct tagModelDesc : public COMPONENT_DESC {
		tagModelDesc(const wstring& tag) : modelTag{ tag } {}
		wstring modelTag{};
	} MODEL_DESC;

public:
	explicit Model();
	explicit Model(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Model(const Model& rhs);
	~Model() override = default;

public:
	size_t Get_NumMeshes() const { return m_NumMeshes; }

public:
	COMPONENT_TYPE Get_ComponentType() const override { return COMPONENT_TYPE::MODEL; }
	HRESULT Initialize_Prototype(const tChar* modelFilePath, const Matrix& preLocalTransformMatrix);
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;

public:
	void Update_ModelAnimation(Float timeDelta);
	HRESULT Render(uint32 meshIndex);
	HRESULT Bind_Material(const Shared<Shader>& shader, const Char* constantName, uint32 meshIndex, uint32 materialType, uint32 textureIndex = 0);
	HRESULT Bind_BoneMatrices(const Shared<Shader>& shader, const Char* constantName, uint32 meshIndex);

private:
	HRESULT Ready_Bones(ifstream& in);
	HRESULT Ready_Meshes(ifstream& in, Bool isAnim);
	HRESULT Ready_Materials(ifstream& in, const std::string& directoryPath);
	HRESULT Ready_Animation(ifstream& in);

private:
	Matrix	m_PreLocalTransformMatrix{};
	Bool	m_IsSkeletal{ false };
	Bool	m_IsAnimLoop{ false };
	uint32	m_CurrentAnimIndex{};

private:
	uint32 m_NumMeshes = {};
	vector<Shared<Mesh>> m_Meshes;
	uint32 m_NumMaterials = {};
	vector<Shared<Material>> m_Materials;
	uint32 m_NumBones = {};
	vector<Shared<Bone>> m_Bones;
	uint32 m_NumAnimation = {};
	vector<Shared<Animation>> m_Animations;

public:
	static Shared<Model> CreatePrototype();
	static Shared<Model> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, const tChar* path, const Matrix& preLocalTransformMatrix = XMMatrixIdentity());
	Shared<Component> Clone(void* arg = nullptr) override;
};

NS_END