#include "LevelSerializer.h"
#include <rttr/registration.h>
#include <nlohmann/json.hpp>
#include "Engine_RTTR_Metadata.h"
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <iostream>
#include "Game.h"
#include "GameObject.h"
#include "Transform.h"
#include "String_Helper.h"
#include "SpdLogger.h"

using namespace std;
using namespace Engine;
using namespace Meta_Key_Type;
using namespace Save_Data_Key;

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

HRESULT LevelSerializer::SerializeLevel(uint32 levIndex, const wstring& filePath)
{
	nlohmann::json root;
	root["version"] = "2.0"; 
	auto& levelsArray = root["levels"];

	const auto& allObjects = GAME_INSTANCE->Get_GameObjects(levIndex);
	map<uint32, vector<Shared<GameObject>>> levelMap;

	for (auto& [instanceID, pObj] : allObjects)
	{
		if (!pObj || pObj->Is_Destroy()) continue;
		levelMap[levIndex].push_back(pObj);
	}


	// 공통 속성 직렬화 헬퍼 람다
	auto SerializeProperties = [&](rttr::instance objInstance, nlohmann::json& outProps) {
		rttr::type objType = objInstance.get_type();
		for (auto& prop : objType.get_properties())
		{
			rttr::variant var = prop.get_value(objInstance);
			string propName = prop.get_name().to_string();
			
			rttr::variant saveMeta = prop.get_metadata(Meta_Key_Type::SaveData);
			if (!saveMeta.is_valid()) continue; // SaveData 메타데이터가 없으면 아예 저장 안 함

			SaveDataKey saveKey = nullptr;
			if (saveMeta.is_type<SaveDataKey>()) {
				saveKey = saveMeta.get_value<SaveDataKey>();
			}
			
			if (saveKey == Save_Data_Key::TargetObjectID) // 1. TargetObjectID (다른 오브젝트 참조)
			{
				if (var.is_type<Shared<GameObject>>())
				{
					Shared<GameObject> pTarget = var.get_value<Shared<GameObject>>();
					outProps[propName] = pTarget ? pTarget->Get_ObjectID() : 0u;
				}
				else if (var.is_type<uint32>() || var.is_type<int32>()) {
					outProps[propName] = var.convert<uint32>();
				}
			}
			else if (saveKey == Save_Data_Key::TextureTag || saveKey == Save_Data_Key::ModelTag) // 2. ResourceTag
			{
				if (var.is_type<wstring>()) outProps[propName] = ToUtf8(var.get_value<wstring>());
				else if (var.is_type<string>()) outProps[propName] = var.get_value<string>();
			}
			else // 3. 그 외 기본 자료형 (SaveData가 붙어있는 float, Vector3, int 등)
			{
				if (var.is_type<int>()) outProps[propName] = var.get_value<int>();
				else if (var.is_type<uint32>()) outProps[propName] = var.get_value<uint32>();
				else if (var.is_type<float>()) outProps[propName] = var.get_value<float>();
				else if (var.is_type<bool>()) outProps[propName] = var.get_value<bool>();
				else if (var.is_type<string>()) outProps[propName] = var.get_value<string>();
				else if (var.is_type<wstring>()) outProps[propName] = ToUtf8(var.get_value<wstring>());
				else if (var.is_type<Vector3>()) { Vector3 v = var.get_value<Vector3>(); outProps[propName] = { v.x, v.y, v.z }; }
				else if (var.is_type<Color>()) { Color c = var.get_value<Color>(); outProps[propName] = { c.R(), c.G(), c.B(), c.A() }; }
				else if (var.is_type<Float3>()) { Float3 v = var.get_value<Float3>(); outProps[propName] = { v.x, v.y, v.z }; }
				else if (var.is_type<Float4>()) { Float4 v = var.get_value<Float4>(); outProps[propName] = { v.x, v.y, v.z, v.w }; }
			}
		}
	};


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
			nlohmann::json& objProp = objJson["properties"];
			SerializeProperties(*pObj, objProp);

			// 3. 컴포넌트 직렬화
			auto& compArray = objJson["components"];
			for (auto& comp : pObj->Get_Components())
			{
				if (!comp) continue;
				nlohmann::json compJson;
				rttr::type compType = rttr::type::get(*comp);
				compJson["typeName"] = compType.get_name().to_string();

				nlohmann::json& comProps = compJson["properties"];
				SerializeProperties(*comp, comProps);
				

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
		LOG_CRITICAL(L"[SceneSerializer] SerializeLevel exception: {}", Helper::To_wString(e.what()));
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
		LOG_CRITICAL(L"[SceneSerializer] DeSerializeLevel exception: {}", FromUtf8(e.what()));
		return E_FAIL;
	}

	string version = root.value("version", "1.1");

	// ── Pass 0: 선택적 클리어 (JSON에 명시된 레벨만 초기화) ───────────────────────
	if (root.contains("levels"))
	{
		for (auto& levelJson : root["levels"])
		{
			uint32 levIndex = levelJson.value("levelIndex", 0u);
			GAME_INSTANCE->Clearing_ObjectManager(levIndex);
		}
	}
	else if (root.contains("objects"))
	{
		// 구버전 혹은 단일 레벨 파일의 경우 0번 레벨만 클리어하거나 전체 클리어 선택
		// 여기서는 안전하게 전체 클리어를 유지하되, 필요 시 levIndex 0만 클리어하도록 수정 가능
		GAME_INSTANCE->Clear_AllGameObjects();
	}

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
		for (auto& levelJson : root["levels"])
			ProcessLevel(levelJson);
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

	// 공통 속성 역직렬화 헬퍼 람다
	auto DeserializeProperties = [&](rttr::instance objInstance, const nlohmann::json& propsJson) {
		rttr::type objType = objInstance.get_type();
		for (auto it = propsJson.begin(); it != propsJson.end(); ++it)
		{
			rttr::property prop = objType.get_property(it.key());
			if (!prop.is_valid()) continue;
			
			// 실제 오브젝트에 값 꽂아넣기
			if (it.value().is_number()) {
				if (prop.get_type() == rttr::type::get<uint32>()) prop.set_value(objInstance, static_cast<uint32>(it.value().get<double>()));
				else if (prop.get_type() == rttr::type::get<int32>()) prop.set_value(objInstance, static_cast<int32>(it.value().get<double>()));
				else if (prop.get_type() == rttr::type::get<Float>()) prop.set_value(objInstance, static_cast<Float>(it.value().get<double>()));
				else prop.set_value(objInstance, it.value().get<double>());
			}
			else if (it.value().is_boolean()) { prop.set_value(objInstance, it.value().get<Bool>()); }
			else if (it.value().is_string()) { prop.set_value(objInstance, FromUtf8(it.value().get<string>())); }
			else if (it.value().is_array()) {
				auto& v = it.value();
				if (v.size() == 3 && prop.get_type() == rttr::type::get<Vector3>()) prop.set_value(objInstance, Vector3(v[0], v[1], v[2]));
				else if (v.size() == 3 && prop.get_type() == rttr::type::get<Float3>()) prop.set_value(objInstance, Float3(v[0], v[1], v[2]));
				else if (v.size() == 4 && prop.get_type() == rttr::type::get<Color>()) prop.set_value(objInstance, Color(v[0], v[1], v[2], v[3]));
				else if (v.size() == 4 && prop.get_type() == rttr::type::get<Float4>()) prop.set_value(objInstance, Float4(v[0], v[1], v[2], v[3]));
			}

			// [로깅용] ResourceTag가 세팅되었는지 검사하여 출력
			rttr::variant saveMeta = prop.get_metadata(Meta_Key_Type::SaveData);
			if (saveMeta.is_valid() && saveMeta.is_type<SaveDataKey>())
			{
				SaveDataKey saveKey = saveMeta.get_value<SaveDataKey>();
				if (saveKey == Save_Data_Key::TextureTag || saveKey == Save_Data_Key::ModelTag)
				{
					LOG_INFO(L"[Serializer] ResourceTag injected into {} : {}",
						Helper::To_wString(objType.get_name().to_string()),
						FromUtf8(it.value().get<string>()));
				}
			}
		}
	};



	// ── Pass 2: 데이터 주입 (Properties & Components) ─────────────────────
	for (auto& data : loadList)
	{
		auto& pObj = data.pObj;
		auto& objJson = *data.pJson;
		rttr::type objType = rttr::type::get(*pObj);

		// 2. RTTR 프로퍼티 주입 (GameObject)
		if (objJson.contains("properties"))
		{
			DeserializeProperties(*pObj, objJson["properties"]);
		}

		// 3. 컴포넌트 데이터 주입
		if (objJson.contains("components"))
		{
			for (auto& compJson : objJson["components"])
			{
				string typeName = compJson.value("typeName", "");
				auto& compPropsJson = compJson["properties"];

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
				if (!targetComp)
				{
                    targetComp = pObj->Add_Component(FromUtf8(typeName));
				}
				if (targetComp)
				{
					DeserializeProperties(*targetComp, compPropsJson);
				}
			}
		}
		
		// 4. (무조건 등록되었던) Transform 주입
		if (objJson.contains("transform"))
		{
			auto& t = objJson["transform"];
			auto transform = pObj->Get_Transform();
			if (transform)
			{
				if (t.contains("position")) 
					transform->Set_LocalPositionByValue({ t["position"][0], t["position"][1], t["position"][2] });
				if (t.contains("rotation")) 
					transform->Set_LocalEulerAngleByValue({ t["rotation"][0], t["rotation"][1], t["rotation"][2] });
				if (t.contains("scale"))    
					transform->Set_LocalScaleByValue({ t["scale"][0], t["scale"][1], t["scale"][2] });
			}
		}
	}

	// ── Pass 4: 참조 해결 (Linking) ──────────────────────────────────────
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