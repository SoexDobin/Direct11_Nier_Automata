#include "TypeCatalog.h"
#include <optional>
#include "Engine_RTTR_Metadata.h"
#include "Camera.h"
#include "Component.h"
#include "GameObject.h"
#include "Object.h"
#include "PartObject.h"
#include "SpdLogger.h"
#include "String_Helper.h"
#include "UIObject.h"

namespace
{
	Engine::REFLECTION_VALUE_TYPE Get_ReflectionValueType(const rttr::type& type)
	{
		using namespace Engine;
		if (type == rttr::type::get<Bool>()) return REFLECTION_VALUE_TYPE::BOOL;
		if (type == rttr::type::get<int32>()) return REFLECTION_VALUE_TYPE::INT32;
		if (type == rttr::type::get<uint32>()) return REFLECTION_VALUE_TYPE::UINT32;
		if (type == rttr::type::get<Float>()) return REFLECTION_VALUE_TYPE::FLOAT;
		if (type == rttr::type::get<Double>()) return REFLECTION_VALUE_TYPE::DOUBLE;
		if (type == rttr::type::get<std::string>()) return REFLECTION_VALUE_TYPE::STRING;
		if (type == rttr::type::get<std::wstring>()) return REFLECTION_VALUE_TYPE::WSTRING;
		if (type == rttr::type::get<Vector3>()) return REFLECTION_VALUE_TYPE::VECTOR3;
		if (type == rttr::type::get<Float3>()) return REFLECTION_VALUE_TYPE::FLOAT3;
		if (type == rttr::type::get<Color>()) return REFLECTION_VALUE_TYPE::COLOR;
		if (type == rttr::type::get<Float4>()) return REFLECTION_VALUE_TYPE::FLOAT4;
		if (type == rttr::type::get<AnimationPresetSnapshot>())
			return REFLECTION_VALUE_TYPE::ANIMATION_PRESET;
		if (type == rttr::type::get<ObjectGuid>())
			return REFLECTION_VALUE_TYPE::OBJECT_REF;
		return REFLECTION_VALUE_TYPE::NONE;
	}

	std::string Get_MetadataText(const rttr::property& property, const Engine::MetadataKeyType key)
	{
		const rttr::variant metadata = property.get_metadata(key);
		if (!metadata.is_valid() || !metadata.is_type<const Engine::Char*>())
			return {};
		const Engine::Char* value = metadata.get_value<const Engine::Char*>();
		return value ? std::string{ value } : std::string{};
	}

	Engine::Bool To_ReflectionValue(const rttr::variant& value, Engine::ReflectionValue& outValue)
	{
		using namespace Engine;
		if (value.is_type<Bool>()) outValue.data = value.get_value<Bool>();
		else if (value.is_type<int32>()) outValue.data = value.get_value<int32>();
		else if (value.is_type<uint32>()) outValue.data = value.get_value<uint32>();
		else if (value.is_type<Float>()) outValue.data = value.get_value<Float>();
		else if (value.is_type<Double>()) outValue.data = value.get_value<Double>();
		else if (value.is_type<std::string>()) outValue.data = value.get_value<std::string>();
		else if (value.is_type<std::wstring>()) outValue.data = value.get_value<std::wstring>();
		else if (value.is_type<Vector3>()) outValue.data = value.get_value<Vector3>();
		else if (value.is_type<Float3>()) outValue.data = value.get_value<Float3>();
		else if (value.is_type<Color>()) outValue.data = value.get_value<Color>();
		else if (value.is_type<Float4>()) outValue.data = value.get_value<Float4>();
		else if (value.is_type<AnimationPresetSnapshot>())
			outValue.data = value.get_value<AnimationPresetSnapshot>();
		else if (value.is_type<ObjectGuid>()) outValue.data = value.get_value<ObjectGuid>();
		else return false;
		return true;
	}

	Engine::Bool Set_ReflectedValue(const rttr::property& property, rttr::instance target,
		const Engine::ReflectionValue& value)
	{
		return std::visit([&](const auto& typedValue) -> Engine::Bool {
			using ValueType = std::decay_t<decltype(typedValue)>;
			if constexpr (std::is_same_v<ValueType, std::monostate>) {
				return false;
			}
			else {
				return property.get_type() == rttr::type::get<ValueType>() &&
					property.set_value(target, typedValue);
			}
		}, value.data);
	}

