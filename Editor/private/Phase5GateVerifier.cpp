#include "pch.h"
#include "Phase5GateVerifier.h"

#include "AnimationPresetEditor.h"
#include "ClientSettingManager.h"
#include "Client_Function.h"
#include "Game.h"
#include "GameObject.h"
#include "Model.h"
#include "PathManager.h"
#include "Transform.h"

using namespace Engine;

NS_BEGIN(Editor)

namespace Phase5GateVerifier
{
	namespace
	{
		constexpr uint32 PrepareStage = 1;
		constexpr uint32 VerifyStage = 2;
		constexpr uint32 GateLevel = ETOI(LEVEL::GAMEPLAY);
		constexpr const Char* PresetName = "Phase5Gate_WP3000";
		constexpr const Char* AnimationEnum = "WP3000.AnimationState";

		filesystem::path Work_Directory()
		{
			return filesystem::absolute(
				filesystem::path(PATH.GetProjectRoot()) / L".codex-tmp/phase5-gates").lexically_normal();
		}

		filesystem::path Preset_Path()
		{
			return Work_Directory() / L"Phase5Gate_WP3000.json";
		}

		filesystem::path Manifest_Path()
		{
			return filesystem::path(PATH.GetResourceDir()) /
				L"Models/wp3000/Animations/manifest.json";
		}

		Bool Read_Json(const filesystem::path& path, nlohmann::json& outDocument)
		{
			try
			{
				ifstream input(path);
				if (!input.is_open())
					return false;
				input >> outDocument;
				return true;
			}
			catch (...)
			{
				return false;
			}
		}

		Bool Write_Json(const filesystem::path& path, const nlohmann::json& document)
		{
			std::error_code errorCode;
			filesystem::create_directories(path.parent_path(), errorCode);
			if (errorCode)
				return false;
			ofstream output(path, ios::binary | ios::trunc);
			if (!output.is_open())
				return false;
			output << document.dump(4);
			output.flush();
			return output.good();
		}

		HRESULT Load_GateModels()
		{
			nlohmann::json settings;
			if (!Read_Json(filesystem::path(PATH.GetProjectSettingDir()) / L"ModelSettings.json",
				settings) || !settings.contains("ModelSettings") ||
				!settings["ModelSettings"].is_array())
				return E_FAIL;

			const unordered_set<string> requiredTags = {
				"pl0000", "wp0070", "wp0220", "wp3000"
			};
			unordered_set<string> loadedTags;
			for (const nlohmann::json& item : settings["ModelSettings"])
			{
				const string tag = item.value("tag", string{});
				if (!requiredTags.contains(tag))
					continue;
				const auto ReadVector = [&item](const Char* name, const Vector3& fallback) {
					Vector3 value = fallback;
					if (item.contains(name) && item[name].is_object())
					{
						value.x = item[name].value("x", fallback.x);
						value.y = item[name].value("y", fallback.y);
						value.z = item[name].value("z", fallback.z);
					}
					return value;
				};
				const Vector3 position = ReadVector("position", Vector3::Zero);
				const Vector3 rotation = ReadVector("rotation", Vector3::Zero);
				const Vector3 scale = ReadVector("scale", Vector3::One);
				const Matrix preTransform = Matrix::CreateScale(scale) *
					Matrix::CreateFromYawPitchRoll(XMConvertToRadians(rotation.y),
						XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.z)) *
					Matrix::CreateTranslation(position);
				const filesystem::path modelPath = filesystem::path(PATH.GetResourceDir()) /
					Helper::To_wString(item.value("path", string{}));
				if (FAILED(GAME_INSTANCE->Load_Model(GateLevel, modelPath.c_str(),
					Helper::To_wString(tag), preTransform)))
					return E_FAIL;
				loadedTags.emplace(tag);
			}
			return loadedTags.size() == requiredTags.size() ? S_OK : E_FAIL;
		}

		Bool Same_Preset(const AnimationPresetSnapshot& lhs,
			const AnimationPresetSnapshot& rhs)
		{
			if (lhs.schemaVersion != rhs.schemaVersion ||
				lhs.animationEnum != rhs.animationEnum ||
				lhs.animations.size() != rhs.animations.size())
				return false;
			for (size_t index = 0; index < lhs.animations.size(); ++index)
			{
				if (lhs.animations[index].relativePath != rhs.animations[index].relativePath ||
					lhs.animations[index].states != rhs.animations[index].states)
					return false;
			}
			return true;
		}

