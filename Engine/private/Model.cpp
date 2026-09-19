#include "Model.h"
#include "Game.h"
#include "Bone.h"
#include "Material.h"
#include "Mesh.h"
#include "Animation.h"
#include "AnimationTracker.h"
#include <nlohmann/json.hpp>

#include <fstream>
#include <istream>
#include <filesystem>
#include "SpdLogger.h"

namespace
{
	using json = nlohmann::json;

	string To_Utf8Path(const filesystem::path& path)
	{
		const std::u8string value = path.generic_u8string();
		return string{ value.begin(), value.end() };
	}

	Bool Is_SafeResourcePath(const string& value)
	{
		if (value.empty() || value.find('\\') != string::npos)
			return false;

		const filesystem::path path = filesystem::path(Helper::To_wString(value));
		if (path.is_absolute() || path.has_root_name() || path.has_root_directory())
			return false;
		if (To_Utf8Path(path.lexically_normal()) != value)
			return false;

		Bool firstPart = true;
		for (const filesystem::path& part : path)
		{
			if (part == L"." || part == L"..")
				return false;
			if (firstPart)
			{
				wstring name = part.wstring();
				std::ranges::transform(name, name.begin(), [](wchar_t ch) {
					return static_cast<wchar_t>(::towlower(ch));
				});
				if (name == L"resources")
					return false;
				firstPart = false;
			}
		}
		return true;
	}

	Bool Parse_MaterialSettingsDocument(const filesystem::path& settingsPath,
		const wstring& expectedModelTag, const string& expectedModelPath,
		const vector<string>& materialNames, MODEL_MATERIAL_SETTINGS& outSettings,
		string& outError)
	{
		outError.clear();
		try
		{
			ifstream in(settingsPath, ios::binary);
			if (!in.is_open())
			{
				outError = "unable to open material settings";
				return false;
			}

			json document;
			in >> document;
			if (!document.is_object() || document.value("schemaVersion", 0u) != 1u)
			{
				outError = "unsupported material settings schema";
				return false;
			}

			const string expectedTag = Helper::To_String(expectedModelTag);
			if (!document.contains("modelTag") || !document["modelTag"].is_string() ||
				document["modelTag"].get<string>() != expectedTag)
			{
				outError = "material settings modelTag mismatch";
				return false;
			}

			if (!document.contains("modelPath") || !document["modelPath"].is_string())
			{
				outError = "material settings modelPath is missing";
				return false;
			}
			const string modelPath = document["modelPath"].get<string>();
			if (!Is_SafeResourcePath(modelPath) || modelPath != expectedModelPath)
			{
				outError = "material settings modelPath is unsafe or does not match the model";
				return false;
			}

			if (!document.contains("materials") || !document["materials"].is_array() ||
				document["materials"].size() != materialNames.size())
			{
				outError = "material settings must contain exactly one entry per model material";
				return false;
			}

			MODEL_MATERIAL_SETTINGS candidate;
			candidate.modelTag = expectedTag;
			candidate.modelPath = modelPath;
			candidate.materials.reserve(materialNames.size());
			unordered_set<uint32> usedIndices;
			for (const json& item : document["materials"])
			{
				if (!item.is_object() || !item.contains("materialIndex") ||
					!item["materialIndex"].is_number_unsigned())
				{
					outError = "materialIndex must be an unsigned integer";
					return false;
				}

				MODEL_MATERIAL_SETTING setting;
				setting.materialIndex = item["materialIndex"].get<uint32>();
				if (setting.materialIndex >= materialNames.size() ||
					!usedIndices.insert(setting.materialIndex).second)
				{
					outError = "duplicate or out-of-range materialIndex";
					return false;
				}

				if (!item.contains("sourceMaterialName") || !item["sourceMaterialName"].is_string())
				{
					outError = "sourceMaterialName is missing";
					return false;
				}
				setting.sourceMaterialName = item["sourceMaterialName"].get<string>();
				if (setting.sourceMaterialName != materialNames[setting.materialIndex])
				{
					outError = "sourceMaterialName does not match the model material index";
					return false;
				}

				setting.sourceShaderName = item.value("sourceShaderName", string{});
				setting.sourceTechniqueName = item.value("sourceTechniqueName", string{});
				setting.passName = item.value("passName", string{});
				if (setting.passName.empty() || setting.passName == "Unresolved")
				{
					outError = "passName must be resolved before saving";
					return false;
				}

				const string renderGroup = item.value("renderGroup", string{});
				if (renderGroup == "NONBLEND")
					setting.isBlend = false;
				else if (renderGroup == "BLEND")
					setting.isBlend = true;
				else
				{
					outError = "renderGroup must be NONBLEND or BLEND";
					return false;
				}

				if (!item.contains("textures") || !item["textures"].is_object())
				{
					outError = "textures must be an object";
					return false;
				}
				for (auto textureIt = item["textures"].begin(); textureIt != item["textures"].end(); ++textureIt)
				{
					if (textureIt.key().empty() || !textureIt.value().is_string())
					{
						outError = "texture slot and path must be strings";
						return false;
					}
					const string resourcePath = textureIt.value().get<string>();
					if (!Is_SafeResourcePath(resourcePath))
					{
						outError = "unsafe texture resource path";
						return false;
					}
					setting.textures.push_back({ textureIt.key(), resourcePath });
				}
				candidate.materials.push_back(std::move(setting));
			}

			std::ranges::sort(candidate.materials, {}, &MODEL_MATERIAL_SETTING::materialIndex);
			outSettings = std::move(candidate);
			return true;
		}
		catch (const std::exception& exception)
		{
			outError = exception.what();
			return false;
		}
	}