	Engine::ReflectedPropertyDescriptor Make_UIAnchor_Property()
	{
		using namespace Engine;
		ReflectedPropertyDescriptor property;
		property.info.registeredName = "Anchor";
		property.info.valueType = REFLECTION_VALUE_TYPE::UINT32;
		property.info.dataTag = "UIAnchor";
		property.info.assetType = Asset_Type_Key::NoneAsset;
		property.info.saveDataKey = Save_Data_Key::UIAnchor;
		property.info.isWritable = true;
		property.info.isSerializable = true;
		property.read = [](Object& target, ReflectionValue& outValue) -> HRESULT {
			auto* uiObject = dynamic_cast<UIObject*>(&target);
			if (!uiObject)
				return E_NOINTERFACE;
			outValue.data = static_cast<uint32>(uiObject->Get_AnchorState());
			return S_OK;
		};
		property.write = [](Object& target, const ReflectionValue& value) -> HRESULT {
			auto* uiObject = dynamic_cast<UIObject*>(&target);
			const uint32* anchor = value.Try_Get<uint32>();
			if (!uiObject || !anchor || *anchor > static_cast<uint32>(UI_ANCHOR::BOTTOM_RIGHT))
				return E_INVALIDARG;
			uiObject->Set_AnchorState(static_cast<UI_ANCHOR>(*anchor));
			return S_OK;
		};
		return property;
	}

	Engine::ReflectedPropertyDescriptor Make_CameraTarget_Property()
	{
		using namespace Engine;
		ReflectedPropertyDescriptor property;
		property.info.registeredName = "Target";
		property.info.valueType = REFLECTION_VALUE_TYPE::OBJECT_REF;
		property.info.dataTag = "ObjectRef";
		property.info.assetType = Asset_Type_Key::GameObject;
		property.info.saveDataKey = Save_Data_Key::ObjectReference;
		property.info.expectedBaseRegisteredName = "GameObject";
		property.info.isWritable = true;
		property.info.isSerializable = true;
		property.read = [](Object& target, ReflectionValue& outValue) -> HRESULT {
			auto* camera = dynamic_cast<Camera*>(&target);
			if (!camera)
				return E_NOINTERFACE;
			outValue.data = camera->Get_TargetObjectGuid();
			return S_OK;
		};
		property.write = [](Object& target, const ReflectionValue& value) -> HRESULT {
			auto* camera = dynamic_cast<Camera*>(&target);
			const ObjectGuid* targetGuid = value.Try_Get<ObjectGuid>();
			return camera && targetGuid
				? camera->Set_TargetObjectGuid(*targetGuid)
				: E_INVALIDARG;
		};
		return property;
	}
}

HRESULT Engine::Registry::Initialize(void* arg) {
    return Refresh();
}

void Engine::Registry::On_Destroy() {
    Clear();
}

