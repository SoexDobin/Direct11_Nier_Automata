#include "LevelSerializer.h"

#include "Game.h"
#include "GameObject.h"
#include "Transform.h"
#include "Model.h"
#include "Texture.h"
#include "String_Helper.h"
#include "SpdLogger.h"

#include <filesystem>
#include <fstream>

using namespace Engine;

string LevelSerializer::ToUtf8(const wstring& ws)
{
	if (ws.empty())
		return {};
	int len = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, nullptr, 0, nullptr, nullptr);
	string result(len, '\0');
	WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, result.data(), len, nullptr, nullptr);
	if (!result.empty() && result.back() == '\0')
		result.pop_back();
	return result;
}

wstring LevelSerializer::FromUtf8(const string& s)
{
	if (s.empty())
		return {};
	int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
	wstring result(len, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, result.data(), len);
	if (!result.empty() && result.back() == L'\0')
		result.pop_back();
	return result;
}

LevelSerializer::LevelSerializer() : EngineManager{}
{
}

HRESULT LevelSerializer::SerializeLevel(const wstring& filePath)
{
	nlohmann::json root;
	root["version"] = "1.1";
	auto& levelsArray = root["levels"];

	// 현재 로드된 모든 객체를 레벨별로 분류
	const auto& allObjects = GAME_INSTANCE->Get_GameObjects();
	map<uint32, vector<Shared<GameObject>>> levelMap;

	for (auto& [instanceID, pObj] : allObjects)
	{
		if (!pObj || pObj->Is_Destroy())
			continue;

		// [중요] 현재 GameObject에 m_levIndex가 없으므로 임시로 0레벨 혹은 
		// prototypeTag 검색 시 사용한 레벨 기반으로 분류하거나, 
		// 전체를 현재 레벨 인덱스로 묶어서 저장하되 구조만 다중 레벨을 지원하도록 함.
		// (추후 GameObject에 levIndex 멤버가 추가되면 정확한 분류 가능)
		uint32 objLevel = GAME_INSTANCE->Get_CurrentLevelIndex(); 
		levelMap[objLevel].push_back(pObj);
	}

	for (auto& [levIndex, objects] : levelMap)
	{
		nlohmann::json levelJson;
		levelJson["levelIndex"] = levIndex;
		auto& objArray = levelJson["objects"];

		for (auto& pObj : objects)
		{
			nlohmann::json objJson;

			// ── 기본 식별 정보 ──────────────────────────────────────────────
			objJson["instanceID"] = pObj->Get_InstanceID();
			objJson["objectID"]   = pObj->Get_ObjectID();
			objJson["name"]       = ToUtf8(pObj->Get_Name());

			// prototypeTag (모든 레벨 검색)
			wstring tag = GAME_INSTANCE->Get_PrototypeTagFromObjectID(pObj->Get_ObjectID(), levIndex);
			if (tag.empty())
				tag = GAME_INSTANCE->Get_PrototypeTagFromObjectID(pObj->Get_ObjectID(), 0);
			
			if (tag.empty())
			{
				for (uint32 i = 0; i < 10; ++i)
				{
					tag = GAME_INSTANCE->Get_PrototypeTagFromObjectID(pObj->Get_ObjectID(), i);
					if (!tag.empty()) break;
				}
			}
			objJson["prototypeTag"] = ToUtf8(tag);

			objJson["parentInstanceID"] = pObj->Has_Parent()
				? static_cast<uint32>(pObj->Get_Parent()->Get_InstanceID())
				: 0u;

			// ── Transform ───────────────────────────────────────────────────
			auto pTrans = pObj->Get_Transform();
			if (pTrans)
			{
				Vector3 pos = pTrans->Get_LocalPosition();
				Vector3 rot = pTrans->Get_LocalEulerAngles();
				Vector3 scl = pTrans->Get_LocalScale();

				objJson["transform"]["position"] = { pos.x, pos.y, pos.z };
				objJson["transform"]["rotation"] = { rot.x, rot.y, rot.z };
				objJson["transform"]["scale"]    = { scl.x, scl.y, scl.z };
			}

			// ── Component 태그 ──────────────────────────────────────────────
			const vector<Shared<Component>> allComps = pObj->Get_Components();
			for (auto& comp : allComps)
			{
				if (nullptr == comp)
				{
					continue;
				}

				if (COMPONENT_TYPE::MODEL == comp->Get_ComponentType())
				{
					Shared<Model> pModel = std::static_pointer_cast<Model>(comp);
					objJson["modelTag"] = ToUtf8(pModel->Get_ModelTag());
				}
				else if (COMPONENT_TYPE::TEXTURE == comp->Get_ComponentType())
				{
					Shared<Texture> pTexture = std::static_pointer_cast<Texture>(comp);
					objJson["textureTag"] = ToUtf8(pTexture->Get_TextureTag());
				}
			}

			if (false == objJson.contains("modelTag"))
			{
				objJson["modelTag"] = "";
			}
			if (false == objJson.contains("textureTag"))
			{
				objJson["textureTag"] = "";
			}

			objArray.push_back(objJson);
		}
		levelsArray.push_back(levelJson);
	}

	try
	{
		std::filesystem::path path(filePath);
		std::filesystem::create_directories(path.parent_path());

		std::ofstream file(filePath);
		if (!file.is_open()) return E_FAIL;
		file << root.dump(2);
		file.close();
	}
	catch (const std::exception& e)
	{
		LOG_ERROR(L"[SceneSerializer] SerializeLevel exception: {}", Helper::To_wString(e.what()));
		return E_FAIL;
	}

	LOG_INFO(L"[SceneSerializer] Multi-level scene saved: {} levels", levelMap.size());
	return S_OK;
}

