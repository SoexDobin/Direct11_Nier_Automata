#include "Model.h"
#include "Game.h"
#include "Bone.h"
#include "Material.h"
#include "Mesh.h"
#include "Animation.h"

#include <fstream>
#include <istream>
#include <filesystem>
#include "SpdLogger.h"

Model::Model() : Component{} {}
Model::Model(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Component{device, context} {}

Model::Model(const Model& rhs)
	: Component{ rhs }, 
	m_PreLocalTransformMatrix{ rhs.m_PreLocalTransformMatrix }, 
	m_IsSkeletal{rhs.m_IsSkeletal},
	m_IsAnimLoop{rhs.m_IsAnimLoop}, 
	m_NumMeshes{ rhs.m_NumMeshes },
	m_Meshes{ rhs.m_Meshes },
	m_NumMaterials{ rhs.m_NumMaterials },
	m_Materials{ rhs.m_Materials },
	m_AnimationNames{ rhs.m_AnimationNames },
	m_NumBones {rhs.m_NumBones}, m_NumAnimation{rhs.m_NumAnimation},
	m_ModelTag { rhs.m_ModelTag }
{
	for (auto& prototypeAnim : rhs.m_Animations)
		m_Animations.push_back(static_pointer_cast<Animation>(prototypeAnim->Clone()));

	for (auto& prototypeBone : rhs.m_Bones)
		m_Bones.push_back(static_pointer_cast<Bone>(prototypeBone->Clone()));
}

HRESULT Model::Initialize_Prototype(const tChar* modelFilePath, const Matrix& preLocalTransformMatrix)
{
	m_PreLocalTransformMatrix = preLocalTransformMatrix;

	ifstream in(modelFilePath, std::ios::binary);
	if (!in.is_open())
	{
		LOG_ERROR(L"Failed to open model binary : {}", modelFilePath);
		return E_FAIL;
	}

	/* Header */
	MODEL_HEADER header{};
	in.read(reinterpret_cast<Char*>(&header), sizeof(header));
	if (memcmp(header.magic, "NMDL", 4) != 0)
		return E_FAIL;

	m_IsSkeletal = header.isAnim;
	m_NumMeshes = header.numMeshes;
	m_NumMaterials = header.numMaterials;
	m_NumBones = header.numBones;
	m_NumAnimation = header.numAnimations;

	std::string currentModelDirectory = std::filesystem::path(modelFilePath).parent_path().string() + "\\";

	if (FAILED(Ready_Bones(in)))
		return E_FAIL;

	if (FAILED(Ready_Meshes(in, header.isAnim)))
		return E_FAIL;

	if (FAILED(Ready_Materials(in, currentModelDirectory)))
		return E_FAIL;

	if (FAILED(Ready_Animation(in)))
		return E_FAIL;
	
	//Update_ModelAnimation(0.f);

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

void Model::Set_ModelTag(const wstring& tag)
{
	if (m_ModelTag == tag) return;

	// 1. 새로운 프로토타입 검색
	int32 levIndex = GAME_INSTANCE->Get_ContainLevelByModelTag(tag);
	if (levIndex == -1)
	{
		LOG_ERROR(L"Failed to find Model {} in levels", tag);
		return;
	}

	auto prototype = GAME_INSTANCE->Get_Model(levIndex, tag.c_str());
	if (!prototype) return;

	// ── 핵심 방어 코드 ──
	// 1. 자기 자신이 복사 대상(프로토타입)일 경우
	if (prototype.get() == this)
	{
		m_ModelTag = tag;
		return;
	}

	// 2. 스켈레탈(애니메이션) 타입 호환성 체크
	if (m_IsSkeletal != prototype->m_IsSkeletal)
	{
		LOG_WARN(L"[Model] Skeletal mismatch! Prev: {}, New: {}. Assignment aborted.", 
			m_IsSkeletal ? L"Skeletal" : L"Static", prototype->m_IsSkeletal ? L"Skeletal" : L"Static");
		return;
	}

	// 2. 기존 리소스 정리
	On_Destroy();

	// 3. 데이터 깊은 복사 (프로토타입으로부터)
	m_ModelTag = tag;
	m_PreLocalTransformMatrix = prototype->m_PreLocalTransformMatrix;
	m_IsSkeletal = prototype->m_IsSkeletal;
	m_NumMeshes = prototype->m_NumMeshes;
	m_Meshes = prototype->m_Meshes; // Mesh는 공유
	m_NumMaterials = prototype->m_NumMaterials;
	m_Materials = prototype->m_Materials; // Material은 공유
	m_AnimationNames = prototype->m_AnimationNames;
	m_NumBones = prototype->m_NumBones;
	m_NumAnimation = prototype->m_NumAnimation;

	// 본과 애니메이션은 상태를 가지므로 클론(Clone) 필수
	m_Bones.clear();
	for (auto& pBone : prototype->m_Bones)
		m_Bones.push_back(static_pointer_cast<Bone>(pBone->Clone()));

	m_Animations.clear();
	for (auto& pAnim : prototype->m_Animations)
		m_Animations.push_back(static_pointer_cast<Animation>(pAnim->Clone()));

	// 애니메이션 초기 바인딩 업데이트
	Update_ModelAnimation(0.f);
}

void Model::On_Destroy()
{
	m_Meshes.clear();
	m_Materials.clear();
	m_Bones.clear();
	m_Animations.clear();
	m_AnimationNames.clear();
	Component::On_Destroy();
}

int32 Model::Get_AnimationIndexByName(const wstring& name)
{
	if (!m_AnimationNames.contains(name))
	{
		LOG_ERROR(L"Failed to Get Animation Index by Name {}", name); 
		return - 1;
	}


	return m_AnimationNames[name];
}

const wstring& Model::Get_AnimationNameByIndex(uint32 index)
{
	static wstring s_EmptyString = L"";
	if (index >= m_Animations.size())
	{
		LOG_ERROR(L"Failed to Get Animation Name by Index {}", index);
		return s_EmptyString;
	}

	return m_Animations[index]->Get_Name();
}

void Model::Update_ModelAnimation(Float timeDelta)
{
	if (!m_IsActive || !m_IsSkeletal) return;

	if (m_IsBlending)
	{
		m_BlendingElapsed += timeDelta;
		Float ratio = m_BlendingElapsed / m_BlendingDuration;

		if (ratio >= 1.f)
		{
			m_IsBlending = false;
			m_CurrentAnimIndex = m_NextAnimIndex;
			m_Animations[m_CurrentAnimIndex]->Update_TransformationMatrix(timeDelta, m_Bones, m_IsAnimLoop, m_RootLocalNode);
		}
		else
		{
			m_Animations[m_CurrentAnimIndex]->Blend_TransformationMatrix(timeDelta, m_Animations[m_NextAnimIndex], ratio, m_Bones, m_RootLocalNode);
		}
	}
	else
	{
		m_IsAnimEnd = m_Animations[m_CurrentAnimIndex]->Update_TransformationMatrix(timeDelta, m_Bones, m_IsAnimLoop, m_RootLocalNode);

		if (m_IsAnimEnd && m_IsAnimLoop)
		{
		}
	}

	for (auto& bone : m_Bones)
	{
		bone->Update_CombinedTransformationMatrix(m_Bones, m_PreLocalTransformMatrix);
	}
}

void Model::Set_Animation(uint32 index, Float blendDuration)
{
	if (m_CurrentAnimIndex == index) return;

	if (m_IsBlending)
	{
		m_CurrentAnimIndex = m_NextAnimIndex;
	}

	m_NextAnimIndex = index;
	m_IsBlending = true;
	m_BlendingElapsed = 0.f;
	m_BlendingDuration = blendDuration;

	Float progress = m_Animations[m_CurrentAnimIndex]->Get_Progress();
	m_Animations[m_NextAnimIndex]->Set_Progress(progress);

}

int32 Model::Get_BoneIndexByName(const string& boneName) const
{
	for (size_t i = 0; i < m_Bones.size(); ++i)
	{
		if (m_Bones[i]->Is_SameBone(boneName.c_str()))
			return static_cast<int32>(i);
	}
	return -1;
}

const TRANSFORM_FRAME& Model::Get_RootTransformDelta(uint32 nodeIndex) const
{
	static TRANSFORM_FRAME emptyFrame{ Vector3::One, Vector4(0.f, 0.f, 0.f, 1.f), Vector3::Zero };
	
	if (m_Animations.empty() || m_CurrentAnimIndex >= m_Animations.size())
		return emptyFrame;

	return m_Animations[m_CurrentAnimIndex]->Get_TransformDelta(nodeIndex);
}

void Model::Set_LocalRootNode(uint32 nodeIndex)
{
	m_RootLocalNode = static_cast<int32>(nodeIndex);
	for (uint32 i = 0; i < m_NumAnimation; ++i)
	{
		m_Animations[i]->Set_LocalTransformationPresent(true);
		// 모든 animation의 순회로 루트에 영향을 받는 걸 명시
	}
}

HRESULT Model::Render(uint32 meshIndex)
{
	m_Meshes[meshIndex]->Bind_Resources();
	m_Meshes[meshIndex]->Render();

	return S_OK;
}

HRESULT Model::Ready_Meshes(ifstream& in, Bool isAnim)
{
	for (uint32 i = 0; i < m_NumMeshes; ++i)
	{
		MODEL_MESH modelData{};
		uint32 nameLength, numBones, numOffsetMatrices, numVertex, numIndex;
		in.read(reinterpret_cast<Char*>(&nameLength), sizeof(uint32));
		if (nameLength > 0)
		{
			modelData.name.resize(nameLength);
			in.read(&modelData.name[0], nameLength);
		}
		in.read(reinterpret_cast<Char*>(&modelData.materialIndex), sizeof(int32));

		in.read(reinterpret_cast<Char*>(&numBones), sizeof(uint32));
		if (numBones > 0)
		{
			// 1. 본 인덱스 배열 로드
			modelData.boneIndices.resize(numBones);
			in.read(reinterpret_cast<Char*>(modelData.boneIndices.data()), numBones * sizeof(uint32));

			// 2. 오프셋 행렬 배열 로드 
			in.read(reinterpret_cast<Char*>(&numOffsetMatrices), sizeof(uint32));
			modelData.offsetMatrices.resize(numOffsetMatrices);
			in.read(reinterpret_cast<Char*>(modelData.offsetMatrices.data()), numOffsetMatrices * sizeof(Matrix));
		}

		in.read(reinterpret_cast<Char*>(&numVertex), sizeof(uint32));
		if (isAnim)
		{
			modelData.animVertices.resize(numVertex);
			in.read(reinterpret_cast<Char*>(modelData.animVertices.data()), numVertex * sizeof(VTXANIMMESH));
		}
		else
		{
			modelData.vertices.resize(numVertex);
			in.read(reinterpret_cast<Char*>(modelData.vertices.data()), numVertex * sizeof(VTXMESH));
		}

		in.read(reinterpret_cast<Char*>(&numIndex), sizeof(uint32));
		modelData.indices.resize(numIndex);
		in.read(reinterpret_cast<Char*>(modelData.indices.data()), numIndex * sizeof(uint32));
		auto mesh = Mesh::Create(m_Device, m_Context, isAnim, modelData, m_PreLocalTransformMatrix);
		if (mesh == nullptr) return E_FAIL;
		m_Meshes.push_back(mesh);
	}

	return S_OK;
}

HRESULT Model::Ready_Materials(ifstream& in, const std::string& directoryPath)
{
	for (size_t i = 0; i < m_NumMaterials; ++i)
	{
		MODEL_MATERIAL materialData{};
		uint32 nameLength = 0;
		in.read(reinterpret_cast<Char*>(&nameLength), sizeof(uint32));
		if (nameLength > 0)
		{
			materialData.name.resize(nameLength);
			in.read(&materialData.name[0], nameLength);
		}

		// textureTypeMax 읽기
		in.read(reinterpret_cast<Char*>(&materialData.textureTypeMax), sizeof(uint32));

		// directoryPath 세팅 (바이너리에서 읽지 않고 파라미터 복사)
		materialData.directoryPath = directoryPath;

		// textures 읽기
		uint32 numTex = 0;
		in.read(reinterpret_cast<Char*>(&numTex), sizeof(uint32));
		for (uint32 t = 0; t < numTex; ++t)
		{
			MODEL_ENTRY entry{};
			in.read(reinterpret_cast<Char*>(&entry.typeIndex), sizeof(uint32));
			uint32 pathLength = 0;
			in.read(reinterpret_cast<Char*>(&pathLength), sizeof(uint32));
			if (pathLength > 0)
			{
				entry.path.resize(pathLength);
				in.read(entry.path.data(), pathLength);
			}
			materialData.textures.push_back(entry);
		}

		auto material = Material::Create(m_Device, m_Context, materialData);
		if (material == nullptr)
			return E_FAIL;
		
		m_Materials.push_back(material);
	}

	return S_OK;
}

HRESULT Model::Ready_Animation(ifstream& in)
{
	for (uint32 i = 0; i < m_NumAnimation; ++i)
	{
		MODEL_ANIMATION animationData{};
		uint32 animNameLength = 0;
		in.read(reinterpret_cast<Char*>(&animNameLength), sizeof(uint32));
		animationData.name.resize(animNameLength);
		in.read(animationData.name.data(), animNameLength);
		in.read(reinterpret_cast<Char*>(&animationData.duration), sizeof(Float));
		in.read(reinterpret_cast<Char*>(&animationData.tickPerSecond), sizeof(Float));
		in.read(reinterpret_cast<Char*>(&animationData.numChannel), sizeof(uint32));

		in.read(reinterpret_cast<Char*>(&animationData.rootTotalTranslation), sizeof(Vector3));
		in.read(reinterpret_cast<Char*>(&animationData.rootTotalRotation), sizeof(Vector4));

		animationData.channels.reserve(animationData.numChannel);
		for (uint32 j = 0; j < animationData.numChannel; ++j)
		{
			MODEL_CHANNEL channelData{};
			uint32 channelNameLength = 0;
			in.read(reinterpret_cast<Char*>(&channelNameLength), sizeof(uint32));
			channelData.name.resize(channelNameLength);
			in.read(channelData.name.data(), channelNameLength);
			in.read(reinterpret_cast<Char*>(&channelData.boneIndex), sizeof(int32));
			in.read(reinterpret_cast<Char*>(&channelData.numKeyFrames), sizeof(uint32));

			channelData.keyFrames.resize(channelData.numKeyFrames);
			in.read(reinterpret_cast<Char*>(channelData.keyFrames.data()), channelData.numKeyFrames * sizeof(KEYFRAME));

			animationData.channels.push_back(channelData);
		}

		
		auto animation = Animation::Create(m_Device, m_Context, animationData);
		if (animation == nullptr)
			return E_FAIL;

		m_AnimationNames.emplace(Helper::To_wString(animationData.name), i);
		m_Animations.push_back(animation);
	}

	return S_OK;
}

HRESULT Model::Ready_Bones(ifstream& in)
{
	for (uint32 i = 0; i < m_NumBones; ++i)
	{
		MODEL_BONE boneData{};
		uint32 nameLength = 0;
		in.read(reinterpret_cast<Char*>(&nameLength), sizeof(uint32));
		boneData.name.resize(nameLength);
		in.read(&boneData.name[0], nameLength);
		in.read(reinterpret_cast<Char*>(&boneData.parentIndex), sizeof(int32));
		in.read(reinterpret_cast<Char*>(&boneData.transform), sizeof(Matrix));

		auto bone = Bone::Create(m_Device, m_Context, boneData);
		if (bone == nullptr)
			return E_FAIL;

		m_Bones.push_back(bone);
	}

	return S_OK;
}

HRESULT Model::Bind_Material(const Shared<Shader>& shader, const Char* constantName, uint32 meshIndex, uint32 materialType, uint32 textureIndex)
{
	return m_Materials[m_Meshes[meshIndex]->Get_MaterialIndex()]->Bind_Material(shader, constantName, materialType, textureIndex);
}

HRESULT Model::Bind_BoneMatrices(const Shared<Shader>& shader, const Char* constantName, uint32 meshIndex)
{
	return m_Meshes[meshIndex]->Bind_BoneMatrices(shader, constantName, m_Bones);
}

Shared<Model> Model::CreatePrototype()
{
	auto model = make_shared<Model>(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context());

	if (FAILED(model->Initialize_Prototype()))
	{
		MSG_BOX("Failed To CreatePrototype Model");
		return nullptr;
	}

	return model;
}

Shared<Model> Model::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, const tChar* path, const Matrix& preLocalTransformMatrix)
{
	auto model = make_shared<Model>(device, context);

	if (FAILED(model->Initialize_Prototype(path, preLocalTransformMatrix)))
	{
		LOG_ERROR(L"Failed To Create Model Resource Path : {}", path);
		MSG_BOX("Failed To Create Model Resource");
		return nullptr;
	}

	return model;
}

Shared<Component> Model::Clone(void* arg)
{
	if (arg == nullptr)
	{
		LOG_ERROR(L"There is no ModelDesc");
		MSG_BOX("There is no ModelDesc");
		return nullptr;
	}

	MODEL_DESC& desc = *static_cast<MODEL_DESC*>(arg);

	int32 levIndex = GAME_INSTANCE->Get_ContainLevelByModelTag(desc.modelTag);
	if (levIndex == -1)
	{
		LOG_ERROR(L"Failed to find Model {} in level searching", desc.modelTag);
		return nullptr;
	}

	auto prototype = GAME_INSTANCE->Get_Model(levIndex, desc.modelTag.c_str());

	auto model = make_shared<Model>(*prototype);


	if (FAILED(model->Initialize(arg)))
	{
		MSG_BOX("Failed To Clone Model");
		return nullptr;
	}

	return model;
}