HRESULT Engine::Registry::Register_Descriptors(const ReflectionDescriptorBatch& descriptors)
{
	std::unordered_set<std::string> typeNames;
	std::unordered_set<RuntimeTypeId> typeIds;
	for (const ReflectedTypeDescriptor& descriptor : descriptors.types) {
		const ReflectedTypeInfo& info = descriptor.info;
		if (info.registeredName.empty() || info.runtimeTypeId == 0 ||
			(info.objectKind != REFLECTED_OBJECT_KIND::OBJECT &&
			 info.objectKind != REFLECTED_OBJECT_KIND::GAMEOBJECT &&
			 info.objectKind != REFLECTED_OBJECT_KIND::COMPONENT) ||
			!descriptor.createPrototype ||
			!typeNames.emplace(info.registeredName).second ||
			!typeIds.emplace(info.runtimeTypeId).second) {
			return E_INVALIDARG;
		}

		std::unordered_set<std::string> propertyNames;
		for (const ReflectedPropertyDescriptor& property : descriptor.properties) {
			const Bool isObjectReference =
				property.info.valueType == REFLECTION_VALUE_TYPE::OBJECT_REF;
			const Bool hasExpectedBase =
				!property.info.expectedBaseRegisteredName.empty();
			if (property.info.registeredName.empty() ||
				property.info.valueType == REFLECTION_VALUE_TYPE::NONE ||
				isObjectReference != hasExpectedBase ||
				!property.read ||
				(property.info.isWritable && !property.write) ||
				!propertyNames.emplace(property.info.registeredName).second) {
				return E_INVALIDARG;
			}
		}
	}

	std::unordered_set<std::string> enumNames;
	for (const ReflectedEnumInfo& enumInfo : descriptors.enums) {
		if (enumInfo.registeredName.empty() || enumInfo.values.empty() ||
			!enumNames.emplace(enumInfo.registeredName).second) {
			return E_INVALIDARG;
		}
		std::unordered_set<std::string> valueNames;
		for (const ReflectedEnumValue& value : enumInfo.values) {
			if (value.name.empty() || !valueNames.emplace(value.name).second)
				return E_INVALIDARG;
		}
	}

	std::lock_guard lock(m_Mutex);
	for (const ReflectedTypeDescriptor& descriptor : descriptors.types) {
		if (m_ExternalTypes.contains(descriptor.info.registeredName) ||
			m_ByRegisteredName.contains(descriptor.info.registeredName) ||
			m_RegisteredNameByRuntimeTypeId.contains(descriptor.info.runtimeTypeId)) {
			return HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS);
		}
	}
	for (const ReflectedEnumInfo& enumInfo : descriptors.enums) {
		if (m_ExternalEnums.contains(enumInfo.registeredName) ||
			m_EnumsByRegisteredName.contains(enumInfo.registeredName)) {
			return HRESULT_FROM_WIN32(ERROR_ALREADY_EXISTS);
		}
	}

	for (const ReflectedTypeDescriptor& descriptor : descriptors.types)
		m_ExternalTypes.emplace(descriptor.info.registeredName, descriptor);
	for (const ReflectedEnumInfo& enumInfo : descriptors.enums)
		m_ExternalEnums.emplace(enumInfo.registeredName, enumInfo);
	return S_OK;
}