	wstring Find_ResourceRoot(const tChar* modelFilePath)
	{
		if (!modelFilePath || !*modelFilePath)
			return {};
		std::error_code errorCode;
		filesystem::path current = filesystem::weakly_canonical(
			filesystem::absolute(filesystem::path(modelFilePath), errorCode), errorCode).parent_path();
		if (errorCode)
			return {};
		while (!current.empty())
		{
			wstring name = current.filename().wstring();
			std::ranges::transform(name, name.begin(), [](wchar_t ch) {
				return static_cast<wchar_t>(::towlower(ch));
			});
			if (name == L"resources")
				return current.wstring();
			const filesystem::path parent = current.parent_path();
			if (parent == current)
				break;
			current = parent;
		}
		return {};
	}

	Bool Is_SafeAnimationPath(const wstring& relativePath)
	{
		const filesystem::path path(relativePath);
		if (relativePath.empty() || path.is_absolute() || path.extension() != L".anim")
			return false;
		return std::ranges::none_of(path, [](const filesystem::path& part) {
			return part == L".." || part == L".";
		});
	}
}

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
	m_ModelTag { rhs.m_ModelTag },
	m_ResourceRootPath{ rhs.m_ResourceRootPath },
	m_ModelFilePath{ rhs.m_ModelFilePath },
	m_ModelResourcePath{ rhs.m_ModelResourcePath },
	m_MaterialSettings{ rhs.m_MaterialSettings },
	m_AnimationPreset{ rhs.m_AnimationPreset },
	m_Tracker{ nullptr }
{
	for (auto& prototypeAnim : rhs.m_Animations)
		m_Animations.push_back(static_pointer_cast<Animation>(prototypeAnim->Clone()));

	for (auto& prototypeBone : rhs.m_Bones)
		m_Bones.push_back(static_pointer_cast<Bone>(prototypeBone->Clone()));

	// ★ 버그 수정: 클론할 때마다 Tracker도 새로 생성 (nullptr로 남겨두면 Reset시 크래시)
	m_Tracker = AnimationTracker::Create(m_Device, m_Context);
}