		class GateReport final
		{
		public:
			explicit GateReport(string stage)
			{
				m_Document = {
					{ "schemaVersion", 1 },
					{ "stage", std::move(stage) },
					{ "processId", GetCurrentProcessId() },
					{ "checks", nlohmann::json::array() }
				};
			}

			void Check(const string& name, Bool passed, const string& detail = {})
			{
				m_Document["checks"].push_back({
					{ "name", name }, { "passed", passed }, { "detail", detail }
				});
				m_Passed = m_Passed && passed;
			}

			Bool Save(const filesystem::path& path)
			{
				m_Document["passed"] = m_Passed;
				return Write_Json(path, m_Document);
			}

			Bool Passed() const { return m_Passed; }

		private:
			nlohmann::json m_Document;
			Bool m_Passed = true;
		};

		void Sort_Components(nlohmann::json& object)
		{
			if (!object.contains("components") || !object["components"].is_array())
				return;
			vector<nlohmann::json> components =
				object["components"].get<vector<nlohmann::json>>();
			std::ranges::sort(components, {}, [](const nlohmann::json& component) {
				return component.value("typeName", string{});
			});
			object["components"] = std::move(components);
		}

		nlohmann::json Canonicalize_Prefab(const nlohmann::json& document)
		{
			nlohmann::json normalized = document;
			normalized.erase("prefabGuid");
			normalized["roots"] = nlohmann::json::array({ "root" });
			normalized["objects"] = nlohmann::json::object();
			if (!document.contains("roots") || document["roots"].size() != 1)
				return normalized;

			const auto Visit = [&](auto&& self, const string& guid, const string& path) -> void
			{
				if (!document["objects"].contains(guid))
					return;
				nlohmann::json object = document["objects"][guid];
				Sort_Components(object);
				nlohmann::json children = nlohmann::json::array();
				for (size_t index = 0; index < object["children"].size(); ++index)
				{
					const string childGuid = object["children"][index].get<string>();
					const string childPath = path + "/" + std::to_string(index);
					children.push_back(childPath);
					self(self, childGuid, childPath);
				}
				object["children"] = std::move(children);
				normalized["objects"][path] = std::move(object);
			};
		Visit(Visit, document["roots"][0].get<string>(), "root");
		return normalized;
		}

		nlohmann::json Normalize_Scene(nlohmann::json document)
		{
			if (document.contains("objects") && document["objects"].is_object())
				for (auto& [guid, object] : document["objects"].items())
					Sort_Components(object);
			return document;
		}

		nlohmann::json* Find_AnimationPreset(nlohmann::json& document)
		{
			if (!document.contains("objects") || !document["objects"].is_object())
				return nullptr;
			for (auto& [guid, object] : document["objects"].items())
			{
				if (object.value("stableChildKey", string{}) != "WP3000Body" ||
					!object.contains("components"))
					continue;
				for (auto& component : object["components"])
				{
					if (component.value("typeName", string{}) == "Model" &&
						component.contains("properties") &&
						component["properties"].contains("AnimationPreset"))
						return &component["properties"]["AnimationPreset"];
				}
			}
			return nullptr;
		}

		Shared<Model> Find_GateModel(const Shared<GameObject>& root)
		{
			if (!root)
				return nullptr;
			const Shared<GameObject> body = root->Find_Child(L"WP3000Body");
			return body ? body->Get_Component<Model>() : nullptr;
		}

		Bool Collect_Graph(const Shared<GameObject>& root,
			unordered_set<ObjectGuid, GuidHash>& outGuids)
		{
			if (!root || !root->Get_ObjectGuid().Is_Valid() ||
				!outGuids.emplace(root->Get_ObjectGuid()).second)
				return false;
			for (const Shared<GameObject>& child : root->Get_Children())
				if (child && child->Get_Parent() == root && !Collect_Graph(child, outGuids))
					return false;
			return true;
		}

		Bool Verify_DerivedChildren(const Shared<GameObject>& root)
		{
			if (!root)
				return false;
			for (const Shared<GameObject>& child : root->Get_Children())
			{
				if (!child || child->Get_Parent() != root)
					return false;
				if (!child->Get_StableChildKey().empty())
				{
					const string typeName =
						GAME_INSTANCE->Find_RegisteredName(child->Get_RuntimeTypeId());
					if (child->Get_ObjectGuid() != Derive_ChildObjectGuid(
						root->Get_ObjectGuid(), child->Get_StableChildKey(), typeName))
						return false;
				}
				if (!Verify_DerivedChildren(child))
					return false;
			}
			return true;
		}