HRESULT Engine::Registry::Refresh() {
    std::unordered_map<std::string, Entry> byRegisteredName;
    std::unordered_map<RuntimeTypeId, std::string> registeredNameByRuntimeTypeId;
    std::unordered_map<std::string, ReflectedEnumInfo> enumsByRegisteredName;

	const rttr::type objectType = rttr::type::get<Object>();
	const rttr::type gameObjectType = rttr::type::get<GameObject>();
	const rttr::type componentType = rttr::type::get<Component>();
	const rttr::type partObjectType = rttr::type::get<PartObject>();
	const rttr::type uiObjectType = rttr::type::get<UIObject>();
	const rttr::type cameraType = rttr::type::get<Camera>();

    for (const rttr::type reflectedType : rttr::type::get_types()) {
        const std::string registeredName = reflectedType.get_name().to_string();
        const auto rawTypeId = reflectedType.get_id();
        if (registeredName.empty() || rawTypeId == 0) {
            return E_FAIL;
        }

        const RuntimeTypeId runtimeTypeId = static_cast<RuntimeTypeId>(rawTypeId);
        Entry entry{ registeredName, runtimeTypeId, reflectedType, {}, {}, {}, {}, {} };
		entry.typeInfo.registeredName = registeredName;
		entry.typeInfo.runtimeTypeId = runtimeTypeId;
		for (const rttr::type baseType : reflectedType.get_base_classes()) {
			const std::string baseName = baseType.get_name().to_string();
			if (!baseName.empty())
				entry.typeInfo.baseRegisteredNames.push_back(baseName);
		}
		std::ranges::sort(entry.typeInfo.baseRegisteredNames);
		if (reflectedType == gameObjectType || reflectedType.is_derived_from(gameObjectType)) {
			entry.typeInfo.objectKind = REFLECTED_OBJECT_KIND::GAMEOBJECT;
			if (reflectedType == uiObjectType || reflectedType.is_derived_from(uiObjectType))
				entry.typeInfo.authoringMode = HIERARCHY_AUTHORING_MODE::EDITOR_DEFINED;
			else if (reflectedType == partObjectType || reflectedType.is_derived_from(partObjectType))
				entry.typeInfo.authoringMode = HIERARCHY_AUTHORING_MODE::LEAF;
		}
		else if (reflectedType == componentType || reflectedType.is_derived_from(componentType))
			entry.typeInfo.objectKind = REFLECTED_OBJECT_KIND::COMPONENT;
		else if (reflectedType == objectType || reflectedType.is_derived_from(objectType))
			entry.typeInfo.objectKind = REFLECTED_OBJECT_KIND::OBJECT;

		const rttr::method createMethod = reflectedType.get_method("Create");
		const rttr::variant levelMetadata = createMethod.is_valid()
			? createMethod.get_metadata("Level")
			: rttr::variant{};
		if (levelMetadata.is_valid()) {
			rttr::variant convertedLevel = levelMetadata;
			if (!convertedLevel.convert(rttr::type::get<uint32>())) {
				LOG_ERROR(L"Registry rejected invalid Level metadata on {}",
					Helper::To_wString(registeredName));
				return E_FAIL;
			}
			entry.typeInfo.level = convertedLevel.get_value<uint32>();
			entry.typeInfo.hasLevel = true;
		}

		if (reflectedType.is_enumeration()) {
			const rttr::enumeration reflectedEnum = reflectedType.get_enumeration();
			ReflectedEnumInfo enumInfo;
			enumInfo.registeredName = registeredName;
			if (reflectedEnum.get_metadata("AnimationPreset").to_bool())
				enumInfo.category = "AnimationState";

			const auto names = reflectedEnum.get_names();
			const auto values = reflectedEnum.get_values();
			auto nameIt = names.begin();
			auto valueIt = values.begin();
			for (; nameIt != names.end() && valueIt != values.end(); ++nameIt, ++valueIt) {
				Bool converted = false;
				const std::int64_t value = valueIt->to_int64(&converted);
				if (!converted) {
					LOG_ERROR(L"Registry rejected non-integral enum value on {}",
						Helper::To_wString(registeredName));
					return E_FAIL;
				}
				enumInfo.values.push_back({ nameIt->to_string(), value });
			}
			if (nameIt != names.end() || valueIt != values.end() || enumInfo.values.empty() ||
				!enumsByRegisteredName.emplace(registeredName, std::move(enumInfo)).second) {
				LOG_ERROR(L"Registry rejected invalid enum descriptor {}",
					Helper::To_wString(registeredName));
				return E_FAIL;
			}
		}

        std::unordered_set<std::string> propertyNames;
        for (const rttr::property property : reflectedType.get_properties()) {
            const std::string propertyName = property.get_name().to_string();
            if (propertyName.empty() || !propertyNames.emplace(propertyName).second) {
                LOG_ERROR(L"Registry rejected duplicate or empty property name on {}",
                          Helper::To_wString(registeredName));
                return E_FAIL;
            }

			ReflectedPropertyInfo propertyInfo;
			propertyInfo.registeredName = propertyName;
			propertyInfo.valueType = Get_ReflectionValueType(property.get_type());
			propertyInfo.dataTag = Get_MetadataText(property, Meta_Key_Type::TypeTag);
			propertyInfo.assetType = Get_MetadataText(property, Meta_Key_Type::AssetType);
			propertyInfo.saveDataKey = Get_MetadataText(property, Meta_Key_Type::SaveData);
			propertyInfo.isWritable = !property.is_readonly();
			propertyInfo.isSerializable = property.get_metadata(Meta_Key_Type::SaveData).is_valid();
			if (propertyInfo.isSerializable &&
				(propertyInfo.valueType == REFLECTION_VALUE_TYPE::NONE || !propertyInfo.isWritable)) {
				LOG_ERROR(L"Registry rejected unsupported serializable property {} on {}",
					Helper::To_wString(propertyName), Helper::To_wString(registeredName));
				return E_FAIL;
			}
			entry.properties.push_back(std::move(propertyInfo));
			entry.reflectedProperties.emplace(propertyName, property);
        }
		std::ranges::sort(entry.properties, {}, &ReflectedPropertyInfo::registeredName);
		if (entry.typeInfo.objectKind == REFLECTED_OBJECT_KIND::GAMEOBJECT &&
			(reflectedType == uiObjectType || reflectedType.is_derived_from(uiObjectType)) &&
			std::ranges::none_of(entry.properties, [](const ReflectedPropertyInfo& property) {
				return property.registeredName == "Anchor";
			})) {
			ReflectedPropertyDescriptor anchorProperty = Make_UIAnchor_Property();
			entry.properties.push_back(anchorProperty.info);
			entry.externalProperties.emplace(anchorProperty.info.registeredName,
				std::move(anchorProperty));
			std::ranges::sort(entry.properties, {}, &ReflectedPropertyInfo::registeredName);
		}
		if (reflectedType == cameraType || reflectedType.is_derived_from(cameraType)) {
			std::erase_if(entry.properties, [](const ReflectedPropertyInfo& property) {
				return property.registeredName == "TargetID";
			});
			entry.reflectedProperties.erase("TargetID");
			ReflectedPropertyDescriptor targetProperty = Make_CameraTarget_Property();
			entry.properties.push_back(targetProperty.info);
			entry.externalProperties.emplace(targetProperty.info.registeredName,
				std::move(targetProperty));
			std::ranges::sort(entry.properties, {}, &ReflectedPropertyInfo::registeredName);
		}

		if (!byRegisteredName.emplace(registeredName, std::move(entry)).second)
			return E_FAIL;
		if (!registeredNameByRuntimeTypeId.emplace(runtimeTypeId, registeredName).second)
			return E_FAIL;
    }

    for (const auto& [registeredName, entry] : byRegisteredName) {
        if (entry.reflectedType == objectType ||
            !entry.reflectedType.is_derived_from(objectType) ||
            !entry.reflectedType.get_constructor().is_valid()) {
            continue;
        }

        if (!entry.reflectedType.get_method("Create").is_valid() &&
            !entry.reflectedType.get_method("CreatePrototype").is_valid()) {
            LOG_ERROR(L"Registry rejected concrete Object type without a factory: {}",
                      Helper::To_wString(registeredName));
            return E_FAIL;
        }
    }

	std::unordered_map<std::string, ReflectedTypeDescriptor> externalTypes;
	std::unordered_map<std::string, ReflectedEnumInfo> externalEnums;
	{
		std::lock_guard lock(m_Mutex);
		externalTypes = m_ExternalTypes;
		externalEnums = m_ExternalEnums;
	}

	for (const auto& [registeredName, descriptor] : externalTypes) {
		const ReflectedTypeInfo& info = descriptor.info;
		if (byRegisteredName.contains(registeredName) ||
			registeredNameByRuntimeTypeId.contains(info.runtimeTypeId)) {
			LOG_ERROR(L"Registry rejected conflicting external type {}",
				Helper::To_wString(registeredName));
			return E_FAIL;
		}

		Entry entry{ registeredName, info.runtimeTypeId, rttr::type::get_by_name(""),
			info, {}, {}, {}, {} };
		entry.externalFactory = descriptor.createPrototype;
		std::unordered_set<std::string> propertyNames;
		for (const ReflectedPropertyDescriptor& property : descriptor.properties) {
			if (!propertyNames.emplace(property.info.registeredName).second)
				return E_FAIL;
			entry.properties.push_back(property.info);
			entry.externalProperties.emplace(property.info.registeredName, property);
		}
		std::ranges::sort(entry.properties, {}, &ReflectedPropertyInfo::registeredName);
		byRegisteredName.emplace(registeredName, std::move(entry));
		registeredNameByRuntimeTypeId.emplace(info.runtimeTypeId, registeredName);
	}

	for (const auto& [registeredName, enumInfo] : externalEnums) {
		if (!enumsByRegisteredName.emplace(registeredName, enumInfo).second) {
			LOG_ERROR(L"Registry rejected conflicting external enum {}",
				Helper::To_wString(registeredName));
			return E_FAIL;
		}
	}

	std::unordered_set<std::string> composedTypes;
	std::unordered_set<std::string> composingTypes;
	std::function<Bool(const std::string&)> composeProperties = [&](const std::string& registeredName) -> Bool {
		if (composedTypes.contains(registeredName))
			return true;
		if (!composingTypes.emplace(registeredName).second) {
			LOG_ERROR(L"Registry detected an external base cycle at {}",
				Helper::To_wString(registeredName));
			return false;
		}
		auto entryIt = byRegisteredName.find(registeredName);
		if (entryIt == byRegisteredName.end()) {
			LOG_ERROR(L"Registry could not resolve external base {}",
				Helper::To_wString(registeredName));
			return false;
		}
		Entry& entry = entryIt->second;
		if (!entry.externalFactory) {
			composingTypes.erase(registeredName);
			composedTypes.emplace(registeredName);
			return true;
		}

		const auto ownProperties = entry.properties;
		const auto ownExternalProperties = entry.externalProperties;
		entry.properties.clear();
		entry.reflectedProperties.clear();
		entry.externalProperties.clear();
		for (const std::string& baseName : entry.typeInfo.baseRegisteredNames) {
			if (!composeProperties(baseName))
				return false;
			const Entry& base = byRegisteredName.at(baseName);
			for (const ReflectedPropertyInfo& property : base.properties) {
				if (std::ranges::none_of(entry.properties, [&](const ReflectedPropertyInfo& current) {
					return current.registeredName == property.registeredName;
				})) entry.properties.push_back(property);
			}
			entry.reflectedProperties.insert(base.reflectedProperties.begin(), base.reflectedProperties.end());
			entry.externalProperties.insert(base.externalProperties.begin(), base.externalProperties.end());
		}
		for (const ReflectedPropertyInfo& property : ownProperties) {
			std::erase_if(entry.properties, [&](const ReflectedPropertyInfo& current) {
				return current.registeredName == property.registeredName;
			});
			entry.properties.push_back(property);
		}
		for (const auto& [propertyName, property] : ownExternalProperties) {
			entry.reflectedProperties.erase(propertyName);
			entry.externalProperties[propertyName] = property;
		}
		std::ranges::sort(entry.properties, {}, &ReflectedPropertyInfo::registeredName);
		composingTypes.erase(registeredName);
		composedTypes.emplace(registeredName);
		return true;
	};

	for (const auto& [registeredName, descriptor] : externalTypes) {
		if (!composeProperties(registeredName)) {
			LOG_ERROR(L"Registry rejected invalid external base graph on {}",
				Helper::To_wString(registeredName));
			return E_FAIL;
		}
	}

    std::lock_guard lock(m_Mutex);
    m_ByRegisteredName = std::move(byRegisteredName);
    m_RegisteredNameByRuntimeTypeId = std::move(registeredNameByRuntimeTypeId);
	m_EnumsByRegisteredName = std::move(enumsByRegisteredName);
    return S_OK;
}