HRESULT Model::Initialize_Prototype(const tChar* modelFilePath, const Matrix& preLocalTransformMatrix)
{
	m_PreLocalTransformMatrix = preLocalTransformMatrix;
	m_ResourceRootPath = Find_ResourceRoot(modelFilePath);
	std::error_code modelPathError;
	m_ModelFilePath = filesystem::weakly_canonical(
		filesystem::absolute(filesystem::path(modelFilePath), modelPathError), modelPathError).wstring();
	if (modelPathError)
		m_ModelFilePath = modelFilePath;
	if (!m_ResourceRootPath.empty())
	{
		const filesystem::path relativePath = filesystem::relative(
			filesystem::path(m_ModelFilePath), filesystem::path(m_ResourceRootPath), modelPathError);
		if (!modelPathError)
			m_ModelResourcePath = To_Utf8Path(relativePath);
	}

	ifstream in(modelFilePath, std::ios::binary);
	if (!in.is_open())
	{
		LOG_ERROR(L"Failed to open model binary : {}", modelFilePath);
		return E_FAIL;
	}

	/* Header */
	MODEL_HEADER header{};
	in.read(reinterpret_cast<Char*>(&header), sizeof(header));
	if (!in || memcmp(header.magic, MODEL_MAGIC, sizeof(header.magic)) != 0 ||
		header.version == 0 || header.version > MODEL_VERSION)
	{
		LOG_ERROR(L"Invalid model header or unsupported version : {}", modelFilePath);
		return E_FAIL;
	}

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

	m_Tracker = AnimationTracker::Create(m_Device, m_Context);
	if (m_Tracker == nullptr)
		return E_FAIL;
	
	Update_ModelAnimation(0.f);

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

HRESULT Model::Post_Load()
{
	if (m_AnimationPreset.Is_Empty() &&
		(m_PendingAnimIndex != UINT32_MAX || !m_AnimationNotifyDefinitions.empty()))
		LOG_WARN(L"[AnimationPreset] Model {} has no snapshot; animation requests remain pending", m_ModelTag);
	return m_AnimationPreset.Is_Empty() ? S_OK : Apply_AnimationPreset(m_AnimationPreset);
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
	m_ResourceRootPath = prototype->m_ResourceRootPath;
	m_ModelFilePath = prototype->m_ModelFilePath;
	m_ModelResourcePath = prototype->m_ModelResourcePath;
	m_MaterialSettings = prototype->m_MaterialSettings;
	m_AnimationPreset = prototype->m_AnimationPreset;
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

	if (m_Tracker)
		m_Tracker->Clear();

	// 애니메이션 초기 바인딩 업데이트
	Update_ModelAnimation(0.f);
}

HRESULT Model::Get_MeshMaterialInfo(uint32 meshIndex, string& meshName, uint32& materialIndex) const
{
	if (meshIndex >= m_Meshes.size() || !m_Meshes[meshIndex]) return E_INVALIDARG;
	meshName = m_Meshes[meshIndex]->Get_MeshName();
	materialIndex = m_Meshes[meshIndex]->Get_MaterialIndex();
	return S_OK;
}

vector<string> Model::Get_MaterialNames() const
{
	vector<string> names;
	names.reserve(m_Materials.size());
	for (const Shared<Material>& material : m_Materials)
		names.push_back(material ? material->Get_MaterialName() : string{});
	return names;
}

wstring Model::Make_MaterialSettingsFileName(const wstring& modelTag)
{
	wstring fileName = modelTag;
	for (wchar_t& ch : fileName)
	{
		if (ch < 0x20 || ch == L'<' || ch == L'>' || ch == L':' || ch == L'"' ||
			ch == L'/' || ch == L'\\' || ch == L'|' || ch == L'?' || ch == L'*')
			ch = L'_';
	}
	for (size_t index = fileName.size(); index > 0 &&
		(fileName[index - 1] == L' ' || fileName[index - 1] == L'.'); --index)
		fileName[index - 1] = L'_';
	if (fileName.empty() || fileName == L"." || fileName == L"..")
		fileName = L"_";
	return fileName + L".json";
}

HRESULT Model::Validate_MaterialSettings(const filesystem::path& settingsPath,
	const wstring& expectedModelTag) const
{
	if (!filesystem::is_regular_file(settingsPath))
		return S_FALSE;

	MODEL_MATERIAL_SETTINGS candidate;
	string error;
	if (!Parse_MaterialSettingsDocument(settingsPath, expectedModelTag,
		m_ModelResourcePath, Get_MaterialNames(), candidate, error))
	{
		LOG_ERROR(L"Invalid material settings {} : {}", settingsPath.wstring(), Helper::To_wString(error));
		return E_FAIL;
	}
	return S_OK;
}

HRESULT Model::Prepare_MaterialSettings(const filesystem::path& settingsPath,
	const wstring& expectedModelTag, Shared<const MODEL_MATERIAL_SETTINGS>& prepared,
	const Shared<Shader>& shader) const
{
	if (!filesystem::is_regular_file(settingsPath))
		return S_FALSE;

	MODEL_MATERIAL_SETTINGS candidate;
	string error;
	if (!Parse_MaterialSettingsDocument(settingsPath, expectedModelTag,
		m_ModelResourcePath, Get_MaterialNames(), candidate, error))
	{
		LOG_ERROR(L"Invalid material settings {} : {}", settingsPath.wstring(), Helper::To_wString(error));
		return E_FAIL;
	}

	try
	{
		for (const auto& setting : candidate.materials)
		{
			vector<pair<string, filesystem::path>> paths;
			for (const auto& texture : setting.textures)
			{
				const filesystem::path path = filesystem::weakly_canonical(
					filesystem::path(m_ResourceRootPath) /
					filesystem::path(std::u8string(texture.resourcePath.begin(), texture.resourcePath.end())));
				const string relative = To_Utf8Path(filesystem::relative(path, m_ResourceRootPath));
				if (!Is_SafeResourcePath(relative) || !filesystem::is_regular_file(path))
				{
					LOG_ERROR(L"Missing or unsafe material texture {}", path.wstring());
					return E_FAIL;
				}
				paths.emplace_back(texture.slotName, path);
				if (std::ranges::find(candidate.textureSlots, texture.slotName) == candidate.textureSlots.end())
					candidate.textureSlots.push_back(texture.slotName);
			}
			auto material = make_shared<Material>(m_Device, m_Context);
			if (FAILED(material->Prepare_NamedTextures(paths))) return E_FAIL;
			candidate.preparedMaterials.push_back(std::move(material));
		}
	}
	catch (const std::exception& exception)
	{
		LOG_ERROR(L"Material preparation failed: {}", Helper::To_wString(exception.what()));
		return E_FAIL;
	}
	std::ranges::sort(candidate.textureSlots);
	Shared<const MODEL_MATERIAL_SETTINGS> snapshot = make_shared<MODEL_MATERIAL_SETTINGS>(std::move(candidate));
	if (shader && FAILED(Validate_MaterialBindings(shader, snapshot))) return E_FAIL;
	prepared = std::move(snapshot);
	return S_OK;
}

HRESULT Model::Load_MaterialSettings(const filesystem::path& settingsPath,
	const wstring& expectedModelTag, const Shared<Shader>& shader)
{
	Shared<const MODEL_MATERIAL_SETTINGS> prepared;
	const HRESULT hr = Prepare_MaterialSettings(settingsPath, expectedModelTag, prepared, shader);
	if (hr != S_OK) return hr;
	return Apply_MaterialSettings(prepared);
}

HRESULT Model::Apply_MaterialSettings(const Shared<const MODEL_MATERIAL_SETTINGS>& prepared)
{
	if (!prepared || (!m_ModelTag.empty() && prepared->modelTag != Helper::To_String(m_ModelTag)) ||
		prepared->modelPath != m_ModelResourcePath || prepared->preparedMaterials.size() != m_Materials.size() ||
		prepared->materials.size() != m_Materials.size()) return E_INVALIDARG;
	for (size_t i = 0; i < m_Materials.size(); ++i)
		if (!prepared->preparedMaterials[i] || prepared->materials[i].materialIndex != i ||
			prepared->materials[i].sourceMaterialName != m_Materials[i]->Get_MaterialName()) return E_INVALIDARG;
	if (m_ModelTag.empty()) m_ModelTag = Helper::To_wString(prepared->modelTag);
	m_MaterialSettings = prepared;
	return S_OK;
}

HRESULT Model::Validate_MaterialBindings(const Shared<Shader>& shader,
	const Shared<const MODEL_MATERIAL_SETTINGS>& settings) const
{
	if (!shader) return E_INVALIDARG;
	if (!settings)
		return shader->Has_Pass("Default_Pass") && shader->Has_SRV("g_AlbedoMap") ? S_OK : E_FAIL;
	if (settings->preparedMaterials.size() != m_Materials.size()) return E_FAIL;
	for (const auto& setting : settings->materials)
	{
		if (!shader->Has_Pass(setting.passName))
		{
			LOG_ERROR(L"Model {} material {} has unknown pass {}", m_ModelTag,
				setting.materialIndex, Helper::To_wString(setting.passName));
			return E_FAIL;
		}
		for (const auto& texture : setting.textures)
			if (!shader->Has_SRV(texture.slotName))
			{
				LOG_ERROR(L"Model {} material {} has unknown slot {}", m_ModelTag,
					setting.materialIndex, Helper::To_wString(texture.slotName));
				return E_FAIL;
			}
	}
	return S_OK;
}

HRESULT Model::Validate_MaterialBindings(const Shared<Shader>& shader) const
{
	return Validate_MaterialBindings(shader, m_MaterialSettings);
}

HRESULT Model::BindAndBeginMaterial(const Shared<Shader>& shader, uint32 meshIndex) const
{
	if (!shader || meshIndex >= m_Meshes.size() || !m_Meshes[meshIndex]) return E_INVALIDARG;
	const uint32 materialIndex = m_Meshes[meshIndex]->Get_MaterialIndex();
	if (materialIndex >= m_Materials.size() || !m_Materials[materialIndex]) return E_INVALIDARG;
	if (!m_MaterialSettings)
	{
		static const vector<string> defaultSlots{ "g_AlbedoMap" };
		if (!shader->Has_Pass("Default_Pass") || FAILED(shader->Clear_MaterialSlots(defaultSlots)) ||
			FAILED(m_Materials[materialIndex]->Bind_DefaultTexture(shader))) return E_FAIL;
		return shader->Begin(string{ "Default_Pass" });
	}
	if (materialIndex >= m_MaterialSettings->preparedMaterials.size() ||
		materialIndex >= m_MaterialSettings->materials.size()) return E_FAIL;
	const auto& setting = m_MaterialSettings->materials[materialIndex];
	const auto& material = m_MaterialSettings->preparedMaterials[materialIndex];
	if (!material || !shader->Has_Pass(setting.passName) ||
		FAILED(shader->Clear_MaterialSlots(m_MaterialSettings->textureSlots)) ||
		FAILED(material->Bind_NamedTextures(shader))) return E_FAIL;
	return shader->Begin(setting.passName);
}

void Model::On_Destroy()
{
	m_Meshes.clear();
	m_Materials.clear();
	m_Bones.clear();
	m_MaterialSettings.reset();
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
	static wstring emptyString = L"";
	if (index >= m_Animations.size())
	{
		LOG_ERROR(L"Failed to Get Animation Name by Index {}", index);
		return emptyString;
	}

	return m_Animations[index]->Get_AnimationName();
}

void Model::Update_ModelAnimation(Float timeDelta)
{
	if (!m_IsActive || !m_IsSkeletal || m_Animations.empty()) return;
	if (m_CurrentAnimIndex >= m_Animations.size() || m_NextAnimIndex >= m_Animations.size())
		return;

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
			m_Animations[m_CurrentAnimIndex]->Blend_TransformationMatrix(timeDelta, m_Animations[m_NextAnimIndex], ratio, m_Bones, m_IsPrevAnimLoop, m_IsAnimLoop, m_RootLocalNode);
		}
	}
	else
	{
		m_IsAnimEnd = m_Animations[m_CurrentAnimIndex]->Update_TransformationMatrix(timeDelta, m_Bones, m_IsAnimLoop, m_RootLocalNode);
	}

	for (auto& bone : m_Bones)
	{
		bone->Update_CombinedTransformationMatrix(m_Bones, m_PreLocalTransformMatrix);
	}
	m_HasPose = true;

	uint32 activeAnimIdx = m_IsBlending ? m_NextAnimIndex : m_CurrentAnimIndex;
	
	if (m_Tracker)
		m_Tracker->Update(activeAnimIdx, Get_AnimationProgress());
}