		Bool Save_PrefabCopy(const Shared<GameObject>& root,
			const filesystem::path& path, nlohmann::json& outDocument)
		{
			const PrefabGuid guid = Create_PrefabGuid();
			return guid.Is_Valid() && SUCCEEDED(GAME_INSTANCE->Register_Prefab(guid, path.wstring())) &&
				SUCCEEDED(GAME_INSTANCE->SerializePrefabDocument(guid, root, GateLevel)) &&
				Read_Json(path, outDocument);
		}

		HRESULT Run_Prepare()
		{
			GateReport report("prepare");
			AnimationPresetSnapshot snapshot;
			string error;
			const Bool built = AnimationPresetEditor::Build_Snapshot(
				Manifest_Path(), AnimationEnum, snapshot, error);
			report.Check("manifest-enum-snapshot", built, error);

			const AssetGuid assetGuid = Create_AssetGuid();
			const Bool saved = built && AnimationPresetEditor::Save_Document(
				Preset_Path(), assetGuid, PresetName, snapshot, error);
			report.Check("atomic-save", saved, error);

			AnimationPresetSnapshot loaded;
			string loadedName;
			const Bool loadedNow = saved && AnimationPresetEditor::Load_Document(
				Preset_Path(), loaded, loadedName, error);
			report.Check("same-process-load", loadedNow && loadedName == PresetName &&
				Same_Preset(snapshot, loaded), error);
			const Bool reportSaved = report.Save(Work_Directory() / L"stage1-prepare.json");
			return report.Passed() && reportSaved ? S_OK : E_FAIL;
		}