rttr::type Engine::Registry::Find_Type(std::string_view registeredName) const {
    std::lock_guard lock(m_Mutex);
    const auto it = m_ByRegisteredName.find(std::string{registeredName});
    return it == m_ByRegisteredName.end()
        ? rttr::type::get_by_name("")
        : it->second.reflectedType;
}

Engine::RuntimeTypeId Engine::Registry::Find_RuntimeTypeId(std::string_view registeredName) const {
    std::lock_guard lock(m_Mutex);
    const auto it = m_ByRegisteredName.find(std::string{registeredName});
    return it == m_ByRegisteredName.end() ? RuntimeTypeId{} : it->second.runtimeTypeId;
}

std::string Engine::Registry::Find_RegisteredName(RuntimeTypeId runtimeTypeId) const {
    std::lock_guard lock(m_Mutex);
    const auto it = m_RegisteredNameByRuntimeTypeId.find(runtimeTypeId);
    return it == m_RegisteredNameByRuntimeTypeId.end() ? std::string{} : it->second;
}

HRESULT Engine::Registry::Find_TypeInfo(std::string_view registeredName,
	ReflectedTypeInfo& outInfo) const
{
	outInfo = {};
	std::lock_guard lock(m_Mutex);
	const auto it = m_ByRegisteredName.find(std::string{ registeredName });
	if (it == m_ByRegisteredName.end())
		return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
	outInfo = it->second.typeInfo;
	return S_OK;
}