void Model::Set_Animation(uint32 index, Float blendDuration)
{
	if (m_IsSkeletal && m_Animations.empty() && m_Tracker)
	{
		if (m_PendingAnimIndex == UINT32_MAX)
			LOG_WARN(L"[AnimationPreset] Deferring initial animation {} for model {} until clips are applied", index, m_ModelTag);
		m_PendingAnimIndex = index;
		return;
	}
	if (index >= m_Animations.size() || !m_Tracker)
	{
		LOG_ERROR(L"Failed to set animation index {} on model {}", index, m_ModelTag);
		return;
	}
	if (m_CurrentAnimIndex == index) return;
	if (m_IsBlending && m_NextAnimIndex == index) return;

	m_Tracker->Reset();

	if (m_IsBlending)
	{
		m_CurrentAnimIndex = m_NextAnimIndex;
	}

	m_IsPrevAnimLoop = m_IsAnimLoop;
 	m_NextAnimIndex = index;

	if (blendDuration <= 0.0001f)
	{
		m_IsBlending = false;
		m_CurrentAnimIndex = index; // 즉시 현재 타겟으로 할당
		m_BlendingElapsed = 0.f;
		m_BlendingDuration = 0.f;
	}
	else
	{
		m_IsBlending = true;
		m_BlendingElapsed = 0.f;
		m_BlendingDuration = blendDuration;
	}
	m_IsAnimEnd = false;

	m_Animations[m_NextAnimIndex]->Set_Progress(0.f);
}

