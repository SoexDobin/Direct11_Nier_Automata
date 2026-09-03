#include "pch.h"
#include "Phase5GateVerifier.h"

#include "AnimationPresetEditor.h"
#include "ClientSettingManager.h"
#include "Client_Function.h"
#include "EditorManager.h"
#include "FreeCamera.h"
#include "Game.h"
#include "GameObject.h"
#include "HpBarWorldUI.h"
#include "Model.h"
#include "PathManager.h"
#include "Pl0000.h"
#include "StaticCamera.h"
#include "TextUI.h"
#include "Transform.h"

using namespace Engine;

NS_BEGIN(Editor)

namespace Phase5GateVerifier
{
	namespace
	{
		constexpr uint32 PrepareStage = 1;
		constexpr uint32 VerifyStage = 2;
		constexpr uint32 FrameStage = 3;
		constexpr uint32 UIAuthoringStage = 4;
		constexpr uint32 ReferencesStage = 5;
		constexpr uint32 PrefabRepositoryStage = 6;
		constexpr uint32 SceneAuthoringStage = 7;
		constexpr uint32 SceneRestartStage = 8;
		constexpr uint32 EditorCutoverStage = 9;
		constexpr uint32 GateLevel = ETOI(LEVEL::GAMEPLAY);
		constexpr uint32 StaticLevel = ETOI(LEVEL::STATIC);
		constexpr const wchar_t* FrameFixtureTag = L"TextUI";
		constexpr const wchar_t* UIFixtureTag = L"TextUI";
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