HRESULT Engine::Registry::Find_TypeInfo(RuntimeTypeId runtimeTypeId,
	ReflectedTypeInfo& outInfo) const
{
	outInfo = {};
	std::lock_guard lock(m_Mutex);
	const auto nameIt = m_RegisteredNameByRuntimeTypeId.find(runtimeTypeId);
	if (nameIt == m_RegisteredNameByRuntimeTypeId.end())
		return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
	const auto typeIt = m_ByRegisteredName.find(nameIt->second);
	if (typeIt == m_ByRegisteredName.end())
		return E_FAIL;
	outInfo = typeIt->second.typeInfo;
	return S_OK;
}

std::vector<Engine::ReflectedTypeInfo> Engine::Registry::Get_Types() const
{
	std::lock_guard lock(m_Mutex);
	std::vector<ReflectedTypeInfo> result;
	result.reserve(m_ByRegisteredName.size());
	for (const auto& [registeredName, entry] : m_ByRegisteredName)
		result.push_back(entry.typeInfo);
	std::ranges::sort(result, {}, &ReflectedTypeInfo::registeredName);
	return result;
}

std::vector<Engine::ReflectedPropertyInfo> Engine::Registry::Get_Properties(
	std::string_view registeredName) const
{
	std::lock_guard lock(m_Mutex);
	const auto it = m_ByRegisteredName.find(std::string{ registeredName });
	return it == m_ByRegisteredName.end()
		? std::vector<ReflectedPropertyInfo>{}
		: it->second.properties;
}