void Model::Add_AnimNotify(uint32 animIndex, const AnimationTracker::ANIMATION_NOTIFY& notify)
{
	if (m_Tracker == nullptr || !m_IsSkeletal ||
		(!m_Animations.empty() && animIndex >= m_Animations.size()))
	{
		LOG_ERROR(L"Failed to Add Animation Notify {} : {}", notify.notifyTag, animIndex);
		return;
	}
	if (m_Animations.empty() && m_AnimationNotifyDefinitions.empty())
		LOG_WARN(L"[AnimationPreset] Deferring notify registration for model {} until clips are applied", m_ModelTag);
	m_AnimationNotifyDefinitions.emplace_back(animIndex, notify);
	if (!m_Animations.empty()) Bind_AnimNotify(animIndex, notify);
}

void Model::Bind_AnimNotify(uint32 animIndex, const AnimationTracker::ANIMATION_NOTIFY& notify) const
{
	Float duration = m_Animations[animIndex]->Get_Duration();

	if (duration <= 0.f)
	{
		duration = 1.f;
	}

	AnimationTracker::ANIMATION_TRACKER_NOTIFY trackerNotify;

	trackerNotify.notifyTag = notify.notifyTag;
	trackerNotify.type = notify.type;
	trackerNotify.invokeProgress = notify.invokeFrame / duration;
	trackerNotify.endProgress = notify.endFrame / duration;
	trackerNotify.onNotify = notify.onNotify;
	trackerNotify.onEndNotify = notify.onEndNotify;

	m_Tracker->Add_Notify(animIndex, trackerNotify);
}

void Model::Add_AnimNotify(uint32 animIndex, std::initializer_list<AnimationTracker::ANIMATION_NOTIFY> notifies)
{
	for (auto notify : notifies)
		Add_AnimNotify(animIndex, notify);
}

void Model::Clear_AnimNotifies()
{
	if (m_Tracker) m_Tracker->Clear();
	m_AnimationNotifyDefinitions.clear();
}

Bool Model::Validate_AnimationRequests(size_t clipCount) const
{
	if (m_PendingAnimIndex != UINT32_MAX && m_PendingAnimIndex >= clipCount)
	{
		LOG_ERROR(L"[AnimationPreset] Pending animation {} is outside {} clips on {}", m_PendingAnimIndex, clipCount, m_ModelTag);
		return false;
	}
	for (const auto& [index, notify] : m_AnimationNotifyDefinitions)
		if (index >= clipCount)
		{
			LOG_ERROR(L"[AnimationPreset] Notify {} index {} is outside {} clips on {}", notify.notifyTag, index, clipCount, m_ModelTag);
			return false;
		}
	return true;
}

void Model::Restore_AnimationRequests()
{
	if (m_Tracker) m_Tracker->Clear();
	for (const auto& [index, notify] : m_AnimationNotifyDefinitions)
		Bind_AnimNotify(index, notify);
	if (m_PendingAnimIndex != UINT32_MAX)
		m_CurrentAnimIndex = m_NextAnimIndex = m_PendingAnimIndex;
	m_PendingAnimIndex = UINT32_MAX;
	m_IsAnimEnd = false;
	if (m_RootLocalNode >= 0)
		Set_LocalRootNode(static_cast<uint32>(m_RootLocalNode));
}

Bool Model::Is_NotifyActive(uint32 animIndex, const wstring& notifyTag) const
{
	return m_Tracker->Is_ActiveNotify(animIndex, notifyTag);
}

Bool Model::Is_NotifyActive(const wstring& notifyTag) const
{
	uint32 activeIndex = m_IsBlending ? m_NextAnimIndex : m_CurrentAnimIndex;
	return m_Tracker->Is_ActiveNotify(activeIndex, notifyTag);
}

