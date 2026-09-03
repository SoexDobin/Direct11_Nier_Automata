#pragma once

namespace Engine
{
	using MetadataKeyType = const Char*;

	using DataTypeKey = const Char*;
	using DataTag = const Char*;
	using AssetTypeKey = const Char*;
	using SaveDataKey = const Char*;

	namespace Meta_Key_Type
	{
		inline constexpr MetadataKeyType DataType		= "DataType";	// 자료형
		inline constexpr MetadataKeyType TypeTag		= "TypeTag";	// 데이터 이름 
		inline constexpr MetadataKeyType AssetType		= "AssetType";	// Texture, Model, Object(특별한 타입 없으면 디폴트)
		inline constexpr MetadataKeyType SaveData		= "SaveData";	// Tag(ResourceTag) 등의 표현과 직렬화 대상인지
	}

	namespace Data_Type_Key /* For DataType */
	{
		inline constexpr DataTypeKey wString = "wString";
		inline constexpr DataTypeKey int32 = "int32";
		inline constexpr DataTypeKey uint32 = "uint32";
		inline constexpr DataTypeKey Float = "Float";
		inline constexpr DataTypeKey Double = "Double";
		inline constexpr DataTypeKey Vector2 = "Vector2";
		inline constexpr DataTypeKey Vector3 = "Vector3";
		inline constexpr DataTypeKey Vector4 = "Vector4";
		inline constexpr DataTypeKey Ptr	= "Pointer";
	}
	namespace Data_Tag /* For TypeTag */
	{
		inline constexpr DataTag NoneTag		= "None";			// Default DataType을 따라감
		inline constexpr DataTag Position		= "Position";		// Vector3
		inline constexpr DataTag Quaternion		= "Quaternion";		// Vector4
		inline constexpr DataTag Rotation		= "Rotation";		// Vector3
		inline constexpr DataTag Scale			= "Scale";			// Vector3
		inline constexpr DataTag Color			= "Color";			// Vector4
		inline constexpr DataTag ResourceTag	= "ResourceTag";	// Load 시 리소스 테그
	}
	namespace Asset_Type_Key  /* For AssetType */
	{
		inline constexpr AssetTypeKey NoneAsset		= "None"; // Default
		inline constexpr AssetTypeKey GameObject	= "GameObject";
		inline constexpr AssetTypeKey Texture		= "Texture";
		inline constexpr AssetTypeKey Model			= "ModelTag";
	}
	namespace Save_Data_Key /* For SaveData, Serialize */
	{
		inline constexpr SaveDataKey TransformPos		= "TransformPosition";
		inline constexpr SaveDataKey TransformRot		= "TransformRotation";
		inline constexpr SaveDataKey TransformScale		= "TransformScale";
		inline constexpr SaveDataKey TextureTag			= "TextureTag";
		inline constexpr SaveDataKey ModelTag			= "ModelTag";
		inline constexpr SaveDataKey AnimationPreset	= "AnimationPreset";
		inline constexpr SaveDataKey UIAnchor			= "UIAnchor";
		inline constexpr SaveDataKey ObjectReference	= "ObjectReference";
	}

}