HRESULT Engine::Registry::Find_Enum(std::string_view registeredName,
	ReflectedEnumInfo& outInfo) const
{
	outInfo = {};
	std::lock_guard lock(m_Mutex);
	const auto it = m_EnumsByRegisteredName.find(std::string{ registeredName });
	if (it == m_EnumsByRegisteredName.end())
		return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
	outInfo = it->second;
	return S_OK;
}

std::vector<Engine::ReflectedEnumInfo> Engine::Registry::Get_Enums(
	std::string_view category) const
{
	std::lock_guard lock(m_Mutex);
	std::vector<ReflectedEnumInfo> result;
	for (const auto& [registeredName, enumInfo] : m_EnumsByRegisteredName) {
		if (category.empty() || enumInfo.category == category)
			result.push_back(enumInfo);
	}
	std::ranges::sort(result, {}, &ReflectedEnumInfo::registeredName);
	return result;
}

std::vector<Engine::ReflectedPropertyInfo> Engine::Registry::Get_Properties(
	RuntimeTypeId runtimeTypeId) const
{
	std::lock_guard lock(m_Mutex);
	const auto nameIt = m_RegisteredNameByRuntimeTypeId.find(runtimeTypeId);
	if (nameIt == m_RegisteredNameByRuntimeTypeId.end())
		return {};
	const auto typeIt = m_ByRegisteredName.find(nameIt->second);
	return typeIt == m_ByRegisteredName.end()
		? std::vector<ReflectedPropertyInfo>{}
		: typeIt->second.properties;
}

HRESULT Engine::Registry::Read_Property(Object& target, std::string_view propertyName,
	ReflectionValue& outValue) const
{
	outValue = {};
	if (target.Get_RuntimeTypeId() == 0 || propertyName.empty())
		return E_INVALIDARG;

	std::optional<rttr::property> reflectedProperty;
	std::function<HRESULT(Object&, ReflectionValue&)> externalRead;
	{
		std::lock_guard lock(m_Mutex);
		const auto nameIt = m_RegisteredNameByRuntimeTypeId.find(target.Get_RuntimeTypeId());
		if (nameIt == m_RegisteredNameByRuntimeTypeId.end())
			return E_FAIL;
		const auto typeIt = m_ByRegisteredName.find(nameIt->second);
		if (typeIt == m_ByRegisteredName.end())
			return E_FAIL;
		const std::string key{ propertyName };
		const auto propertyIt = typeIt->second.reflectedProperties.find(key);
		if (propertyIt != typeIt->second.reflectedProperties.end())
			reflectedProperty = propertyIt->second;
		else if (const auto externalIt = typeIt->second.externalProperties.find(key);
			externalIt != typeIt->second.externalProperties.end())
			externalRead = externalIt->second.read;
		else return E_FAIL;
	}
	if (externalRead)
		return externalRead(target, outValue);

	const rttr::variant reflectedValue = reflectedProperty->get_value(rttr::instance{ target });
	return reflectedValue.is_valid() && To_ReflectionValue(reflectedValue, outValue)
		? S_OK
		: E_NOINTERFACE;
}