Bool Model::Compute_LocalBounds(BoundingBox& outBounds) const
{
	Vector3 minPoint{ FLT_MAX, FLT_MAX, FLT_MAX };
	Vector3 maxPoint{ -FLT_MAX, -FLT_MAX, -FLT_MAX };
	Bool hasPoint = false;
	for (const auto& mesh : m_Meshes)
	{
		const vector<Float>& positions = mesh->Get_RawPositions();
		for (size_t i = 0; i + 2 < positions.size(); i += 3)
		{
			Vector3 point{ positions[i], positions[i + 1], positions[i + 2] };
			// Skinned vertices stay in bind space; the pre-transform is applied through the bones.
			if (m_IsSkeletal)
				point = Vector3::Transform(point, m_PreLocalTransformMatrix);
			minPoint = Vector3::Min(minPoint, point);
			maxPoint = Vector3::Max(maxPoint, point);
			hasPoint = true;
		}
	}
	if (!hasPoint)
		return false;

	BoundingBox::CreateFromPoints(outBounds, minPoint, maxPoint);
	return true;
}

void Model::Extract_RawMeshData(_Out_ vector<Float>& outPositions, _Out_ vector<int32>& outIndices) const
{
	outPositions.clear();
	outIndices.clear();
	int32 vertexOffset = 0;
	for (const auto& mesh : m_Meshes)
	{
		const auto& meshPos = mesh->Get_RawPositions();
		const auto& meshInd = mesh->Get_RawIndices();
		
		outPositions.insert(outPositions.end(), meshPos.begin(), meshPos.end());
		// 인덱스는 이전에 쌓인 정점 개수(vertexOffset)만큼 더해서 연결!
		for (int32 idx : meshInd)
		{
			outIndices.push_back(idx + vertexOffset);
		}
		
		vertexOffset += static_cast<int32>(meshPos.size() / 3);
	}
}

vector<BONE_SNAPSHOT> Model::Get_SnapShot_BoneMatrices()
{
	vector<BONE_SNAPSHOT> snapShots;
	snapShots.resize(m_NumMeshes);

	for (uint32 i = 0 ; i < m_NumMeshes; ++i)
	{
		m_Meshes[i]->Fill_BoneMatrices(m_Bones);

		const uint32 numBones = m_Meshes[i]->Get_NumMeshBones();
		snapShots[i].numBones = numBones;

		memcpy(snapShots[i].matrices,
			m_Meshes[i]->Get_BoneMatrices(),
			sizeof(Matrix) * numBones);
	}

	return snapShots;
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

Matrix Model::Get_BoneMatrix(uint32 boneIndex) const
{
	if (boneIndex >= m_Bones.size())
		return Matrix::Identity;
	return *m_Bones[boneIndex]->Get_CombinedTransformationMatrixPtr();
}

const TRANSFORM_FRAME& Model::Get_RootTransformVelocity(uint32 nodeIndex)
{
	if (m_IsBlending && m_NextAnimIndex < m_Animations.size())
	{
		Float ratio = m_BlendingElapsed / m_BlendingDuration;

		const TRANSFORM_FRAME& curDelta = m_Animations[m_CurrentAnimIndex]->Get_TransformVelocity(nodeIndex);
		const TRANSFORM_FRAME& nextDelta = m_Animations[m_NextAnimIndex]->Get_TransformVelocity(nodeIndex);

		m_TransformFrame.position = Vector3::Lerp(curDelta.position, nextDelta.position, ratio);
		m_TransformFrame.rotation = Quaternion::Slerp(curDelta.rotation, nextDelta.rotation, ratio);
		m_TransformFrame.scale = Vector3::One;

		return m_TransformFrame;
	}

	return m_Animations[m_CurrentAnimIndex]->Get_TransformVelocity(nodeIndex);
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
	if (meshIndex >= m_Meshes.size() || !m_Meshes[meshIndex]) return E_INVALIDARG;
	if (FAILED(m_Meshes[meshIndex]->Bind_Resources())) return E_FAIL;
	return m_Meshes[meshIndex]->Render();
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
		if (FAILED(Read_AnimationData(in, animationData, true)))
			return E_FAIL;

		auto animation = Animation::Create(m_Device, m_Context, animationData);
		if (animation == nullptr)
			return E_FAIL;

		m_AnimationNames.emplace(Helper::To_wString(animationData.name), i);
		m_Animations.push_back(animation);
	}

	return S_OK;
}