HRESULT LevelSerializer::DeSerializeLevel(const wstring& filePath)
{
	nlohmann::json root;
	try
	{
		std::ifstream file(filePath);
		if (!file.is_open()) return E_FAIL;
		file >> root;
		file.close();
	}
	catch (const std::exception& e)
	{
		LOG_ERROR(L"[SceneSerializer] DeSerializeLevel exception: {}", Helper::To_wString(e.what()));
		return E_FAIL;
	}

	if (!root.contains("version") || (root["version"] != "1.0" && root["version"] != "1.1"))
		return E_FAIL;

	if (FAILED(GAME_INSTANCE->Clear_AllGameObjects()))
		return E_FAIL;

	struct ObjectEntry
	{
		uint32  savedInstanceID;
		uint32  parentInstanceID;
		uint32  levelIndex;
		wstring prototypeTag;
		wstring modelTag;
		wstring textureTag;
		Vector3 position;
		Vector3 rotation;
		Vector3 scale;
		Shared<GameObject> spawnedObject;
	};

	vector<ObjectEntry> entries;
	unordered_map<uint32, Shared<GameObject>> instanceMap;

	// ── Pass 1: JSON 파싱 및 구조 선언 ──────────────────────────────────
	auto ParseObject = [&](const nlohmann::json& objJson, uint32 levIndex) {
		ObjectEntry e;
		e.levelIndex = levIndex;
		e.savedInstanceID = objJson.value("instanceID", 0u);
		e.parentInstanceID = objJson.value("parentInstanceID", 0u);

		auto GetSafeString = [&](const char* key) -> wstring {
			if (objJson.contains(key) && objJson[key].is_string())
			{
				return FromUtf8(objJson[key].get<string>());
			}
			return L"";
		};

		e.prototypeTag    = GetSafeString("prototypeTag");
		e.modelTag        = GetSafeString("modelTag");
		e.textureTag      = GetSafeString("textureTag");

		e.scale    = Vector3::One;
		if (objJson.contains("transform"))
		{
			auto& t = objJson["transform"];
			if (t.contains("position") && t["position"].is_array()) e.position = { t["position"][0], t["position"][1], t["position"][2] };
			if (t.contains("rotation") && t["rotation"].is_array()) e.rotation = { t["rotation"][0], t["rotation"][1], t["rotation"][2] };
			if (t.contains("scale")    && t["scale"].is_array())    e.scale    = { t["scale"][0], t["scale"][1], t["scale"][2] };
		}
		return e;
	};

	if (root.contains("levels"))
	{
		for (auto& levelJson : root["levels"])
		{
			uint32 levIndex = levelJson.value("levelIndex", 0u);
			for (auto& objJson : levelJson["objects"])
				entries.push_back(ParseObject(objJson, levIndex));
		}
	}
	else if (root.contains("objects")) // 구버전(1.0) 호환성 유지
	{
		uint32 levIndex = root.value("level", 0u);
		for (auto& objJson : root["objects"])
			entries.push_back(ParseObject(objJson, levIndex));
	}

	// ── Pass 2: 실구매 및 인스턴싱 ──────────────────────────────────────────
	for (auto& entry : entries)
	{
		if (entry.prototypeTag.empty()) continue;

		Shared<GameObject> pObj = GAME_INSTANCE->Instantiate<GameObject>(entry.prototypeTag, UINT_MAX);
		if (!pObj) continue;

		entry.spawnedObject = pObj;
		instanceMap[entry.savedInstanceID] = pObj;
	}

	// ── Pass 3: 계층 구조 복구 ──────────────────────────────────────────────
	for (auto& entry : entries)
	{
		if (!entry.spawnedObject || entry.parentInstanceID == 0) continue;

		auto parentIt = instanceMap.find(entry.parentInstanceID);
		if (parentIt != instanceMap.end() && parentIt->second)
			entry.spawnedObject->Set_Parent(parentIt->second);
	}

	// ── Pass 4: 데이터 세부 복구 ────────────────────────────────────────────
	for (auto& entry : entries)
	{
		if (!entry.spawnedObject) continue;

		auto transform = entry.spawnedObject->Get_Transform();
		if (transform)
		{
			transform->Set_LocalScale(entry.scale);
			transform->Set_LocalRotation(entry.rotation);
			transform->Set_LocalPosition(entry.position);
		}

		auto allComps = entry.spawnedObject->Get_Components();
		for (auto& comp : allComps)
		{
			if (comp->Get_ComponentType() == COMPONENT_TYPE::MODEL && !entry.modelTag.empty())
				std::static_pointer_cast<Model>(comp)->Set_ModelTag(entry.modelTag);
			else if (comp->Get_ComponentType() == COMPONENT_TYPE::TEXTURE && !entry.textureTag.empty())
				std::static_pointer_cast<Texture>(comp)->Set_TextureTag(entry.textureTag);
		}
	}

	LOG_INFO(L"[SceneSerializer] Scene loaded: {} objects", instanceMap.size());
	return S_OK;
}

Unique<LevelSerializer> LevelSerializer::Create()
{
	auto levelSerializer = make_unique<LevelSerializer>();

	if (FAILED(levelSerializer->Initialize_Prototype()) ||
		FAILED(levelSerializer->Initialize(nullptr)))
	{
		LOG_ERROR(L"Failed to Create LevelSerializer");
	}

	return levelSerializer;
}