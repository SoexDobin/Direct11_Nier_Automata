#pragma once
#include "Animation.h"
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
	uint32 Get_NumMeshes() const { return m_NumMeshes; }
	uint32 Get_Num_Bones() const { return m_NumBones; }
	uint32 Get_NumAnimations() const { return m_NumAnimation; }
	Bool Is_Skeletal() const { return m_IsSkeletal; }

public:
	COMPONENT_TYPE Get_ComponentType() const override { return COMPONENT_TYPE::MODEL; }
	HRESULT Initialize_Prototype(const tChar* modelFilePath, const Matrix& preLocalTransformMatrix);
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;

public:
	void Update_ModelAnimation(Float timeDelta);
	void Set_Animation(uint32 index, Float blendDuration = 0.5f, Bool isSync = false);

public:
	int32 Get_AnimationIndexByName(const wstring& name);
	const wstring& Get_AnimationNameByIndex(uint32 index);
	void Set_AnimationIndex(uint32 index) { m_CurrentAnimIndex = index; }
	uint32 Get_AnimationIndex() const { return m_CurrentAnimIndex; }
	uint32 Get_NextAnimationIndex() const { return m_NextAnimIndex; }
	void Set_AnimLoop(Bool isLoop) { m_IsAnimLoop = isLoop; }
	Bool Is_AnimLoop() const { return m_IsAnimLoop; }
	Bool Is_AnimationFinished() const { return m_IsAnimEnd; }
	Float Get_AnimationProgress() const { return m_Animations[m_CurrentAnimIndex]->Get_Progress(); }

public:
	int32 Get_BoneIndexByName(const string& boneName) const;
	TRANSFORM_FRAME Get_RootTransformDelta(uint32 nodeIndex) const;
	void Set_LocalRootNode(uint32 nodeIndex);

	HRESULT Render(uint32 meshIndex);
	HRESULT Bind_Material(const Shared<Shader>& shader, const Char* constantName, uint32 meshIndex, uint32 materialType, uint32 textureIndex = 0);
	HRESULT Bind_BoneMatrices(const Shared<Shader>& shader, const Char* constantName, uint32 meshIndex);

public:
	const wstring& Get_ModelTag() const { return m_ModelTag; }
	void Set_ModelTag(const wstring& tag);

private:
	HRESULT Ready_Bones(ifstream& in);
	HRESULT Ready_Meshes(ifstream& in, Bool isAnim);
	HRESULT Ready_Materials(ifstream& in, const std::string& directoryPath);
	HRESULT Ready_Animation(ifstream& in);

private:
	wstring	m_ModelTag{};
	Matrix	m_PreLocalTransformMatrix{};
	Bool	m_IsSkeletal{ false };
	int32	m_RootLocalNode{ -1 };

private:
	Bool	m_IsAnimLoop{ false };
	Bool	m_IsAnimEnd{ false };
	uint32	m_CurrentAnimIndex{};

private:
	Bool	m_IsBlending{ false };
	uint32	m_NextAnimIndex{};
	Float	m_BlendingElapsed{};
	Float	m_BlendingDuration{};

private:
	uint32 m_NumMeshes = {};
	vector<Shared<Mesh>> m_Meshes;
	uint32 m_NumMaterials = {};
	vector<Shared<Material>> m_Materials;
	uint32 m_NumBones = {};
	vector<Shared<Bone>> m_Bones;
	uint32 m_NumAnimation = {};
	vector<Shared<Animation>> m_Animations;
	map<wstring, uint32> m_AnimationNames;

public:
	static Shared<Model> CreatePrototype();
	static Shared<Model> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, const tChar* path, const Matrix& preLocalTransformMatrix = XMMatrixIdentity());
	Shared<Component> Clone(void* arg = nullptr) override;
};

NS_END