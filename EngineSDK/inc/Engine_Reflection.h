#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "Engine_ID.h"
#include "Engine_Typedef.h"

namespace Engine
{
	class Object;

	template <typename T>
	constexpr RuntimeTypeId Make_ExternalRuntimeTypeId() noexcept
	{
#if defined(_MSC_VER)
		constexpr std::string_view signature = __FUNCSIG__;
#else
		constexpr std::string_view signature = __PRETTY_FUNCTION__;
#endif
		RuntimeTypeId hash = sizeof(RuntimeTypeId) == 8
			? static_cast<RuntimeTypeId>(14695981039346656037ull)
			: static_cast<RuntimeTypeId>(2166136261u);
		constexpr RuntimeTypeId prime = sizeof(RuntimeTypeId) == 8
			? static_cast<RuntimeTypeId>(1099511628211ull)
			: static_cast<RuntimeTypeId>(16777619u);
		for (const char character : signature) {
			hash ^= static_cast<unsigned char>(character);
			hash *= prime;
		}
		const RuntimeTypeId externalBit = RuntimeTypeId{ 1 } << (sizeof(RuntimeTypeId) * 8 - 1);
		return (hash | externalBit) == 0 ? externalBit : (hash | externalBit);
	}

	enum class REFLECTED_OBJECT_KIND : uint8
	{
		VALUE,
		OBJECT,
		GAMEOBJECT,
		COMPONENT,
	};

	enum class HIERARCHY_AUTHORING_MODE : uint8
	{
		CODE_DEFINED,
		EDITOR_DEFINED,
		LEAF,
		TRANSIENT,
	};

	enum class REFLECTION_VALUE_TYPE : uint8
	{
		NONE,
		BOOL,
		INT32,
		UINT32,
		FLOAT,
		DOUBLE,
		STRING,
		WSTRING,
		VECTOR3,
		FLOAT3,
		COLOR,
		FLOAT4,
		ANIMATION_PRESET,
	};

	struct AnimationPresetClip final
	{
		std::wstring relativePath;
		std::vector<std::string> states;
	};

	struct AnimationPresetSnapshot final
	{
		uint32 schemaVersion{ 2 };
		std::string animationEnum;
		std::vector<AnimationPresetClip> animations;

		Bool Is_Empty() const noexcept
		{
			return animationEnum.empty() && animations.empty();
		}
	};

	struct ReflectionValue final
	{
		using Storage = std::variant<std::monostate, Bool, int32, uint32, Float, Double,
			std::string, std::wstring, Vector3, Float3, Color, Float4,
			AnimationPresetSnapshot>;

		Storage data{};

		REFLECTION_VALUE_TYPE Get_Type() const noexcept
		{
			return static_cast<REFLECTION_VALUE_TYPE>(data.index());
		}

		Bool Is_Valid() const noexcept
		{
			return Get_Type() != REFLECTION_VALUE_TYPE::NONE;
		}

		template <typename T>
		const T* Try_Get() const noexcept
		{
			return std::get_if<T>(&data);
		}

		template <typename T>
		T* Try_Get() noexcept
		{
			return std::get_if<T>(&data);
		}
	};

	struct ReflectedTypeInfo final
	{
		std::string registeredName;
		RuntimeTypeId runtimeTypeId{};
		std::vector<std::string> baseRegisteredNames;
		REFLECTED_OBJECT_KIND objectKind{ REFLECTED_OBJECT_KIND::VALUE };
		HIERARCHY_AUTHORING_MODE authoringMode{ HIERARCHY_AUTHORING_MODE::CODE_DEFINED };
		uint32 level{};
		Bool hasLevel{};
	};

	struct ReflectedPropertyInfo final
	{
		std::string registeredName;
		REFLECTION_VALUE_TYPE valueType{ REFLECTION_VALUE_TYPE::NONE };
		std::string dataTag;
		std::string assetType;
		std::string saveDataKey;
		Bool isReadable{ true };
		Bool isWritable{};
		Bool isSerializable{};
	};

	struct ReflectedEnumValue final
	{
		std::string name;
		std::int64_t value{};
	};

	struct ReflectedEnumInfo final
	{
		std::string registeredName;
		std::string category;
		std::vector<ReflectedEnumValue> values;
	};

	struct ReflectedPropertyDescriptor final
	{
		ReflectedPropertyInfo info;
		std::function<HRESULT(Object&, ReflectionValue&)> read;
		std::function<HRESULT(Object&, const ReflectionValue&)> write;
	};

	struct ReflectedTypeDescriptor final
	{
		ReflectedTypeInfo info;
		std::function<Shared<Object>()> createPrototype;
		std::vector<ReflectedPropertyDescriptor> properties;
	};

	struct ReflectionDescriptorBatch final
	{
		std::vector<ReflectedTypeDescriptor> types;
		std::vector<ReflectedEnumInfo> enums;
	};
}