		HRESULT Load_SceneGateTexture()
		{
			const filesystem::path texturePath = filesystem::path(PATH.GetResourceDir()) /
				L"Models/pl0000/Textures/0454E8A4.dds";
			if (!filesystem::is_regular_file(texturePath))
				return E_FAIL;
			return GAME_INSTANCE->Load_Texture(GateLevel, texturePath.c_str(), 1,
				L"UI_Hp_Bar");
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

		template <typename T>
		Shared<T> Find_In_Graph(const Shared<GameObject>& root)
		{
			if (!root)
				return nullptr;
			if (Shared<T> typed = dynamic_pointer_cast<T>(root))
				return typed;
			for (const Shared<GameObject>& child : root->Get_Children())
				if (Shared<T> typed = Find_In_Graph<T>(child))
					return typed;
			return nullptr;
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

		HRESULT Run_Frame(HRESULT initializationResult)
		{
			GateReport report("frame");
			report.Check("runtime-initialization", SUCCEEDED(initializationResult));
			if (FAILED(initializationResult))
			{
				report.Save(Work_Directory() / L"phase5a-frame-gates.json");
				return E_FAIL;
			}

			const auto RuntimeFrameCount = []() {
				return GAME_INSTANCE->Get_RuntimeFrameCount();
			};
			const auto NearlyEqual = [](Float lhs, Float rhs) {
				return abs(lhs - rhs) <= 0.00001f;
			};

			EDITOR->Set_State(EDITOR_STATE::STOP);
			const uint64_t beforeStop = RuntimeFrameCount();
			EDITOR->Update(false);
			report.Check("stop-world-zero", RuntimeFrameCount() == beforeStop);

			EDITOR->Set_State(EDITOR_STATE::PAUSE);
			const uint64_t beforePause = RuntimeFrameCount();
			EDITOR->Update(false);
			report.Check("pause-world-zero", RuntimeFrameCount() == beforePause);

			EDITOR->Request_SingleStep();
			const uint64_t beforeStep = RuntimeFrameCount();
			EDITOR->Update(false);
			const Bool steppedOnce = RuntimeFrameCount() == beforeStep + 1 &&
				GAME_INSTANCE->Get_LastRuntimeFixedStepCount() == 1 &&
				NearlyEqual(GAME_INSTANCE->Get_LastRuntimeDelta(),
					GAME_INSTANCE->Get_FixedDeltaTime());
			report.Check("single-step-fixed-once", steppedOnce);
			report.Check("single-step-returns-pause",
				EDITOR->Get_State() == EDITOR_STATE::PAUSE);

			const uint64_t afterStep = RuntimeFrameCount();
			EDITOR->Update(false);
			report.Check("pause-after-step-world-zero", RuntimeFrameCount() == afterStep);

			EDITOR->Set_State(EDITOR_STATE::PLAY);
			const uint64_t beforePlay = RuntimeFrameCount();
			EDITOR->Update(false);
			report.Check("play-world-once", RuntimeFrameCount() == beforePlay + 1);
			EDITOR->Set_State(EDITOR_STATE::STOP);

			Shared<TextUI> root =
				GAME_INSTANCE->Instantiate<TextUI>(FrameFixtureTag, StaticLevel);
			Shared<TextUI> child =
				GAME_INSTANCE->Instantiate<TextUI>(FrameFixtureTag, StaticLevel);
			Shared<TextUI> unrelated =
				GAME_INSTANCE->Instantiate<TextUI>(FrameFixtureTag, StaticLevel);
			const Bool graphReady = root && child && unrelated &&
				SUCCEEDED(root->Add_Child(child));
			report.Check("mutation-test-graph-created", graphReady);

			if (graphReady)
			{
				root->Set_Name(L"Phase5A_DeleteRoot");
				child->Set_Name(L"Phase5A_DeleteChild");
				unrelated->Set_Name(L"Phase5A_Unrelated");

				const ObjectGuid rootGuid = root->Get_ObjectGuid();
				const ObjectGuid childGuid = child->Get_ObjectGuid();
				const ObjectGuid unrelatedGuid = unrelated->Get_ObjectGuid();
				const RuntimeObjectId rootRuntimeId = root->Get_RuntimeObjectId();
				const RuntimeObjectId childRuntimeId = child->Get_RuntimeObjectId();
				const RuntimeObjectId unrelatedRuntimeId = unrelated->Get_RuntimeObjectId();
				const wstring unrelatedName = unrelated->Get_Name();

				EDITOR->Set_SelectedObject(child);
				EDITOR->Queue_Destroy(rootGuid, StaticLevel);
				const Bool stillLiveBeforeFlush =
					GAME_INSTANCE->Find(rootGuid) == root &&
					GAME_INSTANCE->Find(childGuid) == child &&
					GAME_INSTANCE->Find(unrelatedGuid) == unrelated &&
					!root->Is_Destroy() && !child->Is_Destroy();
				report.Check("mutation-deferred-until-next-update", stillLiveBeforeFlush);

				EDITOR->Update(false);
				const Bool subtreeIndexesCleared =
					!GAME_INSTANCE->Find(rootGuid) && !GAME_INSTANCE->Find(childGuid) &&
					!GAME_INSTANCE->Find(rootRuntimeId) && !GAME_INSTANCE->Find(childRuntimeId);
				report.Check("subtree-delete-clears-live-indexes", subtreeIndexesCleared);

				const Bool unrelatedPreserved =
					GAME_INSTANCE->Find(unrelatedGuid) == unrelated &&
					GAME_INSTANCE->Find(unrelatedRuntimeId) == unrelated &&
					unrelated->Get_Name() == unrelatedName && !unrelated->Is_Destroy();
				report.Check("subtree-delete-preserves-unrelated-root", unrelatedPreserved);
				report.Check("deleted-subtree-selection-only-cleared",
					!EDITOR->Get_SelectedObject());
			}

			Shared<TextUI> codeRoot =
				GAME_INSTANCE->Instantiate<TextUI>(FrameFixtureTag, StaticLevel);
			Shared<TextUI> codeChild =
				GAME_INSTANCE->Instantiate<TextUI>(FrameFixtureTag, StaticLevel);
			const Bool codeGraphReady = codeRoot && codeChild &&
				SUCCEEDED(codeRoot->Add_Child(codeChild, L"StableChild"));
			if (codeGraphReady)
			{
				EDITOR->Set_SelectedObject(codeChild);
				EDITOR->Queue_Destroy(codeChild->Get_ObjectGuid(), StaticLevel);
				EDITOR->Update(false);
			}
			report.Check("code-defined-delete-rejected", codeGraphReady &&
				GAME_INSTANCE->Find(codeChild->Get_ObjectGuid()) == codeChild &&
				EDITOR->Get_SelectedObject() == codeChild);

			Shared<TextUI> cycleRoot =
				GAME_INSTANCE->Instantiate<TextUI>(FrameFixtureTag, StaticLevel);
			Shared<TextUI> cycleChild =
				GAME_INSTANCE->Instantiate<TextUI>(FrameFixtureTag, StaticLevel);
			const Bool cycleGraphReady = cycleRoot && cycleChild &&
				SUCCEEDED(cycleRoot->Add_Child(cycleChild));
			if (cycleGraphReady)
			{
				EDITOR->Queue_Reparent(cycleRoot->Get_ObjectGuid(),
					cycleChild->Get_ObjectGuid(), StaticLevel);
				EDITOR->Update(false);
			}
			report.Check("reparent-cycle-rejected", cycleGraphReady &&
				!cycleRoot->Get_Parent() && cycleChild->Get_Parent() == cycleRoot);

			if (unrelated)
				GAME_INSTANCE->Destroy(unrelated->Get_ObjectGuid());
			if (codeRoot)
				GAME_INSTANCE->Destroy(codeRoot->Get_ObjectGuid());
			if (cycleRoot)
				GAME_INSTANCE->Destroy(cycleRoot->Get_ObjectGuid());
			GAME_INSTANCE->Flush_DestroyedGameObjects();
			EDITOR->Clear_SelectedObject();
			EDITOR->Set_State(EDITOR_STATE::STOP);

			const Bool reportSaved = report.Save(
				Work_Directory() / L"phase5a-frame-gates.json");
			return report.Passed() && reportSaved ? S_OK : E_FAIL;
		}

		HRESULT Run_UIAuthoring(HRESULT initializationResult)
		{
			GateReport report("ui-authoring");
			report.Check("runtime-initialization", SUCCEEDED(initializationResult));
			if (FAILED(initializationResult))
			{
				report.Save(Work_Directory() / L"phase5b-ui-authoring-gates.json");
				return E_FAIL;
			}

			const auto CreateText = []() {
				return GAME_INSTANCE->Instantiate<TextUI>(UIFixtureTag, StaticLevel);
			};
			Shared<TextUI> root = CreateText();
			Shared<TextUI> childA = CreateText();
			Shared<TextUI> childB = CreateText();
			Shared<TextUI> grandChild = CreateText();
			Shared<TextUI> greatGrandChild = CreateText();
			const Bool graphCreated = root && childA && childB && grandChild && greatGrandChild &&
				SUCCEEDED(root->Add_Child(childA)) &&
				SUCCEEDED(root->Add_Child(childB)) &&
				SUCCEEDED(childA->Add_Child(grandChild)) &&
				SUCCEEDED(grandChild->Add_Child(greatGrandChild));
			report.Check("ui-root-three-depth-created", graphCreated);
			if (!graphCreated)
			{
				report.Save(Work_Directory() / L"phase5b-ui-authoring-gates.json");
				return E_FAIL;
			}

			root->Set_Name(L"Phase5B_UIRoot");
			childA->Set_Name(L"Phase5B_A");
			childB->Set_Name(L"Phase5B_B");
			grandChild->Set_Name(L"Phase5B_GrandChild");
			greatGrandChild->Set_Name(L"Phase5B_GreatGrandChild");
			root->Set_AnchorState(UI_ANCHOR::TOP_LEFT);
			childA->Set_AnchorState(UI_ANCHOR::TOP_RIGHT);
			childB->Set_AnchorState(UI_ANCHOR::BOTTOM_LEFT);
			grandChild->Set_AnchorState(UI_ANCHOR::BOTTOM_RIGHT);
			greatGrandChild->Set_AnchorState(UI_ANCHOR::CENTER);

			report.Check("registry-authoring-policy",
				EDITOR->Can_EditHierarchy(root) && EDITOR->Can_EditChildren(root) &&
				EDITOR->Can_EditHierarchy(grandChild));
			ReflectedTypeInfo codeDefinedInfo;
			ReflectedTypeInfo editorDefinedInfo;
			ReflectedTypeInfo leafInfo;
			ReflectedTypeInfo transientInfo;
			ReflectedTypeInfo defaultCodeDefinedInfo;
			const Bool authoringModesRegistered =
				SUCCEEDED(GAME_INSTANCE->Find_ReflectedType("Pl0000", codeDefinedInfo)) &&
				codeDefinedInfo.authoringMode == HIERARCHY_AUTHORING_MODE::CODE_DEFINED &&
				SUCCEEDED(GAME_INSTANCE->Find_ReflectedType("TextUI", editorDefinedInfo)) &&
				editorDefinedInfo.authoringMode == HIERARCHY_AUTHORING_MODE::EDITOR_DEFINED &&
				SUCCEEDED(GAME_INSTANCE->Find_ReflectedType("Pl0000EvadeChecker", leafInfo)) &&
				leafInfo.authoringMode == HIERARCHY_AUTHORING_MODE::LEAF &&
				SUCCEEDED(GAME_INSTANCE->Find_ReflectedType("Bullet", transientInfo)) &&
				transientInfo.authoringMode == HIERARCHY_AUTHORING_MODE::TRANSIENT &&
				SUCCEEDED(GAME_INSTANCE->Find_ReflectedType("StaticCamera", defaultCodeDefinedInfo)) &&
				defaultCodeDefinedInfo.authoringMode == HIERARCHY_AUTHORING_MODE::CODE_DEFINED;
			report.Check("four-authoring-modes-registered", authoringModesRegistered);

			ReflectionValue anchorValue;
			const Bool anchorReflected = SUCCEEDED(GAME_INSTANCE->Read_ReflectedProperty(
				*grandChild, "Anchor", anchorValue)) &&
				anchorValue.Try_Get<uint32>() &&
				*anchorValue.Try_Get<uint32>() == ETOI(UI_ANCHOR::BOTTOM_RIGHT);
			report.Check("ui-anchor-reflected", anchorReflected);

			const PrefabGuid snapshotGuid = Create_PrefabGuid();
			string snapshot;
			nlohmann::json snapshotJson;
			Bool snapshotSaved = snapshotGuid.Is_Valid() &&
				SUCCEEDED(GAME_INSTANCE->SerializeSubtreeSnapshot(
					snapshotGuid, root, snapshot, StaticLevel));
			try
			{
				if (snapshotSaved)
					snapshotJson = nlohmann::json::parse(snapshot);
			}
			catch (...)
			{
				snapshotSaved = false;
			}
			const string grandGuid = To_String(grandChild->Get_ObjectGuid());
			const Bool anchorSerialized = snapshotSaved &&
				snapshotJson["objects"].contains(grandGuid) &&
				snapshotJson["objects"][grandGuid]["properties"].value(
					"Anchor", UINT_MAX) == ETOI(UI_ANCHOR::BOTTOM_RIGHT);
			report.Check("ui-anchor-memory-snapshot", anchorSerialized);

			EDITOR->Queue_Reorder(childB->Get_ObjectGuid(), 0, StaticLevel);
			EDITOR->Update(false);
			const Bool reordered = root->Get_Children().size() == 2 &&
				root->Get_Children()[0] == childB && root->Get_Children()[1] == childA;
			report.Check("same-parent-reorder", reordered);
			EDITOR->Queue_Undo();
			EDITOR->Update(false);
			const Bool reorderUndone = root->Get_Children()[0] == childA &&
				root->Get_Children()[1] == childB;
			EDITOR->Queue_Redo();
			EDITOR->Update(false);
			const Bool reorderRedone = root->Get_Children()[0] == childB &&
				root->Get_Children()[1] == childA;
			report.Check("reorder-undo-redo", reorderUndone && reorderRedone);
			const Bool invalidReorderRejected =
				root->Reorder_Child(childB, 0) == S_FALSE &&
				FAILED(root->Reorder_Child(childB, root->Get_Children().size())) &&
				root->Get_Children()[0] == childB && root->Get_Children()[1] == childA;
			report.Check("reorder-noop-out-of-range-preserves-order", invalidReorderRejected);

			EDITOR->Queue_Reparent(grandChild->Get_ObjectGuid(),
				childB->Get_ObjectGuid(), StaticLevel, 0);
			EDITOR->Update(false);
			const Bool reparented = grandChild->Get_Parent() == childB;
			EDITOR->Queue_Undo();
			EDITOR->Update(false);
			const Bool reparentUndone = grandChild->Get_Parent() == childA;
			EDITOR->Queue_Redo();
			EDITOR->Update(false);
			const Bool reparentRedone = grandChild->Get_Parent() == childB;
			report.Check("reparent-undo-redo", reparented && reparentUndone && reparentRedone);

			EDITOR->Queue_Spawn(UIFixtureTag, StaticLevel, root->Get_ObjectGuid());
			EDITOR->Update(false);
			const Shared<GameObject> spawned = EDITOR->Get_SelectedObject();
			const ObjectGuid spawnedGuid = spawned ? spawned->Get_ObjectGuid() : ObjectGuid{};
			const Bool added = spawned && spawned->Get_Parent() == root;
			EDITOR->Queue_Undo();
			EDITOR->Update(false);
			const Bool addUndone = spawnedGuid.Is_Valid() && !GAME_INSTANCE->Find(spawnedGuid);
			EDITOR->Queue_Redo();
			EDITOR->Update(false);
			const Shared<GameObject> restoredSpawn = GAME_INSTANCE->Find(spawnedGuid);
			const Bool addRedone = restoredSpawn && restoredSpawn->Get_Parent() == root;
			report.Check("add-undo-redo", added && addUndone && addRedone);

			EDITOR->Queue_Destroy(spawnedGuid, StaticLevel);
			EDITOR->Update(false);
			const Bool removed = !GAME_INSTANCE->Find(spawnedGuid);
			EDITOR->Queue_Undo();
			EDITOR->Update(false);
			const Bool removeUndone = GAME_INSTANCE->Find(spawnedGuid) != nullptr;
			report.Check("remove-undo", removed && removeUndone);

			EDITOR->Queue_Duplicate(root->Get_ObjectGuid(), StaticLevel);
			EDITOR->Update(false);
			const Shared<GameObject> duplicate = EDITOR->Get_SelectedObject();
			const ObjectGuid duplicateGuid = duplicate ? duplicate->Get_ObjectGuid() : ObjectGuid{};
			unordered_set<ObjectGuid, GuidHash> originalGraph;
			unordered_set<ObjectGuid, GuidHash> duplicateGraph;
			const Shared<UIObject> duplicateUI = dynamic_pointer_cast<UIObject>(duplicate);
			const Bool duplicated = duplicate && duplicate != root &&
				Collect_Graph(root, originalGraph) && Collect_Graph(duplicate, duplicateGraph) &&
				originalGraph.size() == duplicateGraph.size() &&
				std::ranges::none_of(originalGraph, [&duplicateGraph](ObjectGuid guid) {
					return duplicateGraph.contains(guid);
				}) && duplicateUI && duplicateUI->Get_AnchorState() == UI_ANCHOR::TOP_LEFT &&
				duplicate->Get_Children().size() == root->Get_Children().size();
			report.Check("duplicate-three-depth-distinct-guids-anchor-order", duplicated);
			EDITOR->Queue_Undo();
			EDITOR->Update(false);
			const Bool duplicateUndone = duplicateGuid.Is_Valid() && !GAME_INSTANCE->Find(duplicateGuid);
			EDITOR->Queue_Redo();
			EDITOR->Update(false);
			const Shared<GameObject> duplicateRestored = GAME_INSTANCE->Find(duplicateGuid);
			const Shared<UIObject> duplicateRestoredUI =
				dynamic_pointer_cast<UIObject>(duplicateRestored);
			const Bool duplicateRedone = duplicateRestored && duplicateRestoredUI &&
				duplicateRestoredUI->Get_AnchorState() == UI_ANCHOR::TOP_LEFT;
			report.Check("duplicate-undo-redo-preserves-guid-anchor", duplicateUndone && duplicateRedone);

			if (root)
				GAME_INSTANCE->Destroy(root->Get_ObjectGuid());
			if (duplicateRestored)
				GAME_INSTANCE->Destroy(duplicateRestored->Get_ObjectGuid());
			GAME_INSTANCE->Flush_DestroyedGameObjects();
			EDITOR->Clear_SelectedObject();

			const Bool reportSaved = report.Save(
				Work_Directory() / L"phase5b-ui-authoring-gates.json");
			return report.Passed() && reportSaved ? S_OK : E_FAIL;
		}

		HRESULT Run_References(HRESULT initializationResult)
		{
			GateReport report("references");
			report.Check("runtime-initialization", SUCCEEDED(initializationResult));
			if (FAILED(initializationResult))
			{
				report.Save(Work_Directory() / L"phase6-references-gates.json");
				return E_FAIL;
			}

			const vector<ReflectedPropertyInfo> hpBarProperties =
				GAME_INSTANCE->Get_ReflectedProperties("HpBarWorldUI");
			const auto hpBarTargetIt = std::ranges::find(
				hpBarProperties, string{ "Target" }, &ReflectedPropertyInfo::registeredName);
			const Bool hpBarDescriptorRegistered = hpBarTargetIt != hpBarProperties.end() &&
				hpBarTargetIt->valueType == REFLECTION_VALUE_TYPE::OBJECT_REF &&
				hpBarTargetIt->expectedBaseRegisteredName == "Entity" &&
				hpBarTargetIt->saveDataKey == Save_Data_Key::ObjectReference &&
				hpBarTargetIt->isWritable && hpBarTargetIt->isSerializable;
			report.Check("hpbar-objectref-descriptor", hpBarDescriptorRegistered);

			const auto CreateText = []() {
				return GAME_INSTANCE->Instantiate<TextUI>(UIFixtureTag, GateLevel);
			};
			Shared<TextUI> root = CreateText();
			Shared<TextUI> internalTarget = CreateText();
			Shared<TextUI> externalTarget = CreateText();
			Shared<StaticCamera> internalBinding =
				GAME_INSTANCE->Instantiate<StaticCamera>(GateLevel);
			Shared<FreeCamera> externalBinding =
				GAME_INSTANCE->Instantiate<FreeCamera>(GateLevel);
			const Bool graphCreated = root && internalTarget && externalTarget &&
				internalBinding && externalBinding &&
				SUCCEEDED(root->Add_Child(internalTarget)) &&
				SUCCEEDED(root->Add_Child(internalBinding)) &&
				SUCCEEDED(root->Add_Child(externalBinding));
			report.Check("resource-independent-binding-graph", graphCreated);
			if (!graphCreated)
			{
				if (root)
					GAME_INSTANCE->Destroy(root->Get_ObjectGuid());
				if (externalTarget)
					GAME_INSTANCE->Destroy(externalTarget->Get_ObjectGuid());
				GAME_INSTANCE->Flush_DestroyedGameObjects();
				report.Save(Work_Directory() / L"phase6-references-gates.json");
				return E_FAIL;
			}

			root->Set_Name(L"Phase6_ReferenceRoot");
			internalTarget->Set_Name(L"Phase6_InternalTarget");
			externalTarget->Set_Name(L"Phase6_ExternalTarget");

			HpBarWorldUI hpBarFixture;
			const ObjectGuid unresolvedGuid = Create_ObjectGuid();
			const Bool unresolvedPreserved = unresolvedGuid.Is_Valid() &&
				SUCCEEDED(hpBarFixture.Set_TargetObjectGuid(unresolvedGuid));
			hpBarFixture.Late_Update(0.f);
			hpBarFixture.Submit_RenderGroup();
			report.Check("hpbar-unresolved-remains-authorable", unresolvedPreserved &&
				hpBarFixture.Get_TargetObjectGuid() == unresolvedGuid &&
				!hpBarFixture.Is_Destroy());
			const Bool incompatibleRejected =
				FAILED(hpBarFixture.Set_TargetObjectGuid(externalTarget->Get_ObjectGuid())) &&
				hpBarFixture.Get_TargetObjectGuid() == unresolvedGuid;
			report.Check("hpbar-incompatible-live-target-rejected", incompatibleRejected);
			hpBarFixture.Set_TargetObjectGuid({});

			ReflectionValue internalValue;
			internalValue.data = internalTarget->Get_ObjectGuid();
			ReflectionValue externalValue;
			externalValue.data = externalTarget->Get_ObjectGuid();
			ReflectionValue readValue;
			const Bool guidReadWrite =
				SUCCEEDED(GAME_INSTANCE->Write_ReflectedProperty(
					*internalBinding, "Target", internalValue)) &&
				SUCCEEDED(GAME_INSTANCE->Read_ReflectedProperty(
					*internalBinding, "Target", readValue)) &&
				readValue.Try_Get<ObjectGuid>() &&
				*readValue.Try_Get<ObjectGuid>() == internalTarget->Get_ObjectGuid() &&
				SUCCEEDED(GAME_INSTANCE->Write_ReflectedProperty(
					*externalBinding, "Target", externalValue));
			report.Check("registry-objectguid-read-write", guidReadWrite);

			const PrefabGuid snapshotGuid = Create_PrefabGuid();
			string snapshot;
			nlohmann::json snapshotJson;
			Bool snapshotReady = snapshotGuid.Is_Valid() && guidReadWrite &&
				SUCCEEDED(GAME_INSTANCE->SerializeSubtreeSnapshot(
					snapshotGuid, root, snapshot, GateLevel));
			try
			{
				if (snapshotReady)
					snapshotJson = nlohmann::json::parse(snapshot);
			}
			catch (...)
			{
				snapshotReady = false;
			}
			const string internalBindingGuid = To_String(internalBinding->Get_ObjectGuid());
			const string externalBindingGuid = To_String(externalBinding->Get_ObjectGuid());
			const Bool guidOnlyJson = snapshotReady &&
				snapshotJson["objects"][internalBindingGuid]["properties"]["Target"] ==
					To_String(internalTarget->Get_ObjectGuid()) &&
				snapshotJson["objects"][externalBindingGuid]["properties"]["Target"] ==
					To_String(externalTarget->Get_ObjectGuid());
			report.Check("objectref-guid-only-json", guidOnlyJson);

			nlohmann::json malformedJson = snapshotJson;
			if (snapshotReady)
				malformedJson["objects"][internalBindingGuid]["properties"]["Target"] =
					"not-an-object-guid";
			const size_t objectCountBeforeMalformed =
				GAME_INSTANCE->Get_GameObjects(GateLevel).size();
			Shared<GameObject> malformedRoot;
			const Bool malformedRejected = snapshotReady &&
				FAILED(GAME_INSTANCE->DeSerializeSubtreeSnapshot(snapshotGuid,
					malformedJson.dump(), false, malformedRoot, GateLevel)) &&
				!malformedRoot && GAME_INSTANCE->Get_GameObjects(GateLevel).size() ==
					objectCountBeforeMalformed;
			report.Check("malformed-objectref-preflight-rollback", malformedRejected);

			GAME_INSTANCE->Destroy(root->Get_ObjectGuid());
			GAME_INSTANCE->Flush_DestroyedGameObjects();
			root.reset();
			internalTarget.reset();
			internalBinding.reset();
			externalBinding.reset();

			Shared<GameObject> restoredA;
			const Bool restoredFirst = snapshotReady &&
				SUCCEEDED(GAME_INSTANCE->DeSerializeSubtreeSnapshot(
					snapshotGuid, snapshot, false, restoredA, GateLevel));
			Shared<StaticCamera> internalA = Find_In_Graph<StaticCamera>(restoredA);
			Shared<FreeCamera> externalA = Find_In_Graph<FreeCamera>(restoredA);
			const Shared<GameObject> internalTargetA = restoredA && !restoredA->Get_Children().empty()
				? restoredA->Get_Children().front()
				: nullptr;
			unordered_set<ObjectGuid, GuidHash> graphA;
			const Bool firstRemapCorrect = restoredFirst && internalA && externalA &&
				internalTargetA && Collect_Graph(restoredA, graphA) &&
				internalA->Get_TargetObjectGuid() == internalTargetA->Get_ObjectGuid() &&
				internalValue.Try_Get<ObjectGuid>() &&
				internalA->Get_TargetObjectGuid() != *internalValue.Try_Get<ObjectGuid>() &&
				externalA->Get_TargetObjectGuid() == externalTarget->Get_ObjectGuid() &&
				externalA->Get_Target() == externalTarget;
			report.Check("internal-remap-external-reference-preserved", firstRemapCorrect);

			if (restoredA)
				GAME_INSTANCE->Destroy(restoredA->Get_ObjectGuid());
			GAME_INSTANCE->Flush_DestroyedGameObjects();
			restoredA.reset();
			internalA.reset();
			externalA.reset();

			Shared<GameObject> restoredB;
			const Bool restoredSecond = snapshotReady &&
				SUCCEEDED(GAME_INSTANCE->DeSerializeSubtreeSnapshot(
					snapshotGuid, snapshot, false, restoredB, GateLevel));
			const Shared<StaticCamera> internalB = Find_In_Graph<StaticCamera>(restoredB);
			const Shared<FreeCamera> externalB = Find_In_Graph<FreeCamera>(restoredB);
			const Shared<GameObject> internalTargetB = restoredB && !restoredB->Get_Children().empty()
				? restoredB->Get_Children().front()
				: nullptr;
			unordered_set<ObjectGuid, GuidHash> graphB;
			const Bool secondInstanceCorrect = restoredSecond && internalB && externalB &&
				internalTargetB && Collect_Graph(restoredB, graphB) &&
				std::ranges::none_of(graphA, [&graphB](ObjectGuid guid) {
					return graphB.contains(guid);
				}) && internalB->Get_TargetObjectGuid() == internalTargetB->Get_ObjectGuid() &&
				externalB->Get_TargetObjectGuid() == externalTarget->Get_ObjectGuid();
			report.Check("two-instantiates-own-internal-binding", secondInstanceCorrect);

			ReflectionValue nullValue;
			nullValue.data = ObjectGuid{};
			const PrefabGuid nullSnapshotGuid = Create_PrefabGuid();
			string nullSnapshot;
			nlohmann::json nullSnapshotJson;
			Bool nullReady = externalB &&
				SUCCEEDED(GAME_INSTANCE->Write_ReflectedProperty(
					*externalB, "Target", nullValue)) &&
				SUCCEEDED(GAME_INSTANCE->SerializeSubtreeSnapshot(
					nullSnapshotGuid, restoredB, nullSnapshot, GateLevel));
			try
			{
				if (nullReady)
					nullSnapshotJson = nlohmann::json::parse(nullSnapshot);
			}
			catch (...)
			{
				nullReady = false;
			}
			const Bool nullEncoded = nullReady &&
				nullSnapshotJson["objects"][To_String(externalB->Get_ObjectGuid())]
					["properties"]["Target"].is_null();
			if (restoredB)
				GAME_INSTANCE->Destroy(restoredB->Get_ObjectGuid());
			GAME_INSTANCE->Flush_DestroyedGameObjects();
			restoredB.reset();

			Shared<GameObject> nullRestored;
			const Bool nullDecoded = nullEncoded &&
				SUCCEEDED(GAME_INSTANCE->DeSerializeSubtreeSnapshot(
					nullSnapshotGuid, nullSnapshot, false, nullRestored, GateLevel));
			const Shared<FreeCamera> nullExternal =
				Find_In_Graph<FreeCamera>(nullRestored);
			report.Check("null-objectref-json-roundtrip", nullDecoded && nullExternal &&
				!nullExternal->Get_TargetObjectGuid().Is_Valid());

			if (nullRestored)
				GAME_INSTANCE->Destroy(nullRestored->Get_ObjectGuid());
			if (externalTarget)
				GAME_INSTANCE->Destroy(externalTarget->Get_ObjectGuid());
			GAME_INSTANCE->Flush_DestroyedGameObjects();

			const Bool reportSaved = report.Save(
				Work_Directory() / L"phase6-references-gates.json");
			return report.Passed() && reportSaved ? S_OK : E_FAIL;
		}

		HRESULT Run_PrefabRepository(HRESULT initializationResult)
		{
			GateReport report("prefab-repository");
			report.Check("runtime-initialization", SUCCEEDED(initializationResult));
			if (FAILED(initializationResult))
			{
				report.Save(Work_Directory() / L"phase6-prefab-repository-gates.json");
				return E_FAIL;
			}

			const filesystem::path fixtureRoot = Work_Directory() /
				(L"prefab-repository-" + std::to_wstring(GetCurrentProcessId()));
			const filesystem::path validDirectory = fixtureRoot / L"valid";
			const filesystem::path duplicateDirectory = fixtureRoot / L"duplicate";
			const filesystem::path malformedDirectory = fixtureRoot / L"malformed";
			const filesystem::path invalidGuidDirectory = fixtureRoot / L"invalid-guid";
			const filesystem::path emptyDirectory = fixtureRoot / L"empty";
			const PrefabGuid guidA = Create_PrefabGuid();
			const PrefabGuid guidB = Create_PrefabGuid();
			const auto MakeHeader = [](PrefabGuid prefabGuid) {
				return nlohmann::json{
					{ "schemaVersion", 2 },
					{ "documentType", "Prefab" },
					{ "prefabGuid", To_String(prefabGuid) }
				};
			};
			std::error_code errorCode;
			filesystem::create_directories(emptyDirectory, errorCode);
			const Bool fixturesWritten = !errorCode && guidA.Is_Valid() && guidB.Is_Valid() &&
				Write_Json(validDirectory / L"a.json", MakeHeader(guidA)) &&
				Write_Json(validDirectory / L"z.JSON", MakeHeader(guidB)) &&
				Write_Json(validDirectory / L"ignored.txt", MakeHeader(guidA)) &&
				Write_Json(duplicateDirectory / L"a.json", MakeHeader(guidA)) &&
				Write_Json(duplicateDirectory / L"b.json", MakeHeader(guidA)) &&
				Write_Json(malformedDirectory / L"a.json", MakeHeader(guidA)) &&
				Write_Json(malformedDirectory / L"broken.json", nlohmann::json::array()) &&
				Write_Json(invalidGuidDirectory / L"a.json", MakeHeader(guidA)) &&
				Write_Json(invalidGuidDirectory / L"invalid.json", {
					{ "schemaVersion", 2 },
					{ "documentType", "Prefab" },
					{ "prefabGuid", "00000000-0000-0000-0000-000000000000" }
				});
			report.Check("repository-fixtures-written", fixturesWritten);
			if (!fixturesWritten)
			{
				report.Save(Work_Directory() / L"phase6-prefab-repository-gates.json");
				return E_FAIL;
			}

			const Bool missingDirectoryAccepted =
				SUCCEEDED(GAME_INSTANCE->Load_PrefabRepository(
					(fixtureRoot / L"missing").wstring())) &&
				GAME_INSTANCE->Get_PrefabDocuments().empty();
			report.Check("missing-directory-is-empty-repository", missingDirectoryAccepted);

			const Bool validLoaded = SUCCEEDED(GAME_INSTANCE->Load_PrefabRepository(
				validDirectory.wstring()));
			const vector<pair<PrefabGuid, wstring>> validDocuments =
				GAME_INSTANCE->Get_PrefabDocuments();
			const Bool deterministicAndFiltered = validLoaded && validDocuments.size() == 2 &&
				validDocuments[0].first == guidA && validDocuments[1].first == guidB &&
				filesystem::path(validDocuments[0].second).filename() == L"a.json" &&
				filesystem::path(validDocuments[1].second).filename() == L"z.JSON";
			report.Check("json-only-deterministic-repository", deterministicAndFiltered);

			const HRESULT samePathResult = GAME_INSTANCE->Register_Prefab(
				guidA, (validDirectory / L"." / L"a.json").wstring());
			PrefabGuid sameDocumentGuid{};
			const Bool samePathIdempotent = samePathResult == S_FALSE &&
				SUCCEEDED(GAME_INSTANCE->Register_PrefabDocument(
					(validDirectory / L"a.json").wstring(), sameDocumentGuid)) &&
				sameDocumentGuid == guidA &&
				GAME_INSTANCE->Get_PrefabDocuments() == validDocuments;
			report.Check("canonical-same-path-idempotent", samePathIdempotent);

			const Bool duplicateRejectedAtomically =
				FAILED(GAME_INSTANCE->Load_PrefabRepository(duplicateDirectory.wstring())) &&
				GAME_INSTANCE->Get_PrefabDocuments() == validDocuments;
			report.Check("duplicate-guid-atomic-rollback", duplicateRejectedAtomically);

			const Bool malformedRejectedAtomically =
				FAILED(GAME_INSTANCE->Load_PrefabRepository(malformedDirectory.wstring())) &&
				GAME_INSTANCE->Get_PrefabDocuments() == validDocuments;
			report.Check("malformed-document-atomic-rollback", malformedRejectedAtomically);

			const Bool invalidGuidRejectedAtomically =
				FAILED(GAME_INSTANCE->Load_PrefabRepository(invalidGuidDirectory.wstring())) &&
				GAME_INSTANCE->Get_PrefabDocuments() == validDocuments;
			report.Check("invalid-guid-atomic-rollback", invalidGuidRejectedAtomically);

			const Bool emptyRepositoryReplacesPrevious =
				SUCCEEDED(GAME_INSTANCE->Load_PrefabRepository(emptyDirectory.wstring())) &&
				GAME_INSTANCE->Get_PrefabDocuments().empty();
			report.Check("empty-directory-replaces-repository", emptyRepositoryReplacesPrevious);

			const Bool repositoryRestored =
				SUCCEEDED(GAME_INSTANCE->Load_PrefabRepository(validDirectory.wstring())) &&
				GAME_INSTANCE->Get_PrefabDocuments() == validDocuments;
			report.Check("folder-reload-restores-same-map", repositoryRestored);

			const Bool reportSaved = report.Save(
				Work_Directory() / L"phase6-prefab-repository-gates.json");
			return report.Passed() && reportSaved ? S_OK : E_FAIL;
		}

		HRESULT Run_SceneAuthoring(HRESULT initializationResult)
		{
			GateReport report("scene-authoring");
			report.Check("runtime-initialization", SUCCEEDED(initializationResult));
			if (FAILED(initializationResult))
			{
				report.Save(Work_Directory() / L"phase6-scene-authoring-gates.json");
				return E_FAIL;
			}

			GAME_INSTANCE->Clear_GameObjects(GateLevel);
			const Shared<Pl0000> player =
				dynamic_pointer_cast<Pl0000>(GAME_INSTANCE->Instantiate_GameObject("Pl0000", GateLevel));
			const Shared<TextUI> uiRoot = GAME_INSTANCE->Instantiate<TextUI>(UIFixtureTag, GateLevel);
			const Shared<TextUI> uiPanel = GAME_INSTANCE->Instantiate<TextUI>(UIFixtureTag, GateLevel);
			const Shared<HpBarWorldUI> hpBar = dynamic_pointer_cast<HpBarWorldUI>(
				GAME_INSTANCE->Instantiate_GameObject("HpBarWorldUI", GateLevel));
			const Bool graphCreated = player && uiRoot && uiPanel && hpBar &&
				SUCCEEDED(uiRoot->Add_Child(uiPanel)) && SUCCEEDED(uiPanel->Add_Child(hpBar));
			report.Check("pl0000-and-three-depth-ui-created", graphCreated);
			if (!graphCreated)
			{
				report.Save(Work_Directory() / L"phase6-scene-authoring-gates.json");
				return E_FAIL;
			}

			player->Set_Name(L"Phase6_Pl0000");
			player->Get_Transform()->Set_Position({ 12.f, 0.f, 8.f });
			uiRoot->Set_Name(L"Phase6_UIRoot");
			uiRoot->Set_AnchorState(UI_ANCHOR::TOP_LEFT);
			uiRoot->Get_Transform()->Set_LocalPosition({ 48.f, 32.f, 0.f });
			uiPanel->Set_Name(L"Phase6_UIPanel");
			uiPanel->Set_AnchorState(UI_ANCHOR::CENTER);
			uiPanel->Get_Transform()->Set_LocalPosition({ 120.f, 64.f, 0.f });
			hpBar->Set_Name(L"Phase6_HpBar");
			hpBar->Set_AnchorState(UI_ANCHOR::TOP_CENTER);
			hpBar->Set_WorldOffset({ 0.f, 2.25f, 0.f });
			const Bool objectRefAssigned =
				SUCCEEDED(hpBar->Set_TargetObjectGuid(player->Get_ObjectGuid()));
			report.Check("hpbar-external-objectref-assigned", objectRefAssigned);

			const filesystem::path prefabDirectory(PATH.GetPrefabSettingsDir());
			const filesystem::path prefabPath = prefabDirectory / L"Phase6_Pl0000.json";
			PrefabGuid prefabGuid{};
			Bool registeredNew = false;
			if (filesystem::exists(prefabPath))
				GAME_INSTANCE->Register_PrefabDocument(prefabPath.wstring(), prefabGuid);
			else
			{
				prefabGuid = Create_PrefabGuid();
				registeredNew = prefabGuid.Is_Valid() &&
					SUCCEEDED(GAME_INSTANCE->Register_Prefab(prefabGuid, prefabPath.wstring()));
			}
			const Bool prefabSaved = prefabGuid.Is_Valid() &&
				SUCCEEDED(GAME_INSTANCE->SerializePrefabDocument(prefabGuid, player, GateLevel));
			if (!prefabSaved && registeredNew)
				GAME_INSTANCE->Unregister_Prefab(prefabGuid);
			report.Check("world-prefab-saved", prefabSaved);

			const filesystem::path scenePath = PATH.GetLevelDataPath(GateLevel);
			const Bool sceneSaved = prefabSaved &&
				SUCCEEDED(GAME_INSTANCE->SerializeLevel(GateLevel, scenePath.wstring()));
			report.Check("project-scene-saved", sceneSaved);
			const Bool repositoryRestored = sceneSaved &&
				SUCCEEDED(GAME_INSTANCE->Load_PrefabRepository(prefabDirectory.wstring())) &&
				GAME_INSTANCE->Find_PrefabPath(prefabGuid) ==
					filesystem::weakly_canonical(prefabPath).wstring();
			report.Check("project-prefab-repository-restored", repositoryRestored);

			const Bool reportSaved = report.Save(
				Work_Directory() / L"phase6-scene-authoring-gates.json");
			return report.Passed() && reportSaved ? S_OK : E_FAIL;
		}

		HRESULT Run_SceneRestart(HRESULT initializationResult)
		{
			GateReport report("scene-restart");
			report.Check("runtime-initialization", SUCCEEDED(initializationResult));
			const filesystem::path prefabDirectory(PATH.GetPrefabSettingsDir());
			const filesystem::path scenePath = PATH.GetLevelDataPath(GateLevel);
			const Bool loaded = SUCCEEDED(initializationResult) &&
				SUCCEEDED(GAME_INSTANCE->Load_PrefabRepository(prefabDirectory.wstring())) &&
				GAME_INSTANCE->Get_PrefabDocuments().size() == 1 &&
				SUCCEEDED(GAME_INSTANCE->DeSerializeLevel(scenePath.wstring()));
			report.Check("restart-repository-and-scene-load", loaded);
			if (!loaded)
			{
				report.Save(Work_Directory() / L"phase6-scene-restart-gates.json");
				return E_FAIL;
			}

			Shared<Pl0000> player;
			Shared<TextUI> uiRoot;
			for (const auto& [instanceId, object] : GAME_INSTANCE->Get_GameObjects(GateLevel))
			{
				if (!object || object->Get_Parent())
					continue;
				if (object->Get_Name() == L"Phase6_Pl0000")
					player = dynamic_pointer_cast<Pl0000>(object);
				else if (object->Get_Name() == L"Phase6_UIRoot")
					uiRoot = dynamic_pointer_cast<TextUI>(object);
			}
			const Shared<TextUI> uiPanel = uiRoot && uiRoot->Get_Children().size() == 1
				? dynamic_pointer_cast<TextUI>(uiRoot->Get_Children()[0]) : nullptr;
			const Shared<HpBarWorldUI> hpBar = uiPanel && uiPanel->Get_Children().size() == 1
				? dynamic_pointer_cast<HpBarWorldUI>(uiPanel->Get_Children()[0]) : nullptr;
			const Bool hierarchyRestored = player && uiRoot && uiPanel && hpBar &&
				uiPanel->Get_Name() == L"Phase6_UIPanel" && hpBar->Get_Name() == L"Phase6_HpBar" &&
				uiRoot->Get_AnchorState() == UI_ANCHOR::TOP_LEFT &&
				uiPanel->Get_AnchorState() == UI_ANCHOR::CENTER &&
				hpBar->Get_AnchorState() == UI_ANCHOR::TOP_CENTER;
			report.Check("ui-order-anchor-three-depth-restored", hierarchyRestored);
			report.Check("external-objectref-restored", hierarchyRestored &&
				hpBar->Get_TargetObjectGuid() == player->Get_ObjectGuid());

			nlohmann::json authoredScene;
			nlohmann::json resavedScene;
			const filesystem::path resavePath = Work_Directory() / L"phase6-scene-resaved.json";
			const Bool semanticEqual = Read_Json(scenePath, authoredScene) &&
				SUCCEEDED(GAME_INSTANCE->SerializeLevel(GateLevel, resavePath.wstring())) &&
				Read_Json(resavePath, resavedScene) &&
				Normalize_Scene(authoredScene) == Normalize_Scene(resavedScene);
			report.Check("restart-save-semantic-equality", semanticEqual);

			const size_t countBeforeFailure = GAME_INSTANCE->Get_GameObjects(GateLevel).size();
			const ObjectGuid playerGuid = player ? player->Get_ObjectGuid() : ObjectGuid{};
			nlohmann::json malformed = authoredScene;
			if (hpBar)
				malformed["objects"][To_String(hpBar->Get_ObjectGuid())]["properties"]["Target"] =
					"invalid-object-guid";
			const filesystem::path malformedPath = Work_Directory() / L"phase6-scene-malformed.json";
			const Bool rollbackPreserved = Write_Json(malformedPath, malformed) &&
				FAILED(GAME_INSTANCE->DeSerializeLevel(malformedPath.wstring())) &&
				GAME_INSTANCE->Get_GameObjects(GateLevel).size() == countBeforeFailure &&
				GAME_INSTANCE->Find(playerGuid) == player;
			report.Check("scene-preflight-failure-preserves-level", rollbackPreserved);

			const auto prefabs = GAME_INSTANCE->Get_PrefabDocuments();
			Shared<GameObject> prefabInstance;
			const Bool prefabInstantiated = prefabs.size() == 1 &&
				SUCCEEDED(GAME_INSTANCE->DeSerializePrefabDocument(
					prefabs[0].first, prefabInstance, GateLevel)) && prefabInstance &&
				prefabInstance->Get_ObjectGuid() != playerGuid;
			report.Check("restart-prefab-instantiates-disjoint-root", prefabInstantiated);

			const Bool reportSaved = report.Save(
				Work_Directory() / L"phase6-scene-restart-gates.json");
			return report.Passed() && reportSaved ? S_OK : E_FAIL;
		}

		HRESULT Run_EditorCutover(HRESULT initializationResult)
		{
			GateReport report("phase7-cutover");
			report.Check("runtime-initialization", SUCCEEDED(initializationResult));
			if (FAILED(initializationResult)) {
				report.Save(Work_Directory() / L"phase7-cutover-gates.json");
				return E_FAIL;
			}

			const auto& prototypes = GAME_INSTANCE->Get_Prototypes(StaticLevel);
			const auto prototypeIt = prototypes.find(FrameFixtureTag);
			const Shared<GameObject> originalPrototype = prototypeIt == prototypes.end()
				? nullptr
				: prototypeIt->second;
			const size_t prototypeCount = prototypes.size();
			const Shared<TextUI> duplicatePrototype = TextUI::CreatePrototype();
			const HRESULT duplicateResult = duplicatePrototype
				? GAME_INSTANCE->Add_Prototype(StaticLevel, duplicatePrototype, FrameFixtureTag)
				: E_FAIL;
			report.Check("prototype-tag-storage-is-atomic",
				originalPrototype && duplicateResult == S_FALSE &&
				prototypes.size() == prototypeCount &&
				prototypes.at(FrameFixtureTag) == originalPrototype);

			const uint32 editorLevel = GAME_INSTANCE->Get_CurrentLevelIndex();
			const Shared<TextUI> fixture =
				GAME_INSTANCE->Instantiate<TextUI>(FrameFixtureTag, editorLevel);
			const Shared<TextUI> typedFixture = GAME_INSTANCE->Instantiate<TextUI>(editorLevel);
			const Shared<Component> wrongKind =
				GAME_INSTANCE->Instantiate<Component>(FrameFixtureTag, editorLevel);
			report.Check("prototype-static-fallback-by-tag", fixture != nullptr);
			report.Check("prototype-static-fallback-by-runtime-type", typedFixture != nullptr);
			report.Check("prototype-wrong-kind-is-rejected", wrongKind == nullptr);
			report.Check("runtime-object-ids-are-unique", fixture && typedFixture &&
				fixture->Get_RuntimeObjectId() != typedFixture->Get_RuntimeObjectId());
			const Shared<Transform> retainedTransform = fixture ? fixture->Get_Transform() : nullptr;
			const Shared<Transform> duplicateTransform =
				GAME_INSTANCE->Instantiate<Transform>(editorLevel);
			const size_t componentCountBeforeDuplicate = fixture
				? fixture->Get_Components().size()
				: 0;
			report.Check("duplicate-component-type-is-rejected", fixture && duplicateTransform &&
				FAILED(fixture->Add_Component(duplicateTransform)) &&
				fixture->Get_Components().size() == componentCountBeforeDuplicate &&
				fixture->Get_Transform() == retainedTransform);

			Shared<GameObject> spawned;
			if (fixture && typedFixture) {
				unordered_set<RuntimeObjectId> runtimeIdsBeforeSpawn;
				for (const auto& [runtimeObjectId, object] :
					GAME_INSTANCE->Get_GameObjects(editorLevel))
					runtimeIdsBeforeSpawn.emplace(runtimeObjectId);
				const size_t objectCountBeforeSpawn = runtimeIdsBeforeSpawn.size();
				const Vector3 spawnPosition{ 7.f, 11.f, 13.f };
				EDITOR->Queue_Spawn(FrameFixtureTag, editorLevel, {},
					numeric_limits<size_t>::max(), spawnPosition);
				report.Check("editor-spawn-is-deferred",
					GAME_INSTANCE->Get_GameObjects(editorLevel).size() == objectCountBeforeSpawn);
				EDITOR->Update(false);
				for (const auto& [runtimeObjectId, object] :
					GAME_INSTANCE->Get_GameObjects(editorLevel)) {
					if (!runtimeIdsBeforeSpawn.contains(runtimeObjectId) && object &&
						object->Get_RuntimeTypeId() == fixture->Get_RuntimeTypeId()) {
						spawned = object;
						break;
					}
				}
				const Vector3 actualPosition = spawned && spawned->Get_Transform()
					? spawned->Get_Transform()->Get_LocalPosition()
					: Vector3{};
				report.Check("editor-spawn-flushes-with-placement", spawned &&
					actualPosition.x == spawnPosition.x &&
					actualPosition.y == spawnPosition.y &&
					actualPosition.z == spawnPosition.z);
				if (spawned)
					GAME_INSTANCE->Destroy(spawned->Get_ObjectGuid());
				GAME_INSTANCE->Flush_DestroyedGameObjects();
			}

			if (fixture && fixture->Get_Transform()) {
				EDITOR->Clear_History();
				const Shared<TextUI> transformChild =
					GAME_INSTANCE->Instantiate<TextUI>(FrameFixtureTag, editorLevel);
				const Bool transformChildReady = transformChild && transformChild->Get_Transform() &&
					SUCCEEDED(fixture->Add_Child(transformChild));
				if (transformChildReady) {
					transformChild->Get_Transform()->Set_LocalPositionByValue({ 1.f, 1.f, 1.f });
					transformChild->Get_Transform()->Update_WorldMatrix();
				}
				report.Check("transform-descendant-fixture-created", transformChildReady);
				ReflectionValue initial;
				ReflectionValue first;
				ReflectionValue second;
				initial.data = Vector3{ 0.f, 0.f, 0.f };
				first.data = Vector3{ 2.f, 4.f, 6.f };
				second.data = Vector3{ 3.f, 6.f, 9.f };

				auto PositionEquals = [&](const ReflectionValue& expected) {
					ReflectionValue current;
					if (FAILED(GAME_INSTANCE->Read_ReflectedProperty(
						*fixture->Get_Transform(), "Position", current)))
						return false;
					const Vector3* lhs = current.Try_Get<Vector3>();
					const Vector3* rhs = expected.Try_Get<Vector3>();
					return lhs && rhs && lhs->x == rhs->x && lhs->y == rhs->y && lhs->z == rhs->z;
				};

				EDITOR->Queue_PropertyWrite(fixture, *fixture->Get_Transform(),
					"Position", initial, first, true);
				report.Check("property-write-is-deferred", PositionEquals(initial));
				EDITOR->Update(false);
				report.Check("property-write-flushed", PositionEquals(first));
				const Vector3 childWorldPosition = transformChildReady
					? transformChild->Get_Transform()->Get_Position()
					: Vector3{};
				report.Check("property-write-refreshes-descendant-transform",
					transformChildReady && childWorldPosition.x == 3.f &&
					childWorldPosition.y == 5.f && childWorldPosition.z == 7.f);

				EDITOR->Queue_PropertyWrite(fixture, *fixture->Get_Transform(),
					"Position", first, second, false);
				EDITOR->Update(false);
				EDITOR->Queue_Undo();
				EDITOR->Update(false);
				report.Check("continuous-property-edit-coalesced", PositionEquals(initial));
				EDITOR->Queue_Redo();
				EDITOR->Update(false);
				report.Check("property-redo-restores-value", PositionEquals(second));

				ReflectionValue staleDesired;
				ReflectionValue externalValue;
				staleDesired.data = Vector3{ 4.f, 8.f, 12.f };
				externalValue.data = Vector3{ 5.f, 10.f, 15.f };
				EDITOR->Queue_PropertyWrite(fixture, *fixture->Get_Transform(),
					"Position", second, staleDesired, true);
				GAME_INSTANCE->Write_ReflectedProperty(
					*fixture->Get_Transform(), "Position", externalValue);
				EDITOR->Update(false);
				report.Check("stale-property-write-rejected", PositionEquals(externalValue));
			}

			const ObjectGuid fixtureGuid = fixture ? fixture->Get_ObjectGuid() : ObjectGuid{};
			const ObjectGuid typedFixtureGuid = typedFixture ? typedFixture->Get_ObjectGuid() : ObjectGuid{};
			const RuntimeObjectId fixtureRuntimeId = fixture ? fixture->Get_RuntimeObjectId() : 0;
			const RuntimeObjectId typedFixtureRuntimeId = typedFixture ? typedFixture->Get_RuntimeObjectId() : 0;
			if (fixture)
				GAME_INSTANCE->Destroy(fixture->Get_ObjectGuid());
			if (typedFixture)
				GAME_INSTANCE->Destroy(typedFixture->Get_ObjectGuid());
			GAME_INSTANCE->Flush_DestroyedGameObjects();
			report.Check("object-manager-destroy-cleans-indices",
				(!fixtureGuid.Is_Valid() || (!GAME_INSTANCE->Find(fixtureGuid) &&
					!GAME_INSTANCE->Find(fixtureRuntimeId))) &&
				(!typedFixtureGuid.Is_Valid() || (!GAME_INSTANCE->Find(typedFixtureGuid) &&
					!GAME_INSTANCE->Find(typedFixtureRuntimeId))));
			report.Check("destroyed-owner-releases-components", !retainedTransform ||
				(retainedTransform->Is_Destroy() && !retainedTransform->Get_Owner()));

			const Bool reportSaved = report.Save(
				Work_Directory() / L"phase7-cutover-gates.json");
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
		if (wstring_view(value) == L"frame")
			return FrameStage;
		if (wstring_view(value) == L"ui-authoring")
			return UIAuthoringStage;
		if (wstring_view(value) == L"references")
			return ReferencesStage;
		if (wstring_view(value) == L"prefab-repository")
			return PrefabRepositoryStage;
		if (wstring_view(value) == L"scene-authoring")
			return SceneAuthoringStage;
		if (wstring_view(value) == L"scene-restart")
			return SceneRestartStage;
		if (wstring_view(value) == L"phase7-cutover" ||
			wstring_view(value) == L"editor-cutover")
			return EditorCutoverStage;
#endif
		return 0;
	}

	HRESULT Initialize_Runtime(uint32 stage)
	{
		if (stage != PrepareStage && stage != VerifyStage &&
			stage != FrameStage && stage != UIAuthoringStage &&
			stage != ReferencesStage && stage != PrefabRepositoryStage &&
			stage != SceneAuthoringStage && stage != SceneRestartStage &&
			stage != EditorCutoverStage)
			return E_INVALIDARG;
		if (FAILED(Client::Register_Client_Reflection()) ||
			FAILED(GAME_INSTANCE->Refresh_ReflectionRegistry()) ||
			FAILED(ClientSettingManager::GetInstance()->Apply_LayerAndTagSettings()) ||
			FAILED(GAME_INSTANCE->Register_ReflectedPrototypes(ETOI(LEVEL::STATIC))))
			return E_FAIL;
		if (stage == PrepareStage || stage == PrefabRepositoryStage)
			return S_OK;
		if (stage == FrameStage || stage == EditorCutoverStage)
		{
			ClientSettingManager::g_EngineDesc = EDITOR->Get_EngineDesc();
			const Shared<TextUI> fixturePrototype = TextUI::CreatePrototype();
			return fixturePrototype && SUCCEEDED(GAME_INSTANCE->Add_Prototype(
				StaticLevel, fixturePrototype, FrameFixtureTag)) ? S_OK : E_FAIL;
		}
		if (stage == UIAuthoringStage || stage == ReferencesStage)
		{
			if (stage == ReferencesStage)
				ClientSettingManager::g_EngineDesc = EDITOR->Get_EngineDesc();
			const Shared<TextUI> fixturePrototype = TextUI::CreatePrototype();
			return fixturePrototype && SUCCEEDED(GAME_INSTANCE->Add_Prototype(
				StaticLevel, fixturePrototype, UIFixtureTag)) ? S_OK : E_FAIL;
		}
		if (stage == SceneAuthoringStage || stage == SceneRestartStage)
		{
			ClientSettingManager::g_EngineDesc = EDITOR->Get_EngineDesc();
			const Shared<TextUI> fixturePrototype = TextUI::CreatePrototype();
			if (!fixturePrototype || FAILED(GAME_INSTANCE->Add_Prototype(
				StaticLevel, fixturePrototype, UIFixtureTag)))
				return E_FAIL;
		}
		if (FAILED(ClientSettingManager::GetInstance()->Load_Shader()) ||
			FAILED(Load_GateModels()) ||
			((stage == SceneAuthoringStage || stage == SceneRestartStage) &&
				FAILED(Load_SceneGateTexture())) ||
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
		if (stage == FrameStage)
			return Run_Frame(initializationResult);
		if (stage == UIAuthoringStage)
			return Run_UIAuthoring(initializationResult);
		if (stage == ReferencesStage)
			return Run_References(initializationResult);
		if (stage == PrefabRepositoryStage)
			return Run_PrefabRepository(initializationResult);
		if (stage == SceneAuthoringStage)
			return Run_SceneAuthoring(initializationResult);
		if (stage == SceneRestartStage)
			return Run_SceneRestart(initializationResult);
		if (stage == EditorCutoverStage)
			return Run_EditorCutover(initializationResult);
		return E_INVALIDARG;
	}
}

NS_END
