#include "LevelSerializer.h"
#include <rttr/registration.h>
#include <nlohmann/json.hpp>
#include "Engine_RTTR_Metadata.h"
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <limits>
#include <unordered_set>
#include "Game.h"
#include "GameObject.h"
#include "Component.h"
#include "Transform.h"
#include "String_Helper.h"
#include "SpdLogger.h"

using namespace std;
using namespace Engine;
using namespace Meta_Key_Type;
using namespace Save_Data_Key;

namespace
{
	constexpr uint32 SceneSchemaVersion = 2;

	Bool IsSupportedPropertyType(const rttr::type& propertyType)
	{
		return propertyType == rttr::type::get<int32>() ||
			propertyType == rttr::type::get<uint32>() ||
			propertyType == rttr::type::get<Float>() ||
			propertyType == rttr::type::get<Double>() ||
			propertyType == rttr::type::get<Bool>() ||
			propertyType == rttr::type::get<string>() ||
			propertyType == rttr::type::get<wstring>() ||
			propertyType == rttr::type::get<Vector3>() ||
			propertyType == rttr::type::get<Float3>() ||
			propertyType == rttr::type::get<Color>() ||
			propertyType == rttr::type::get<Float4>();
	}

	Bool IsCompatiblePropertyValue(const rttr::type& propertyType, const nlohmann::json& value)
	{
		if (!IsSupportedPropertyType(propertyType))
			return false;
		if (propertyType == rttr::type::get<uint32>())
			return value.is_number_unsigned() && value.get<std::uint64_t>() <= numeric_limits<uint32>::max();
		if (propertyType == rttr::type::get<int32>())
			return value.is_number_integer() && !value.is_number_unsigned() &&
				value.get<std::int64_t>() >= numeric_limits<int32>::min() &&
				value.get<std::int64_t>() <= numeric_limits<int32>::max();
		if (propertyType == rttr::type::get<Float>() || propertyType == rttr::type::get<Double>())
			return value.is_number();
		if (propertyType == rttr::type::get<Bool>())
			return value.is_boolean();
		if (propertyType == rttr::type::get<string>() || propertyType == rttr::type::get<wstring>())
			return value.is_string();
		if (propertyType == rttr::type::get<Vector3>() || propertyType == rttr::type::get<Float3>())
			return value.is_array() && value.size() == 3 &&
				all_of(value.begin(), value.end(), [](const auto& element) { return element.is_number(); });
		return value.is_array() && value.size() == 4 &&
			all_of(value.begin(), value.end(), [](const auto& element) { return element.is_number(); });
	}