HRESULT Engine::Registry::Write_Property(Object& target, std::string_view propertyName,
	const ReflectionValue& value) const
{
	if (target.Get_RuntimeTypeId() == 0 || propertyName.empty() || !value.Is_Valid())
		return E_INVALIDARG;

	std::optional<rttr::property> reflectedProperty;
	std::function<HRESULT(Object&, const ReflectionValue&)> externalWrite;
	{
		std::lock_guard lock(m_Mutex);
		const auto nameIt = m_RegisteredNameByRuntimeTypeId.find(target.Get_RuntimeTypeId());
		if (nameIt == m_RegisteredNameByRuntimeTypeId.end())
			return E_FAIL;
		const auto typeIt = m_ByRegisteredName.find(nameIt->second);
		if (typeIt == m_ByRegisteredName.end())
			return E_FAIL;
		const std::string key{ propertyName };
		const auto propertyIt = typeIt->second.reflectedProperties.find(key);
		if (propertyIt != typeIt->second.reflectedProperties.end()) {
			if (propertyIt->second.is_readonly())
				return E_ACCESSDENIED;
			reflectedProperty = propertyIt->second;
		}
		else if (const auto externalIt = typeIt->second.externalProperties.find(key);
			externalIt != typeIt->second.externalProperties.end()) {
			if (!externalIt->second.info.isWritable || !externalIt->second.write)
				return E_ACCESSDENIED;
			externalWrite = externalIt->second.write;
		}
		else return E_FAIL;
	}
	if (externalWrite)
		return externalWrite(target, value);

	return Set_ReflectedValue(*reflectedProperty, rttr::instance{ target }, value)
		? S_OK
		: E_INVALIDARG;
}

std::vector<Engine::Registry::Entry> Engine::Registry::Get_ObjectTypes() const {
    std::lock_guard lock(m_Mutex);
    std::vector<Entry> result;
    for (const auto& [registeredName, entry] : m_ByRegisteredName) {
		if (entry.typeInfo.objectKind == REFLECTED_OBJECT_KIND::GAMEOBJECT ||
			entry.typeInfo.objectKind == REFLECTED_OBJECT_KIND::COMPONENT)
            result.push_back(entry);
    }
    std::ranges::sort(result, {}, &Entry::registeredName);
    return result;
}

HRESULT Engine::Registry::Create_Prototype(std::string_view registeredName,
	Shared<Object>& outPrototype) const
{
	outPrototype.reset();
	std::function<Shared<Object>()> factory;
	RuntimeTypeId runtimeTypeId{};
	std::string canonicalName;
	{
		std::lock_guard lock(m_Mutex);
		const auto it = m_ByRegisteredName.find(std::string{ registeredName });
		if (it == m_ByRegisteredName.end())
			return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
		if (!it->second.externalFactory)
			return S_FALSE;
		factory = it->second.externalFactory;
		runtimeTypeId = it->second.runtimeTypeId;
		canonicalName = it->second.registeredName;
	}

	outPrototype = factory();
	if (!outPrototype)
		return E_FAIL;
	outPrototype->Assign_ReflectedIdentity(runtimeTypeId, canonicalName);
	return S_OK;
}

void Engine::Registry::Clear() {
    std::lock_guard lock(m_Mutex);
	m_ExternalEnums.clear();
	m_ExternalTypes.clear();
	m_EnumsByRegisteredName.clear();
    m_RegisteredNameByRuntimeTypeId.clear();
    m_ByRegisteredName.clear();
}
