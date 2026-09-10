#pragma once
#include "Animation.h"
#include "AnimationTracker.h"
#include "Component.h"
#include "Engine_Reflection.h"

NS_BEGIN(Engine)
class Mesh;
class Material;
class Bone;
class Animation;
class Shader;
class AnimationTracker;

struct MATERIAL_TEXTURE_SETTING
{
	string slotName{};
	string resourcePath{};
};

struct MODEL_MATERIAL_SETTING
{
	uint32 materialIndex{};
	string sourceMaterialName{};
	string sourceShaderName{};
	string sourceTechniqueName{};
	string passName{ "Default_Pass" };
	Bool isBlend{ false };
	vector<MATERIAL_TEXTURE_SETTING> textures{};
};

struct MODEL_MATERIAL_SETTINGS
{
	uint32 schemaVersion{ 1 };
	string modelTag{};
	string modelPath{};
	vector<MODEL_MATERIAL_SETTING> materials{};
private:
	friend class Model;
	// Runtime-only preparation; never serialized into the authored document.
	vector<Shared<const Material>> preparedMaterials{};
	vector<string> textureSlots{};
};

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
	void Set_Animation(uint32 index, Float blendDuration = 0.5f);
	HRESULT Load_Animations(const vector<wstring>& animationFilePaths);
	HRESULT Apply_AnimationPreset(const AnimationPresetSnapshot& preset);

public:
	Bool Is_Blending() const { return m_IsBlending; }
	int32 Get_AnimationIndexByName(const wstring& name);
	const wstring& Get_AnimationNameByIndex(uint32 index);
	void Set_AnimationIndex(uint32 index) { m_CurrentAnimIndex = index; }
	uint32 Get_AnimationIndex() const { return m_CurrentAnimIndex; }
	uint32 Get_NextAnimationIndex() const { return m_NextAnimIndex; }
	void Set_AnimLoop(Bool isLoop) { m_IsAnimLoop = isLoop; }
	Bool Is_AnimLoop() const { return m_IsAnimLoop; }
	Bool Is_AnimationFinished() const { return m_IsAnimEnd; }
	Float Get_AnimationProgress() const {
		if (m_Animations.empty())
			return 0.f;
		if (m_IsBlending)
			return m_Animations[m_NextAnimIndex]->Get_Progress();
		return m_Animations[m_CurrentAnimIndex]->Get_Progress();
	}

public: /* Animation Tracker */
	void Add_AnimNotify(uint32 animIndex, const AnimationTracker::ANIMATION_NOTIFY& notify);
	void Add_AnimNotify(uint32 animIndex, std::initializer_list<AnimationTracker::ANIMATION_NOTIFY> notifies);
	void Clear_AnimNotifies();
	Bool Is_NotifyActive(uint32 animIndex, const wstring & notifyTag) const;
	Bool Is_NotifyActive(const wstring & notifyName) const;

public: /* Mesh Info */
	HRESULT Get_MeshMaterialInfo(uint32 meshIndex, string& meshName, uint32& materialIndex) const;
	void Extract_RawMeshData(_Out_ vector<Float>& outPositions, _Out_ vector<int32>& outIndices) const;

public: /* snap shot */
	vector<BONE_SNAPSHOT> Get_SnapShot_BoneMatrices();

public:
	int32 Get_BoneIndexByName(const string& boneName) const;
	Matrix Get_BoneMatrix(uint32 boneIndex) const; // 특정 부모 뼈대의 트랜스폼 가져오기
	const TRANSFORM_FRAME& Get_RootTransformVelocity(uint32 nodeIndex);
	void Set_LocalRootNode(uint32 nodeIndex);

	HRESULT Render(uint32 meshIndex);
	HRESULT Bind_Material(const Shared<Shader>& shader, const Char* constantName, uint32 meshIndex, uint32 materialType, uint32 textureIndex = 0);
	HRESULT Bind_BoneMatrices(const Shared<Shader>& shader, const Char* constantName, uint32 meshIndex);
	HRESULT Validate_MaterialBindings(const Shared<Shader>& shader) const;
	HRESULT BindAndBeginMaterial(const Shared<Shader>& shader, uint32 meshIndex) const;

