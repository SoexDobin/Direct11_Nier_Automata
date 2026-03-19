#include "LevelSerializer.h"
#include <rttr/registration.h>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>


#include "Game.h"
#include "GameObject.h"
#include "Transform.h"
#include "String_Helper.h"
#include "SpdLogger.h"

using namespace std;
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
	root["version"] = "2.0"; // RTTR 기반 버전
	auto& levelsArray = root["levels"];

	const auto& allObjects = GAME_INSTANCE->Get_GameObjects();
	map<uint32, vector<Shared<GameObject>>> levelMap;

	for (auto& [instanceID, pObj] : allObjects)
	{
		if (!pObj || pObj->Is_Destroy()) continue;
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
			rttr::type objType = rttr::type::get(*pObj);

			// 1. 기본 식별 정보
			objJson["typeName"] = objType.get_name().to_string();
			objJson["instanceID"] = pObj->Get_InstanceID();
			objJson["objectID"] = pObj->Get_ObjectID();
			objJson["name"] = ToUtf8(pObj->Get_Name());
			
			// RTTR Type Name == PrototypeTag
			wstring protoTag = Helper::To_wString(objType.get_name().to_string());
			if (protoTag.empty()) protoTag = GAME_INSTANCE->Get_PrototypeTagFromObjectID(pObj->Get_ObjectID(), 0);
			objJson["prototypeTag"] = ToUtf8(protoTag);

			objJson["parentObjectID"] = pObj->Has_Parent() ? pObj->Get_Parent()->Get_ObjectID() : 0u;

			// 트랜스폼 예외 직렬화 (RTTR 누락 방지 안정장치)
			auto transform = pObj->Get_Transform();
			if (transform)
			{
				nlohmann::json transJson;
				Vector3 pos = transform->Get_LocalPosition();
				Vector3 rot = transform->Get_LocalEulerAngles();
				Vector3 scale = transform->Get_LocalScale();
				transJson["position"] = { pos.x, pos.y, pos.z };
				transJson["rotation"] = { rot.x, rot.y, rot.z };
				transJson["scale"] = { scale.x, scale.y, scale.z };
				objJson["transform"] = transJson;
			}

			// 2. GameObject의 RTTR 프로퍼티 직렬화
			nlohmann::json& objProps = objJson["properties"];
			for (auto& prop : objType.get_properties())
			{
				if (prop.get_metadata("NoSerialize") == true) continue;
				
				rttr::variant var = prop.get_value(*pObj);
				string propName = prop.get_name().to_string();

				// GameObject 참조는 고정 해시 ID(ObjectID)로 저장
				if (prop.get_metadata("SaveData") == "GameObject")
				{
					if (var.is_type<Shared<GameObject>>())
					{
						Shared<GameObject> pTarget = var.get_value<Shared<GameObject>>();
						objProps[propName] = pTarget ? pTarget->Get_ObjectID() : 0u;
					}
					else if (var.is_type<uint32>() || var.is_type<int>())
					{
						// 이미 ID를 들고 있는 경우 (Camera 타겟 등)
						// 해당 런타임 ID(InstanceID)에 대응하는 실제 객체를 찾아 고정 ObjectID를 추출
						uint32 currentRuntimeID = var.convert<uint32>();
						Shared<GameObject> pTarget = GAME_INSTANCE->Find_ByInstanceID(currentRuntimeID);
						objProps[propName] = pTarget ? pTarget->Get_ObjectID() : 0u;
					}
				}
				else
				{
					// 기본 타입 지원 (필요 시 확장)
					if (var.is_type<int>()) objProps[propName] = var.get_value<int>();
					else if (var.is_type<uint32>()) objProps[propName] = var.get_value<uint32>();
					else if (var.is_type<float>()) objProps[propName] = var.get_value<float>();
					else if (var.is_type<bool>()) objProps[propName] = var.get_value<bool>();
					else if (var.is_type<string>()) objProps[propName] = var.get_value<string>();
					else if (var.is_type<wstring>()) objProps[propName] = ToUtf8(var.get_value<wstring>());
					else if (var.is_type<Vector3>()) {
						Vector3 v = var.get_value<Vector3>();
						objProps[propName] = { v.x, v.y, v.z };
					}
					else if (var.is_type<Color>()) {
						Color c = var.get_value<Color>();
						objProps[propName] = { c.R(), c.G(), c.B(), c.A() };
					}
					else if (var.is_type<Float3>()) {
						Float3 v = var.get_value<Float3>();
						objProps[propName] = { v.x, v.y, v.z };
					}
					else if (var.is_type<Float4>()) {
						Float4 v = var.get_value<Float4>();
						objProps[propName] = { v.x, v.y, v.z, v.w };
					}
				}
			}

			// 3. 컴포넌트 직렬화
			auto& compArray = objJson["components"];
			for (auto& comp : pObj->Get_Components())
			{
				if (!comp) continue;
				nlohmann::json compJson;
				rttr::type compType = rttr::type::get(*comp);
				compJson["typeName"] = compType.get_name().to_string();

				nlohmann::json& compProps = compJson["properties"];
				for (auto& prop : compType.get_properties())
				{
					if (prop.get_metadata("NoSerialize") == true) continue;

					rttr::variant var = prop.get_value(*comp);
					string propName = prop.get_name().to_string();

					if (prop.get_metadata("SaveData") == "GameObject")
					{
						if (var.is_type<Shared<GameObject>>())
						{
							Shared<GameObject> pTarget = var.get_value<Shared<GameObject>>();
							compProps[propName] = pTarget ? pTarget->Get_ObjectID() : 0u;
						}
						else
						{
							uint32 currentRuntimeID = var.convert<uint32>();
							Shared<GameObject> pTarget = GAME_INSTANCE->Find_ByInstanceID(currentRuntimeID);
							compProps[propName] = pTarget ? pTarget->Get_ObjectID() : 0u;
						}
					}
					else
					{
						if (var.is_type<int>()) compProps[propName] = var.get_value<int>();
						else if (var.is_type<uint32>()) compProps[propName] = var.get_value<uint32>();
						else if (var.is_type<float>()) compProps[propName] = var.get_value<float>();
						else if (var.is_type<bool>()) compProps[propName] = var.get_value<bool>();
						else if (var.is_type<string>()) compProps[propName] = var.get_value<string>();
						else if (var.is_type<wstring>()) compProps[propName] = ToUtf8(var.get_value<wstring>());
						else if (var.is_type<Vector3>()) {
							Vector3 v = var.get_value<Vector3>();
							compProps[propName] = { v.x, v.y, v.z };
						}
						else if (var.is_type<Color>()) {
							Color c = var.get_value<Color>();
							compProps[propName] = { c.R(), c.G(), c.B(), c.A() };
						}
						else if (var.is_type<Float3>()) {
							Float3 v = var.get_value<Float3>();
							compProps[propName] = { v.x, v.y, v.z };
						}
						else if (var.is_type<Float4>()) {
							Float4 v = var.get_value<Float4>();
							compProps[propName] = { v.x, v.y, v.z, v.w };
						}
					}
				}
				compArray.push_back(compJson);
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
		LOG_ERROR(L"[SceneSerializer] DeSerializeLevel exception: {}", FromUtf8(e.what()));
		return E_FAIL;
	}

	string version = root.value("version", "1.1");
	if (FAILED(GAME_INSTANCE->Clear_AllGameObjects())) return E_FAIL;

	unordered_map<uint32, Shared<GameObject>> instanceMap;
	struct ObjectData {
		Shared<GameObject> pObj;
		const nlohmann::json* pJson;
	};
	vector<ObjectData> loadList;

	// ── Pass 1: 객체 생성 (Instantiate) ──────────────────────────────────
	auto ProcessLevel = [&](const nlohmann::json& levelJson) {
		uint32 levIndex = levelJson.value("levelIndex", 0u);
		for (auto& objJson : levelJson["objects"])
		{
			wstring protoTag = FromUtf8(objJson.value("prototypeTag", ""));
			if (protoTag.empty()) continue;

			Shared<GameObject> pObj = GAME_INSTANCE->Instantiate<GameObject>(protoTag, levIndex);
			if (!pObj) pObj = GAME_INSTANCE->Instantiate<GameObject>(protoTag, 0); // STATIC에서 재시도

			if (pObj)
			{
				uint32 savedObjectID = objJson.value("objectID", 0u);
				if (savedObjectID != 0) 
				{
					pObj->Set_ObjectID(savedObjectID);
				}
				
				wstring savedName = FromUtf8(objJson.value("name", ""));
				if (!savedName.empty())
				{
					pObj->Set_Name(savedName);
				}

				uint32 fixedObjectID = pObj->Get_ObjectID();
				instanceMap[fixedObjectID] = pObj;
				loadList.push_back({ pObj, &objJson });
			}
		}
	};

	if (root.contains("levels"))
	{
		for (auto& levelJson : root["levels"]) ProcessLevel(levelJson);
	}
	else if (root.contains("objects"))
	{
		ProcessLevel(root);
	}

	// ── Pass 2: 부모 설정 (ObjectID 기반) ───────────────────────
	for (auto& data : loadList)
	{
		uint32 savedParentObjectID = data.pJson->value("parentObjectID", 0u);
		if (savedParentObjectID != 0)
		{
			auto it = instanceMap.find(savedParentObjectID);
			if (it != instanceMap.end()) 
                data.pObj->Set_Parent(it->second);
		}
	}

	// ── Pass 2: 데이터 주입 (Properties & Components) ─────────────────────
	for (auto& data : loadList)
	{
		auto& pObj = data.pObj;
		auto& objJson = *data.pJson;
		rttr::type objType = rttr::type::get(*pObj);

		// 2. RTTR 프로퍼티 주입 (GameObject)
		if (objJson.contains("properties"))
		{
			auto& propsJson = objJson["properties"];
			for (auto it = propsJson.begin(); it != propsJson.end(); ++it)
			{
				rttr::property prop = objType.get_property(it.key());
				if (!prop.is_valid()) continue;

				if (it.value().is_number())
				{
					if (prop.get_type() == rttr::type::get<uint32>())
						prop.set_value(*pObj, static_cast<uint32>(it.value().get<double>()));
					else if (prop.get_type() == rttr::type::get<int32>())
						prop.set_value(*pObj, static_cast<int32>(it.value().get<double>()));
					else if (prop.get_type() == rttr::type::get<Float>())
						prop.set_value(*pObj, static_cast<Float>(it.value().get<double>()));
					else
						prop.set_value(*pObj, it.value().get<double>());
				}
				else if (it.value().is_boolean()) 
				{
					prop.set_value(*pObj, it.value().get<Bool>());
				}
				else if (it.value().is_string()) 
				{
					prop.set_value(*pObj, FromUtf8(it.value().get<string>()));
				}
				else if (it.value().is_array()) 
				{
					auto& v = it.value();
					if (v.size() == 3 && prop.get_type() == rttr::type::get<Vector3>())
						prop.set_value(*pObj, Vector3(v[0], v[1], v[2]));
					else if (v.size() == 3 && prop.get_type() == rttr::type::get<Float3>())
						prop.set_value(*pObj, Float3(v[0], v[1], v[2]));
					else if (v.size() == 4 && prop.get_type() == rttr::type::get<Color>())
						prop.set_value(*pObj, Color(v[0], v[1], v[2], v[3]));
					else if (v.size() == 4 && prop.get_type() == rttr::type::get<Float4>())
						prop.set_value(*pObj, Float4(v[0], v[1], v[2], v[3]));
				}
			}
		}

		// 3. 컴포넌트 데이터 주입
		if (objJson.contains("components"))
		{
			for (auto& compJson : objJson["components"])
			{
				string typeName = compJson.value("typeName", "");
				auto& compPropsJson = compJson["properties"];
				
				// 해당 타입을 가진 컴포넌트를 객체에서 찾음
				Shared<Component> targetComp = nullptr;
				for (auto& pComp : pObj->Get_Components())
				{
					if (!pComp) continue;
					rttr::type compType = rttr::type::get(*pComp);
					if (compType.get_name().to_string() == typeName) {
						targetComp = pComp;
						break;
					}
				}

				// 없으면 동적 생성 후 부착 (엔진 로직 활용)
				if (!targetComp)
				{
					targetComp = pObj->Add_Component(FromUtf8(typeName));
				}

				if (targetComp)
				{
					rttr::type compType = rttr::type::get(*targetComp);
					for (auto it = compPropsJson.begin(); it != compPropsJson.end(); ++it)
					{
						rttr::property prop = compType.get_property(it.key());
						if (!prop.is_valid()) continue;

						if (it.value().is_number())
						{
							if (prop.get_type() == rttr::type::get<uint32>())
								prop.set_value(*targetComp, static_cast<uint32>(it.value().get<double>()));
							else if (prop.get_type() == rttr::type::get<int>())
								prop.set_value(*targetComp, static_cast<int>(it.value().get<double>()));
							else if (prop.get_type() == rttr::type::get<Float>())
								prop.set_value(*targetComp, static_cast<Float>(it.value().get<double>()));
							else
								prop.set_value(*targetComp, it.value().get<double>());
						}
						else if (it.value().is_boolean()) 
						{
							prop.set_value(*targetComp, it.value().get<bool>());
						}
						else if (it.value().is_string()) 
						{
							prop.set_value(*targetComp, FromUtf8(it.value().get<string>()));
						}
						else if (it.value().is_array()) 
						{
							auto& v = it.value();
							if (v.size() == 3 && prop.get_type() == rttr::type::get<Vector3>())
								prop.set_value(*targetComp, Vector3(v[0], v[1], v[2]));
							else if (v.size() == 3 && prop.get_type() == rttr::type::get<Float3>())
								prop.set_value(*targetComp, Float3(v[0], v[1], v[2]));
							else if (v.size() == 4 && prop.get_type() == rttr::type::get<Color>())
								prop.set_value(*targetComp, Color(v[0], v[1], v[2], v[3]));
							else if (v.size() == 4 && prop.get_type() == rttr::type::get<Float4>())
								prop.set_value(*targetComp, Float4(v[0], v[1], v[2], v[3]));
						}
					}
				}
			}
		}

		// 4. Transform 명시적 안전장치: 구버전/신버전 모두 지원
		if (objJson.contains("transform"))
		{
			auto& t = objJson["transform"];
			auto transform = pObj->Get_Transform();
			if (transform)
			{
				if (t.contains("position")) transform->Set_LocalPositionByValue({ t["position"][0], t["position"][1], t["position"][2] });
				if (t.contains("rotation")) transform->Set_LocalEulerAngleByValue({ t["rotation"][0], t["rotation"][1], t["rotation"][2] });
				if (t.contains("scale"))    transform->Set_LocalScaleByValue({ t["scale"][0], t["scale"][1], t["scale"][2] });
			}
		}
	}

	// ── Pass 3: 참조 해결 (Linking) ──────────────────────────────────────
	for (auto& pair : instanceMap)
	{
		if (pair.second) 
		{
			pair.second->Post_Load(instanceMap);
			if (pair.second->Get_Transform()) {
				pair.second->Get_Transform()->Update_WorldMatrix();
			}
		}
	}

	LOG_INFO(L"[SceneSerializer] RTTR Scene loaded: {} objects", instanceMap.size());
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