		HRESULT Run_Verify(HRESULT initializationResult)
		{
			GateReport report("verify");
			report.Check("runtime-initialization", SUCCEEDED(initializationResult));

			nlohmann::json prepareReport;
			const Bool restarted = Read_Json(Work_Directory() / L"stage1-prepare.json", prepareReport) &&
				prepareReport.value("passed", false) &&
				prepareReport.value("processId", GetCurrentProcessId()) != GetCurrentProcessId();
			report.Check("editor-process-restart", restarted);

			AnimationPresetSnapshot expected;
			AnimationPresetSnapshot loaded;
			string loadedName;
			string error;
			const Bool rebuilt = AnimationPresetEditor::Build_Snapshot(
				Manifest_Path(), AnimationEnum, expected, error);
			const Bool loadedAfterRestart = rebuilt && AnimationPresetEditor::Load_Document(
				Preset_Path(), loaded, loadedName, error) && loadedName == PresetName &&
				Same_Preset(expected, loaded);
			report.Check("gate1-restart-load-same-mapping", loadedAfterRestart, error);

			if (FAILED(initializationResult) || !loadedAfterRestart)
			{
				report.Save(Work_Directory() / L"phase5-entry-gates.json");
				return E_FAIL;
			}

			GAME_INSTANCE->Clear_GameObjects(GateLevel);
			Shared<GameObject> source = GAME_INSTANCE->Instantiate_GameObject("Pl0000", GateLevel);
			Shared<Model> sourceModel = Find_GateModel(source);
			const Bool applied = sourceModel && SUCCEEDED(AnimationPresetEditor::Apply_PresetFile(
				*sourceModel, Preset_Path(), error)) && Same_Preset(sourceModel->Get_AnimationPreset(), loaded);
			report.Check("gate1-apply-after-restart", applied, error);
			if (!applied)
			{
				report.Save(Work_Directory() / L"phase5-entry-gates.json");
				return E_FAIL;
			}

			source->Set_Name(L"Phase5GateRoot");
			source->Get_Transform()->Set_Position(Vector3{ 7.f, 2.f, -3.f });
			source->Get_Transform()->Set_LocalEulerAngleByValue(Vector3{ 5.f, 30.f, 0.f });
			source->Get_Transform()->Set_Scale(Vector3{ 1.25f, 0.75f, 2.f });

			nlohmann::json validPreset;
			Read_Json(Preset_Path(), validPreset);
			const AnimationPresetSnapshot presetBeforeInvalid = sourceModel->Get_AnimationPreset();
			nlohmann::json invalidPathPreset = validPreset;
			invalidPathPreset["animations"][0]["path"] = "../escape.anim";
			const filesystem::path invalidPath = Work_Directory() / L"invalid-path-preset.json";
			Write_Json(invalidPath, invalidPathPreset);
			const Bool invalidPathRejected = FAILED(AnimationPresetEditor::Apply_PresetFile(
				*sourceModel, invalidPath, error)) &&
				Same_Preset(sourceModel->Get_AnimationPreset(), presetBeforeInvalid);
			report.Check("gate3-invalid-preset-path-no-partial-state", invalidPathRejected, error);

			nlohmann::json invalidStatePreset = validPreset;
			invalidStatePreset["animations"][0]["states"][0] = "INVALID_STATE";
			const filesystem::path invalidState = Work_Directory() / L"invalid-state-preset.json";
			Write_Json(invalidState, invalidStatePreset);
			const Bool invalidStateRejected = FAILED(AnimationPresetEditor::Apply_PresetFile(
				*sourceModel, invalidState, error)) &&
				Same_Preset(sourceModel->Get_AnimationPreset(), presetBeforeInvalid);
			report.Check("gate3-invalid-preset-state-no-partial-state", invalidStateRejected, error);

			const PrefabGuid prefabGuid = Create_PrefabGuid();
			const filesystem::path prefabPath = Work_Directory() / L"gate.prefab.json";
			const Bool prefabSaved = prefabGuid.Is_Valid() &&
				SUCCEEDED(GAME_INSTANCE->Register_Prefab(prefabGuid, prefabPath.wstring())) &&
				SUCCEEDED(GAME_INSTANCE->SerializePrefabDocument(prefabGuid, source, GateLevel));
			report.Check("gate2-prefab-save-with-preset", prefabSaved);

			Shared<GameObject> instanceA;
			Shared<GameObject> instanceB;
			const Bool instantiatedTwice = prefabSaved &&
				SUCCEEDED(GAME_INSTANCE->DeSerializePrefabDocument(prefabGuid, instanceA, GateLevel)) &&
				SUCCEEDED(GAME_INSTANCE->DeSerializePrefabDocument(prefabGuid, instanceB, GateLevel));
			unordered_set<ObjectGuid, GuidHash> graphA;
			unordered_set<ObjectGuid, GuidHash> graphB;
			const Bool graphAValid = instantiatedTwice && Collect_Graph(instanceA, graphA);
			const Bool graphBValid = instantiatedTwice && Collect_Graph(instanceB, graphB);
			const Bool disjointGraphs = graphAValid && graphBValid && graphA.size() == graphB.size() &&
				std::ranges::none_of(graphA, [&graphB](ObjectGuid guid) { return graphB.contains(guid); });
			report.Check("gate2-distinct-objectguid-graphs", disjointGraphs,
				"nodes=" + std::to_string(graphA.size()));

			const Shared<Model> modelA = Find_GateModel(instanceA);
			const Shared<Model> modelB = Find_GateModel(instanceB);
			const Bool sameMapping = modelA && modelB &&
				Same_Preset(modelA->Get_AnimationPreset(), loaded) &&
				Same_Preset(modelB->Get_AnimationPreset(), loaded);
			nlohmann::json prefabA;
			nlohmann::json prefabB;
			const Bool sameProperties = Save_PrefabCopy(instanceA,
				Work_Directory() / L"instance-a.prefab.json", prefabA) &&
				Save_PrefabCopy(instanceB, Work_Directory() / L"instance-b.prefab.json", prefabB) &&
				Canonicalize_Prefab(prefabA) == Canonicalize_Prefab(prefabB);
			report.Check("gate2-identical-properties-and-animation-mapping",
				sameMapping && sameProperties);

			nlohmann::json sourcePrefab;
			Read_Json(prefabPath, sourcePrefab);
			const size_t objectCountBeforeFailure = GAME_INSTANCE->Get_GameObjects(GateLevel).size();
			nlohmann::json malformedPrefab = sourcePrefab;
			const PrefabGuid malformedGuid = Create_PrefabGuid();
			malformedPrefab["prefabGuid"] = To_String(malformedGuid);
			malformedPrefab["roots"] = nlohmann::json::array({ "not-a-guid" });
			const filesystem::path malformedPath = Work_Directory() / L"malformed.prefab.json";
			Write_Json(malformedPath, malformedPrefab);
			Shared<GameObject> failedRoot;
			const Bool malformedRejected = SUCCEEDED(GAME_INSTANCE->Register_Prefab(
				malformedGuid, malformedPath.wstring())) &&
				FAILED(GAME_INSTANCE->DeSerializePrefabDocument(malformedGuid, failedRoot, GateLevel)) &&
				!failedRoot && GAME_INSTANCE->Get_GameObjects(GateLevel).size() == objectCountBeforeFailure;
			report.Check("gate3-malformed-prefab-rollback", malformedRejected);

			nlohmann::json postLoadPrefab = sourcePrefab;
			const PrefabGuid postLoadGuid = Create_PrefabGuid();
			postLoadPrefab["prefabGuid"] = To_String(postLoadGuid);
			nlohmann::json* embeddedPreset = Find_AnimationPreset(postLoadPrefab);
			if (embeddedPreset)
				(*embeddedPreset)["animations"][0]["path"] =
					"Models/wp0070/Animations/wp0070_wpf000_0000.anim";
			const filesystem::path postLoadPath = Work_Directory() / L"post-load-failure.prefab.json";
			Write_Json(postLoadPath, postLoadPrefab);
			const AnimationPresetSnapshot existingBeforePostLoad = sourceModel->Get_AnimationPreset();
			const Bool postLoadRejected = embeddedPreset && SUCCEEDED(GAME_INSTANCE->Register_Prefab(
				postLoadGuid, postLoadPath.wstring())) &&
				FAILED(GAME_INSTANCE->DeSerializePrefabDocument(postLoadGuid, failedRoot, GateLevel)) &&
				!failedRoot && GAME_INSTANCE->Get_GameObjects(GateLevel).size() == objectCountBeforeFailure &&
				Same_Preset(sourceModel->Get_AnimationPreset(), existingBeforePostLoad);
			report.Check("gate3-post-load-failure-subtree-rollback", postLoadRejected);

			const filesystem::path sceneAPath = Work_Directory() / L"scene-a.json";
			const filesystem::path sceneBPath = Work_Directory() / L"scene-b.json";
			nlohmann::json sceneA;
			nlohmann::json sceneB;
			const Bool sceneSaved = SUCCEEDED(GAME_INSTANCE->SerializeLevel(GateLevel, sceneAPath.wstring())) &&
				Read_Json(sceneAPath, sceneA);
			const Bool sceneLoaded = sceneSaved &&
				SUCCEEDED(GAME_INSTANCE->DeSerializeLevel(sceneAPath.wstring()));
			const Bool sceneResaved = sceneLoaded &&
				SUCCEEDED(GAME_INSTANCE->SerializeLevel(GateLevel, sceneBPath.wstring())) &&
				Read_Json(sceneBPath, sceneB);
			Bool derivedGuidsValid = sceneResaved;
			if (derivedGuidsValid)
			{
				for (const auto& [instanceId, object] : GAME_INSTANCE->Get_GameObjects(GateLevel))
					if (object && !object->Get_Parent() && !Verify_DerivedChildren(object))
						derivedGuidsValid = false;
			}
			const Bool semanticRoundTrip = sceneResaved &&
				Normalize_Scene(sceneA) == Normalize_Scene(sceneB);
			report.Check("gate4-scene-save-load-save-properties-preset", semanticRoundTrip);
			report.Check("gate4-deterministic-factory-child-guids", derivedGuidsValid);

			const Bool reportSaved = report.Save(Work_Directory() / L"phase5-entry-gates.json");
			return report.Passed() && reportSaved ? S_OK : E_FAIL;
		}
	}

	uint32 Get_RequestedStage()
	{
#ifdef _DEBUG
		wchar_t value[32]{};
		size_t length = 0;
		_wgetenv_s(&length, value, L"NIER_PHASE5_GATE_STAGE");
		if (wstring_view(value) == L"prepare")
			return PrepareStage;
		if (wstring_view(value) == L"verify")
			return VerifyStage;
#endif
		return 0;
	}

	HRESULT Initialize_Runtime(uint32 stage)
	{
		if (stage != PrepareStage && stage != VerifyStage)
			return E_INVALIDARG;
		if (FAILED(Client::Register_Client_Reflection()) ||
			FAILED(GAME_INSTANCE->Refresh_ReflectionRegistry()) ||
			FAILED(ClientSettingManager::GetInstance()->Apply_LayerAndTagSettings()) ||
			FAILED(GAME_INSTANCE->Register_ReflectedPrototypes(ETOI(LEVEL::STATIC))))
			return E_FAIL;
		if (stage == PrepareStage)
			return S_OK;
		if (FAILED(ClientSettingManager::GetInstance()->Load_Shader()) ||
			FAILED(Load_GateModels()) ||
			FAILED(ClientSettingManager::GetInstance()->Ready_Client_Prototypes(LEVEL::GAMEPLAY)))
			return E_FAIL;
		return S_OK;
	}

	HRESULT Run(uint32 stage, HRESULT initializationResult)
	{
		if (stage == PrepareStage)
			return SUCCEEDED(initializationResult) ? Run_Prepare() : initializationResult;
		if (stage == VerifyStage)
			return Run_Verify(initializationResult);
		return E_INVALIDARG;
	}
}

NS_END