public:
	const wstring& Get_ModelTag() const { return m_ModelTag; }
	const string& Get_ModelResourcePath() const { return m_ModelResourcePath; }
	const wstring& Get_ModelFilePath() const { return m_ModelFilePath; }
	Shared<const MODEL_MATERIAL_SETTINGS> Get_MaterialSettings() const { return m_MaterialSettings; }
	vector<string> Get_MaterialNames() const;
	void Set_ModelTag(const wstring& tag);
	const AnimationPresetSnapshot& Get_AnimationPreset() const { return m_AnimationPreset; }
	void Set_AnimationPreset(const AnimationPresetSnapshot& preset) { m_AnimationPreset = preset; }
	HRESULT Validate_MaterialSettings(const filesystem::path& settingsPath, const wstring& expectedModelTag) const;
	HRESULT Load_MaterialSettings(const filesystem::path& settingsPath, const wstring& expectedModelTag,
		const Shared<Shader>& shader = nullptr);
	HRESULT Prepare_MaterialSettings(const filesystem::path& settingsPath, const wstring& expectedModelTag,
		Shared<const MODEL_MATERIAL_SETTINGS>& prepared, const Shared<Shader>& shader = nullptr) const;
	HRESULT Apply_MaterialSettings(const Shared<const MODEL_MATERIAL_SETTINGS>& prepared);
	static wstring Make_MaterialSettingsFileName(const wstring& modelTag);

private:
	HRESULT Post_Load() override;
	HRESULT Ready_Bones(ifstream& in);
	HRESULT Ready_Meshes(ifstream& in, Bool isAnim);
	HRESULT Ready_Materials(ifstream& in, const std::string& directoryPath);
	HRESULT Ready_Animation(ifstream& in);
	HRESULT Read_AnimationData(ifstream& in, MODEL_ANIMATION& animationData,
		Bool hasStoredChannelCount, uint32 channelCount = 0) const;
	HRESULT Build_Animations(const vector<wstring>& animationFilePaths,
		const unordered_set<wstring>& existingNames,
		vector<Shared<Animation>>& outAnimations, vector<wstring>& outNames) const;
	Bool Validate_AnimationPreset(const AnimationPresetSnapshot& preset) const;
	Bool Validate_AnimationRequests(size_t clipCount) const;
	void Bind_AnimNotify(uint32 animIndex, const AnimationTracker::ANIMATION_NOTIFY& notify) const;
	void Restore_AnimationRequests();
	HRESULT Validate_MaterialBindings(const Shared<Shader>& shader,
		const Shared<const MODEL_MATERIAL_SETTINGS>& settings) const;

private:
	TRANSFORM_FRAME m_TransformFrame{};
	wstring	m_ModelTag{};
	wstring m_ResourceRootPath{};
	wstring m_ModelFilePath{};
	string m_ModelResourcePath{};
	Shared<const MODEL_MATERIAL_SETTINGS> m_MaterialSettings{};
	AnimationPresetSnapshot m_AnimationPreset{};
	Matrix	m_PreLocalTransformMatrix{};
	Bool	m_IsSkeletal{ false };
	int32	m_RootLocalNode{ -1 };

private:
	Bool	m_IsAnimLoop{ false };
	Bool	m_IsPrevAnimLoop{ false };
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
	Shared<AnimationTracker> m_Tracker{ nullptr };
	// Instance callbacks are deliberately not copied from resource prototypes.
	vector<pair<uint32, AnimationTracker::ANIMATION_NOTIFY>> m_AnimationNotifyDefinitions;
	uint32 m_PendingAnimIndex{ UINT32_MAX };

public:
	static Shared<Model> CreatePrototype();
	static Shared<Model> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, const tChar* path, const Matrix& preLocalTransformMatrix = XMMatrixIdentity());
	Shared<Component> Clone(void* arg = nullptr) override;
};

NS_END