HRESULT Model::Read_AnimationData(ifstream& in, MODEL_ANIMATION& animationData,
	Bool hasStoredChannelCount, uint32 channelCount) const
{
	constexpr uint32 MaxNameLength = 4096;
	constexpr uint32 MaxChannelCount = MODEL_BONE_MAX * 4;
	constexpr uint32 MaxKeyFrameCount = 10'000'000;

	uint32 animNameLength = 0;
	in.read(reinterpret_cast<Char*>(&animNameLength), sizeof(animNameLength));
	if (!in || animNameLength == 0 || animNameLength > MaxNameLength)
		return E_FAIL;

	animationData.name.resize(animNameLength);
	in.read(animationData.name.data(), animNameLength);
	in.read(reinterpret_cast<Char*>(&animationData.duration), sizeof(animationData.duration));
	in.read(reinterpret_cast<Char*>(&animationData.tickPerSecond), sizeof(animationData.tickPerSecond));
	if (hasStoredChannelCount)
		in.read(reinterpret_cast<Char*>(&channelCount), sizeof(channelCount));
	in.read(reinterpret_cast<Char*>(&animationData.rootTotalTranslation), sizeof(animationData.rootTotalTranslation));
	in.read(reinterpret_cast<Char*>(&animationData.rootTotalRotation), sizeof(animationData.rootTotalRotation));
	if (!in || channelCount == 0 || channelCount > MaxChannelCount)
		return E_FAIL;
	if (!std::isfinite(animationData.duration) || animationData.duration < 0.f ||
		!std::isfinite(animationData.tickPerSecond) || animationData.tickPerSecond <= 0.f)
		return E_FAIL;

	animationData.numChannel = channelCount;
	animationData.channels.clear();
	animationData.channels.reserve(channelCount);
	unordered_set<string> channelNames;

	for (uint32 i = 0; i < channelCount; ++i)
	{
		MODEL_CHANNEL channelData{};
		uint32 channelNameLength = 0;
		in.read(reinterpret_cast<Char*>(&channelNameLength), sizeof(channelNameLength));
		if (!in || channelNameLength == 0 || channelNameLength > MaxNameLength)
			return E_FAIL;

		channelData.name.resize(channelNameLength);
		in.read(channelData.name.data(), channelNameLength);
		if (!in || !channelNames.emplace(channelData.name).second)
			return E_FAIL;

		if (hasStoredChannelCount)
		{
			in.read(reinterpret_cast<Char*>(&channelData.boneIndex), sizeof(channelData.boneIndex));
		}
		else
		{
			channelData.boneIndex = Get_BoneIndexByName(channelData.name);
			if (channelData.boneIndex < 0)
			{
				LOG_ERROR(L"Animation channel bone not found : {}", Helper::To_wString(channelData.name));
				return E_FAIL;
			}
		}

		in.read(reinterpret_cast<Char*>(&channelData.numKeyFrames), sizeof(channelData.numKeyFrames));
		if (!in || channelData.numKeyFrames == 0 || channelData.numKeyFrames > MaxKeyFrameCount)
			return E_FAIL;

		channelData.keyFrames.resize(channelData.numKeyFrames);
		in.read(reinterpret_cast<Char*>(channelData.keyFrames.data()),
			static_cast<std::streamsize>(channelData.numKeyFrames) * sizeof(KEYFRAME));
		if (!in)
			return E_FAIL;

		animationData.channels.push_back(std::move(channelData));
	}

	return S_OK;
}

HRESULT Model::Build_Animations(const vector<wstring>& animationFilePaths,
	const unordered_set<wstring>& existingNames,
	vector<Shared<Animation>>& outAnimations, vector<wstring>& outNames) const
{
	if (!m_IsSkeletal || m_Bones.empty() || animationFilePaths.empty())
		return E_INVALIDARG;

	outAnimations.clear();
	outNames.clear();
	unordered_set<wstring> allNames = existingNames;

	for (const wstring& animationFilePath : animationFilePaths)
	{
		ifstream in(animationFilePath, std::ios::binary);
		if (!in.is_open())
		{
			LOG_ERROR(L"Failed to open animation binary : {}", animationFilePath);
			return E_FAIL;
		}

		ANIMATION_HEADER header{};
		in.read(reinterpret_cast<Char*>(&header), sizeof(header));
		if (!in || memcmp(header.magic, ANIMATION_MAGIC, sizeof(header.magic)) != 0 ||
			header.version != ANIMATION_VERSION)
		{
			LOG_ERROR(L"Invalid animation header or unsupported version : {}", animationFilePath);
			return E_FAIL;
		}

		MODEL_ANIMATION animationData{};
		if (FAILED(Read_AnimationData(in, animationData, false, header.numChannels)))
		{
			LOG_ERROR(L"Failed to read animation binary : {}", animationFilePath);
			return E_FAIL;
		}

		const wstring animationName = Helper::To_wString(animationData.name);
		if (!allNames.emplace(animationName).second)
		{
			LOG_ERROR(L"Duplicate animation name {} from {}", animationName, animationFilePath);
			return E_FAIL;
		}

		auto animation = Animation::Create(m_Device, m_Context, animationData);
		if (!animation)
			return E_FAIL;

		outNames.push_back(animationName);
		outAnimations.push_back(std::move(animation));
	}
	return S_OK;
}

HRESULT Model::Load_Animations(const vector<wstring>& animationFilePaths)
{
	unordered_set<wstring> existingNames;
	for (const auto& [name, index] : m_AnimationNames)
		existingNames.emplace(name);

	vector<Shared<Animation>> pendingAnimations;
	vector<wstring> pendingNames;
	if (FAILED(Build_Animations(animationFilePaths, existingNames, pendingAnimations, pendingNames)))
		return E_FAIL;
	if (!Validate_AnimationRequests(m_Animations.size() + pendingAnimations.size()))
		return E_INVALIDARG;

	for (size_t i = 0; i < pendingAnimations.size(); ++i)
	{
		const uint32 index = static_cast<uint32>(m_Animations.size());
		m_AnimationNames.emplace(pendingNames[i], index);
		m_Animations.push_back(std::move(pendingAnimations[i]));
	}
	m_NumAnimation = static_cast<uint32>(m_Animations.size());
	m_CurrentAnimIndex = 0;
	m_NextAnimIndex = 0;
	m_IsBlending = false;
	Restore_AnimationRequests();
	Update_ModelAnimation(0.f);
	return S_OK;
}