	Bool IsVector3Json(const nlohmann::json& value)
	{
		return value.is_array() && value.size() == 3 &&
			all_of(value.begin(), value.end(), [](const auto& element) { return element.is_number(); });
	}
}

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
	root["schemaVersion"] = SceneSchemaVersion;
	root["levelIndex"] = levIndex;
	root["roots"] = nlohmann::json::array();
	root["objects"] = nlohmann::json::object();

	const auto& allObjects = GAME_INSTANCE->Get_GameObjects(levIndex);
	vector<Shared<GameObject>> objects;
	unordered_set<ObjectGuid, GuidHash> serializedGuids;
	for (const auto& [instanceID, object] : allObjects) {
		if (!object || object->Is_Destroy())
			continue;
		if (!object->Get_ObjectGuid().Is_Valid() ||
			!serializedGuids.emplace(object->Get_ObjectGuid()).second) {
			LOG_ERROR(L"[SceneSerializer] Invalid or duplicate ObjectGuid on {}", object->Get_Name());
			return E_FAIL;
		}
		objects.push_back(object);
	}
	std::ranges::sort(objects, {}, [](const Shared<GameObject>& object) {
		return object->Get_ObjectGuid();
	});

	auto SerializeProperties = [&](rttr::instance objInstance, nlohmann::json& outProps) -> Bool {
		outProps = nlohmann::json::object();
		const rttr::type objectType = objInstance.get_type();
		for (const rttr::property property : objectType.get_properties()) {
			const string propertyName = property.get_name().to_string();
			if (!property.get_metadata(Meta_Key_Type::SaveData).is_valid())
				continue;

			const rttr::variant value = property.get_value(objInstance);
			if (!value.is_valid() || !IsSupportedPropertyType(property.get_type())) {
				LOG_ERROR(L"[Serializer] Unsupported saved property {} on type {}",
					FromUtf8(propertyName), Helper::To_wString(objectType.get_name().to_string()));
				return false;
			}

			if (value.is_type<int32>()) outProps[propertyName] = value.get_value<int32>();
			else if (value.is_type<uint32>()) outProps[propertyName] = value.get_value<uint32>();
			else if (value.is_type<Float>()) outProps[propertyName] = value.get_value<Float>();
			else if (value.is_type<Double>()) outProps[propertyName] = value.get_value<Double>();
			else if (value.is_type<Bool>()) outProps[propertyName] = value.get_value<Bool>();
			else if (value.is_type<string>()) outProps[propertyName] = value.get_value<string>();
			else if (value.is_type<wstring>()) outProps[propertyName] = ToUtf8(value.get_value<wstring>());
			else if (value.is_type<Vector3>()) { const Vector3 v = value.get_value<Vector3>(); outProps[propertyName] = { v.x, v.y, v.z }; }
			else if (value.is_type<Float3>()) { const Float3 v = value.get_value<Float3>(); outProps[propertyName] = { v.x, v.y, v.z }; }
			else if (value.is_type<Color>()) { const Color c = value.get_value<Color>(); outProps[propertyName] = { c.R(), c.G(), c.B(), c.A() }; }
			else if (value.is_type<Float4>()) { const Float4 v = value.get_value<Float4>(); outProps[propertyName] = { v.x, v.y, v.z, v.w }; }
		}
		return true;
	};

	for (const Shared<GameObject>& object : objects)
	{
		nlohmann::json objectJson;
		const rttr::type objectType = rttr::type::get(*object);
		const string typeName = objectType.get_name().to_string();
		if (typeName.empty() || GAME_INSTANCE->Find_RuntimeTypeId(typeName) == 0) {
			LOG_ERROR(L"[SceneSerializer] Unregistered GameObject type {}", FromUtf8(typeName));
			return E_FAIL;
		}

		objectJson["typeName"] = typeName;
		objectJson["name"] = ToUtf8(object->Get_Name());
		objectJson["stableChildKey"] = ToUtf8(object->Get_StableChildKey());
		objectJson["children"] = nlohmann::json::array();
		for (const Shared<GameObject>& child : object->Get_Children()) {
			if (child && child->Get_Parent() == object && serializedGuids.contains(child->Get_ObjectGuid()))
				objectJson["children"].push_back(Engine::To_String(child->Get_ObjectGuid()));
		}

		const Shared<GameObject> parent = object->Get_Parent();
		if (!parent || !serializedGuids.contains(parent->Get_ObjectGuid()))
			root["roots"].push_back(Engine::To_String(object->Get_ObjectGuid()));

		if (const Shared<Transform> transform = object->Get_Transform()) {
			const Vector3 position = transform->Get_LocalPosition();
			const Vector3 rotation = transform->Get_LocalEulerAngles();
			const Vector3 scale = transform->Get_LocalScale();
			objectJson["transform"] = {
				{ "position", { position.x, position.y, position.z } },
				{ "rotation", { rotation.x, rotation.y, rotation.z } },
				{ "scale", { scale.x, scale.y, scale.z } }
			};
		}

		if (!SerializeProperties(*object, objectJson["properties"]))
			return E_FAIL;

		objectJson["components"] = nlohmann::json::array();
		for (const Shared<Component>& component : object->Get_Components()) {
			if (!component)
				continue;
			nlohmann::json componentJson;
			const string componentTypeName = rttr::type::get(*component).get_name().to_string();
			if (componentTypeName.empty() || GAME_INSTANCE->Find_RuntimeTypeId(componentTypeName) == 0) {
				LOG_ERROR(L"[SceneSerializer] Unregistered component type {}", FromUtf8(componentTypeName));
				return E_FAIL;
			}
			componentJson["typeName"] = componentTypeName;
			if (!SerializeProperties(*component, componentJson["properties"]))
				return E_FAIL;
			objectJson["components"].push_back(std::move(componentJson));
		}

		root["objects"][Engine::To_String(object->Get_ObjectGuid())] = std::move(objectJson);
	}

	try
	{
		std::filesystem::path path(filePath);
		if (!path.parent_path().empty())
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

	LOG_INFO(L"[SceneSerializer] Scene saved: {} objects", objects.size());
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

	// 파일 전체를 먼저 검사한다. 이 검사가 끝나기 전에는 현재 레벨을 변경하지 않는다.
	auto ValidateProperties = [&](const rttr::type& reflectedType,
		const nlohmann::json& properties) -> Bool {
		if (!properties.is_object())
			return false;
		for (auto it = properties.begin(); it != properties.end(); ++it) {
			const rttr::property property = reflectedType.get_property(it.key());
			if (!property.is_valid() ||
				!property.get_metadata(Meta_Key_Type::SaveData).is_valid() ||
				!IsCompatiblePropertyValue(property.get_type(), it.value())) {
				LOG_ERROR(L"[SceneSerializer] Invalid property {} on type {}",
					FromUtf8(it.key()), Helper::To_wString(reflectedType.get_name().to_string()));
				return false;
			}
		}
		return true;
	};

	if (!root.is_object() || !root.contains("schemaVersion") ||
		!root["schemaVersion"].is_number_unsigned() ||
		root["schemaVersion"].get<uint32>() != SceneSchemaVersion ||
		!root.contains("levelIndex") || !root["levelIndex"].is_number_unsigned() ||
		!root.contains("roots") || !root["roots"].is_array() ||
		!root.contains("objects") || !root["objects"].is_object()) {
		LOG_ERROR(L"[SceneSerializer] Unsupported or malformed scene schema");
		return E_FAIL;
	}

	const uint32 levelIndex = root["levelIndex"].get<uint32>();
	unordered_set<ObjectGuid, GuidHash> documentObjectGuids;
	unordered_map<ObjectGuid, string, GuidHash> objectKeys;
	unordered_map<ObjectGuid, ObjectGuid, GuidHash> parentByChild;
	unordered_map<ObjectGuid, vector<ObjectGuid>, GuidHash> childrenByParent;
	const rttr::type gameObjectBaseType = rttr::type::get<GameObject>();
	const rttr::type componentBaseType = rttr::type::get<Component>();

	for (auto objectIt = root["objects"].begin(); objectIt != root["objects"].end(); ++objectIt)
	{
		const string objectGuidText = objectIt.key();
		const nlohmann::json& objectJson = objectIt.value();
		ObjectGuid objectGuid{};
		if (!Try_Parse_ObjectGuid(objectGuidText, objectGuid) || !objectGuid.Is_Valid() ||
			!documentObjectGuids.emplace(objectGuid).second) {
			LOG_ERROR(L"[SceneSerializer] Invalid or duplicate ObjectGuid key {}", FromUtf8(objectGuidText));
			return E_FAIL;
		}
		objectKeys.emplace(objectGuid, objectGuidText);

		if (!objectJson.is_object() || !objectJson.contains("typeName") ||
			!objectJson["typeName"].is_string() || !objectJson.contains("properties") ||
			!objectJson.contains("components") || !objectJson["components"].is_array() ||
			!objectJson.contains("children") || !objectJson["children"].is_array() ||
			(objectJson.contains("name") && !objectJson["name"].is_string()) ||
			(objectJson.contains("stableChildKey") && !objectJson["stableChildKey"].is_string())) {
			LOG_ERROR(L"[SceneSerializer] Malformed object {}", FromUtf8(objectGuidText));
			return E_FAIL;
		}

		const string typeName = objectJson["typeName"].get<string>();
		const rttr::type objectType = rttr::type::get_by_name(typeName);
		if (GAME_INSTANCE->Find_RuntimeTypeId(typeName) == 0 || !objectType.is_valid() ||
			(objectType != gameObjectBaseType && !objectType.is_derived_from(gameObjectBaseType)) ||
			!GAME_INSTANCE->Can_Instantiate(PROTOTYPE::GAMEOBJECT, typeName, levelIndex) ||
			!ValidateProperties(objectType, objectJson["properties"])) {
			LOG_ERROR(L"[SceneSerializer] Invalid GameObject type {}", FromUtf8(typeName));
			return E_FAIL;
		}

		if (objectJson.contains("transform")) {
			const auto& transform = objectJson["transform"];
			if (!transform.is_object() || !transform.contains("position") ||
				!transform.contains("rotation") || !transform.contains("scale") ||
				!IsVector3Json(transform["position"]) || !IsVector3Json(transform["rotation"]) ||
				!IsVector3Json(transform["scale"])) {
				LOG_ERROR(L"[SceneSerializer] Malformed transform on {}", FromUtf8(typeName));
				return E_FAIL;
			}
		}

		unordered_set<RuntimeTypeId> componentTypes;
		for (const auto& componentJson : objectJson["components"]) {
			if (!componentJson.is_object() || !componentJson.contains("typeName") ||
				!componentJson["typeName"].is_string() || !componentJson.contains("properties"))
				return E_FAIL;
			const string componentTypeName = componentJson["typeName"].get<string>();
			const RuntimeTypeId componentRuntimeTypeId = GAME_INSTANCE->Find_RuntimeTypeId(componentTypeName);
			const rttr::type componentType = rttr::type::get_by_name(componentTypeName);
			if (componentRuntimeTypeId == 0 || !componentTypes.emplace(componentRuntimeTypeId).second ||
				!componentType.is_valid() ||
				(componentType != componentBaseType && !componentType.is_derived_from(componentBaseType)) ||
				!GAME_INSTANCE->Can_Instantiate(PROTOTYPE::COMPONENT, componentTypeName, levelIndex) ||
				!ValidateProperties(componentType, componentJson["properties"])) {
				LOG_ERROR(L"[SceneSerializer] Invalid or duplicate component type {}", FromUtf8(componentTypeName));
				return E_FAIL;
			}
		}
	}

	for (auto objectIt = root["objects"].begin(); objectIt != root["objects"].end(); ++objectIt) {
		ObjectGuid parentGuid{};
		Try_Parse_ObjectGuid(objectIt.key(), parentGuid);
		unordered_set<ObjectGuid, GuidHash> childGuids;
		unordered_set<wstring> stableChildKeys;
		for (const auto& childValue : objectIt.value()["children"]) {
			if (!childValue.is_string())
				return E_FAIL;
			ObjectGuid childGuid{};
			const string childGuidText = childValue.get<string>();
			if (!Try_Parse_ObjectGuid(childGuidText, childGuid) || childGuid == parentGuid ||
				!documentObjectGuids.contains(childGuid) || !childGuids.emplace(childGuid).second ||
				!parentByChild.emplace(childGuid, parentGuid).second) {
				LOG_ERROR(L"[SceneSerializer] Invalid or multiply-owned child {}", FromUtf8(childGuidText));
				return E_FAIL;
			}
			childrenByParent[parentGuid].push_back(childGuid);
			const auto& childJson = root["objects"][objectKeys.at(childGuid)];
			const wstring stableChildKey = FromUtf8(childJson.value("stableChildKey", ""));
			if (!stableChildKey.empty() && !stableChildKeys.emplace(stableChildKey).second) {
				LOG_ERROR(L"[SceneSerializer] Duplicate StableChildKey {}", stableChildKey);
				return E_FAIL;
			}
		}
	}

	unordered_set<ObjectGuid, GuidHash> rootGuids;
	for (const auto& rootValue : root["roots"]) {
		if (!rootValue.is_string())
			return E_FAIL;
		ObjectGuid rootGuid{};
		const string rootGuidText = rootValue.get<string>();
		if (!Try_Parse_ObjectGuid(rootGuidText, rootGuid) || !documentObjectGuids.contains(rootGuid) ||
			parentByChild.contains(rootGuid) || !rootGuids.emplace(rootGuid).second) {
			LOG_ERROR(L"[SceneSerializer] Invalid root {}", FromUtf8(rootGuidText));
			return E_FAIL;
		}
	}

	unordered_set<ObjectGuid, GuidHash> visitedGuids;
	const auto ValidateHierarchy = [&](auto&& self, ObjectGuid objectGuid) -> Bool {
		if (!visitedGuids.emplace(objectGuid).second)
			return false;
		const auto childIt = childrenByParent.find(objectGuid);
		if (childIt == childrenByParent.end())
			return true;
		for (const ObjectGuid childGuid : childIt->second) {
			if (!self(self, childGuid))
				return false;
		}
		return true;
	};
	for (const ObjectGuid rootGuid : rootGuids) {
		if (!ValidateHierarchy(ValidateHierarchy, rootGuid)) {
			LOG_ERROR(L"[SceneSerializer] Hierarchy cycle detected");
			return E_FAIL;
		}
	}
	if (visitedGuids.size() != documentObjectGuids.size()) {
		LOG_ERROR(L"[SceneSerializer] Orphaned object or hierarchy cycle detected");
		return E_FAIL;
	}

	GAME_INSTANCE->Clearing_ObjectManager(levelIndex);

	unordered_map<ObjectGuid, Shared<GameObject>, GuidHash> objectGuidMap;
	Bool deserializeFailed = false;
	struct ObjectData {
		Shared<GameObject> pObj;
		const nlohmann::json* pJson;
	};
	vector<ObjectData> loadList;
	auto RollbackLoad = [&]() {
		GAME_INSTANCE->Clearing_ObjectManager(levelIndex);
		loadList.clear();
		objectGuidMap.clear();
	};

	const auto RestoreObject = [&](auto&& self,
		ObjectGuid savedObjectGuid,
		const Shared<GameObject>& object) -> Bool {
		if (!object || object->Get_ObjectGuid() != savedObjectGuid)
			return false;

		const nlohmann::json& objectJson = root["objects"][objectKeys.at(savedObjectGuid)];
		const string savedTypeName = objectJson["typeName"].get<string>();
		const string instantiatedTypeName = rttr::type::get(*object).get_name().to_string();
		if (instantiatedTypeName != savedTypeName) {
			LOG_ERROR(L"[SceneSerializer] Prototype type mismatch: saved {}, created {}",
				FromUtf8(savedTypeName), FromUtf8(instantiatedTypeName));
			return false;
		}
		if (!objectGuidMap.emplace(savedObjectGuid, object).second) {
			LOG_ERROR(L"[SceneSerializer] Runtime object matched more than once {}",
				FromUtf8(objectKeys.at(savedObjectGuid)));
			return false;
		}

		object->Set_Name(FromUtf8(objectJson.value("name", "")));
		loadList.push_back({ object, &objectJson });

		const auto savedChildrenIt = childrenByParent.find(savedObjectGuid);
		const size_t savedChildCount = savedChildrenIt == childrenByParent.end()
			? 0
			: savedChildrenIt->second.size();

		for (size_t childIndex = 0; childIndex < savedChildCount; ++childIndex) {
			const ObjectGuid savedChildGuid = savedChildrenIt->second[childIndex];
			const nlohmann::json& childJson = root["objects"][objectKeys.at(savedChildGuid)];
			const string childTypeName = childJson["typeName"].get<string>();
			const wstring stableChildKey = FromUtf8(childJson.value("stableChildKey", ""));
			Shared<GameObject> child;

			if (!stableChildKey.empty()) {
				const auto& runtimeChildren = object->Get_Children();
				child = object->Find_Child(stableChildKey);
				if (childIndex >= runtimeChildren.size() || runtimeChildren[childIndex] != child) {
					LOG_ERROR(L"[SceneSerializer] Missing factory child {}", stableChildKey);
					return false;
				}

				const ObjectGuid derivedChildGuid = Derive_ChildObjectGuid(
					savedObjectGuid, stableChildKey, childTypeName);
				if (!child || child->Get_Parent() != object ||
					child->Get_StableChildKey() != stableChildKey ||
					!derivedChildGuid.Is_Valid() || derivedChildGuid != savedChildGuid ||
					child->Get_ObjectGuid() != savedChildGuid ||
					rttr::type::get(*child).get_name().to_string() != childTypeName) {
					LOG_ERROR(L"[SceneSerializer] Factory child structure mismatch {}", stableChildKey);
					return false;
				}
			}
			else {
				const auto& runtimeChildren = object->Get_Children();
				if (childIndex < runtimeChildren.size() &&
					!runtimeChildren[childIndex]->Get_StableChildKey().empty()) {
					LOG_ERROR(L"[SceneSerializer] Editor child overlaps factory child at index {}", childIndex);
					return false;
				}

				child = GAME_INSTANCE->Instantiate_GameObject(
					childTypeName, levelIndex, nullptr, savedChildGuid);
				if (!child || FAILED(object->Add_Child(child, L"", childIndex))) {
					LOG_ERROR(L"[SceneSerializer] Failed to create Editor child {}", FromUtf8(childTypeName));
					return false;
				}
			}

			if (!self(self, savedChildGuid, child))
				return false;
		}

		if (object->Get_Children().size() != savedChildCount) {
			LOG_ERROR(L"[SceneSerializer] Unexpected factory child count on {}", object->Get_Name());
			return false;
		}
		return true;
	};

	// Root만 생성한다. CodeDefined Child는 Root factory가 만든 객체를 문서와 매칭한다.
	for (const auto& rootValue : root["roots"]) {
		ObjectGuid rootGuid{};
		Try_Parse_ObjectGuid(rootValue.get<string>(), rootGuid);
		const nlohmann::json& rootJson = root["objects"][objectKeys.at(rootGuid)];
		const string rootTypeName = rootJson["typeName"].get<string>();
		const Shared<GameObject> rootObject = GAME_INSTANCE->Instantiate_GameObject(
			rootTypeName, levelIndex, nullptr, rootGuid);
		if (!rootObject || !RestoreObject(RestoreObject, rootGuid, rootObject)) {
			LOG_ERROR(L"[SceneSerializer] Failed to restore root type {}", FromUtf8(rootTypeName));
			deserializeFailed = true;
			break;
		}
	}

	if (objectGuidMap.size() != documentObjectGuids.size() ||
		GAME_INSTANCE->Get_GameObjects(levelIndex).size() != documentObjectGuids.size()) {
		LOG_ERROR(L"[SceneSerializer] Runtime and document hierarchy counts differ");
		deserializeFailed = true;
	}
	if (deserializeFailed) {
		RollbackLoad();
		return E_FAIL;
	}

	// 공통 속성 역직렬화 헬퍼 람다
	auto DeserializeProperties = [&](rttr::instance objInstance, const nlohmann::json& propsJson) -> Bool {
		rttr::type objType = objInstance.get_type();
		Bool succeeded = true;
		for (auto it = propsJson.begin(); it != propsJson.end(); ++it)
		{
			rttr::property prop = objType.get_property(it.key());
			if (!prop.is_valid()) {
				LOG_ERROR(L"[Serializer] Unknown property {} on type {}",
					FromUtf8(it.key()), Helper::To_wString(objType.get_name().to_string()));
				succeeded = false;
				continue;
			}
			
			// 실제 오브젝트에 값 꽂아넣기
			Bool valueApplied = false;
			if (it.value().is_number()) {
				if (prop.get_type() == rttr::type::get<uint32>()) valueApplied = prop.set_value(objInstance, static_cast<uint32>(it.value().get<Double>()));
				else if (prop.get_type() == rttr::type::get<int32>()) valueApplied = prop.set_value(objInstance, static_cast<int32>(it.value().get<Double>()));
				else if (prop.get_type() == rttr::type::get<Float>()) valueApplied = prop.set_value(objInstance, static_cast<Float>(it.value().get<Double>()));
				else if (prop.get_type() == rttr::type::get<Double>()) valueApplied = prop.set_value(objInstance, it.value().get<Double>());
			}
			else if (it.value().is_boolean()) { valueApplied = prop.set_value(objInstance, it.value().get<Bool>()); }
			else if (it.value().is_string()) {
				if (prop.get_type() == rttr::type::get<wstring>())
					valueApplied = prop.set_value(objInstance, FromUtf8(it.value().get<string>()));
				else if (prop.get_type() == rttr::type::get<string>())
					valueApplied = prop.set_value(objInstance, it.value().get<string>());
			}
			else if (it.value().is_array()) {
				auto& v = it.value();
				if (v.size() == 3 && prop.get_type() == rttr::type::get<Vector3>()) valueApplied = prop.set_value(objInstance, Vector3(v[0], v[1], v[2]));
				else if (v.size() == 3 && prop.get_type() == rttr::type::get<Float3>()) valueApplied = prop.set_value(objInstance, Float3(v[0], v[1], v[2]));
				else if (v.size() == 4 && prop.get_type() == rttr::type::get<Color>()) valueApplied = prop.set_value(objInstance, Color(v[0], v[1], v[2], v[3]));
				else if (v.size() == 4 && prop.get_type() == rttr::type::get<Float4>()) valueApplied = prop.set_value(objInstance, Float4(v[0], v[1], v[2], v[3]));
			}

			if (!valueApplied) {
				LOG_ERROR(L"[Serializer] Failed to set property {} on type {}",
					FromUtf8(it.key()), Helper::To_wString(objType.get_name().to_string()));
				succeeded = false;
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
		return succeeded;
	};


	// ── Pass 2: 데이터 주입 (Properties & Components) ─────────────────────
	for (auto& data : loadList)
	{
		auto& pObj = data.pObj;
		auto& objJson = *data.pJson;

		// 2. RTTR 프로퍼티 주입 (GameObject)
		if (objJson.contains("properties"))
		{
			if (!DeserializeProperties(*pObj, objJson["properties"]))
				deserializeFailed = true;
		}

		// 3. 컴포넌트 데이터 주입
		if (objJson.contains("components"))
		{
			unordered_map<string, Shared<Component>> runtimeComponents;
			for (const Shared<Component>& component : pObj->Get_Components()) {
				if (!component)
					continue;
				const string typeName = rttr::type::get(*component).get_name().to_string();
				if (typeName.empty() || !runtimeComponents.emplace(typeName, component).second) {
					LOG_ERROR(L"[SceneSerializer] Invalid runtime component structure on {}", pObj->Get_Name());
					deserializeFailed = true;
				}
			}

			if (runtimeComponents.size() != objJson["components"].size()) {
				LOG_ERROR(L"[SceneSerializer] Component count mismatch on {}", pObj->Get_Name());
				deserializeFailed = true;
			}

			for (auto& compJson : objJson["components"])
			{
				string typeName = compJson.value("typeName", "");
				if (typeName.empty() || GAME_INSTANCE->Find_RuntimeTypeId(typeName) == 0) {
					LOG_ERROR(L"[SceneSerializer] Unknown component registered name {}", FromUtf8(typeName));
					deserializeFailed = true;
					continue;
				}
				auto& compPropsJson = compJson["properties"];

				const auto componentIt = runtimeComponents.find(typeName);
				const Shared<Component> targetComp = componentIt == runtimeComponents.end()
					? nullptr
					: componentIt->second;
				if (targetComp)
				{
					if (!DeserializeProperties(*targetComp, compPropsJson))
						deserializeFailed = true;
				}
				else {
					LOG_ERROR(L"[SceneSerializer] Missing factory component {}", FromUtf8(typeName));
					deserializeFailed = true;
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

	if (deserializeFailed) {
		RollbackLoad();
		return E_FAIL;
	}

	// ── Pass 4: 로드 후 처리 ─────────────────────────────────────────────
	for (auto& data : loadList)
	{
		if (data.pObj)
		{
			data.pObj->Post_Load();
			if (data.pObj->Get_Transform()) {
				data.pObj->Get_Transform()->Update_WorldMatrix();
			}
		}
	}
	LOG_INFO(L"[SceneSerializer] RTTR Scene loaded: {} objects", loadList.size());
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