Bool Model::Validate_AnimationPreset(const AnimationPresetSnapshot& preset) const
{
	if (preset.schemaVersion != 2 || preset.animationEnum.empty() || preset.animations.empty())
		return false;

	ReflectedEnumInfo enumInfo;
	if (FAILED(GAME_INSTANCE->Find_ReflectedEnum(preset.animationEnum, enumInfo)) ||
		enumInfo.category != "AnimationState" || enumInfo.values.empty())
		return false;

	unordered_map<string, int64_t> enumValues;
	for (const ReflectedEnumValue& value : enumInfo.values)
	{
		if (value.name.empty() || value.value < 0 ||
			static_cast<size_t>(value.value) >= preset.animations.size() ||
			!enumValues.emplace(value.name, value.value).second)
			return false;
	}

	unordered_set<wstring> animationPaths;
	unordered_set<string> coveredStates;
	for (size_t index = 0; index < preset.animations.size(); ++index)
	{
		const AnimationPresetClip& clip = preset.animations[index];
		filesystem::path normalizedPath = filesystem::path(clip.relativePath).lexically_normal();
		wstring pathKey = normalizedPath.generic_wstring();
		std::ranges::transform(pathKey, pathKey.begin(), [](wchar_t ch) {
			return static_cast<wchar_t>(::towlower(ch));
		});
		if (!Is_SafeAnimationPath(clip.relativePath) ||
			!animationPaths.emplace(std::move(pathKey)).second)
			return false;

		for (const string& state : clip.states)
		{
			const auto stateIt = enumValues.find(state);
			if (stateIt == enumValues.end() || stateIt->second != static_cast<int64_t>(index) ||
				!coveredStates.emplace(state).second)
				return false;
		}
	}
	return coveredStates.size() == enumValues.size();
}

HRESULT Model::Apply_AnimationPreset(const AnimationPresetSnapshot& preset)
{
	if (preset.schemaVersion == 2 && preset.Is_Empty())
	{
		if (!m_Animations.empty())
			m_PendingAnimIndex = m_IsBlending ? m_NextAnimIndex : m_CurrentAnimIndex;
		if (m_Tracker) m_Tracker->Clear();
		m_Animations.clear();
		m_AnimationNames.clear();
		m_NumAnimation = 0;
		m_CurrentAnimIndex = m_NextAnimIndex = 0;
		m_IsBlending = false;
		m_IsAnimEnd = false;
		m_AnimationPreset = preset;
		LOG_WARN(L"[AnimationPreset] Cleared snapshot on model {}; requests retained for reapply", m_ModelTag);
		return S_OK;
	}
	if (!Validate_AnimationPreset(preset) || m_ResourceRootPath.empty() ||
		!Validate_AnimationRequests(preset.animations.size()))
		return E_INVALIDARG;

	vector<wstring> fullPaths;
	fullPaths.reserve(preset.animations.size());
	const filesystem::path resourceRoot(m_ResourceRootPath);
	for (const AnimationPresetClip& clip : preset.animations)
	{
		const filesystem::path fullPath = (resourceRoot / clip.relativePath).lexically_normal();
		if (!filesystem::is_regular_file(fullPath))
		{
			LOG_ERROR(L"AnimationPreset clip does not exist: {}", fullPath.wstring());
			return E_FAIL;
		}
		fullPaths.push_back(fullPath.wstring());
	}

	vector<Shared<Animation>> pendingAnimations;
	vector<wstring> pendingNames;
	if (FAILED(Build_Animations(fullPaths, {}, pendingAnimations, pendingNames)))
		return E_FAIL;

	map<wstring, uint32> pendingAnimationNames;
	for (size_t index = 0; index < pendingNames.size(); ++index)
		pendingAnimationNames.emplace(pendingNames[index], static_cast<uint32>(index));

	m_Animations = std::move(pendingAnimations);
	m_AnimationNames = std::move(pendingAnimationNames);
	m_NumAnimation = static_cast<uint32>(m_Animations.size());
	m_CurrentAnimIndex = 0;
	m_NextAnimIndex = 0;
	m_IsBlending = false;
	m_AnimationPreset = preset;
	Restore_AnimationRequests();
	Update_ModelAnimation(0.f);
	LOG_INFO(L"[AnimationPreset] Applied {} clips and {} notifies to model {}", m_NumAnimation, m_AnimationNotifyDefinitions.size(), m_ModelTag);
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
	if (!shader || !constantName || meshIndex >= m_Meshes.size() || !m_Meshes[meshIndex])
		return E_INVALIDARG;
	// Without an animation update (edit mode) the combined matrices were never built; show the bind pose.
	if (!m_HasPose)
	{
		for (auto& bone : m_Bones)
			bone->Update_CombinedTransformationMatrix(m_Bones, m_PreLocalTransformMatrix);
		m_HasPose = true;
	}
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
