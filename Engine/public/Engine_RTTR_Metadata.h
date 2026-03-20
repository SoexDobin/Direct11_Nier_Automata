#pragma once

namespace Engine
{
	using MetadataKey = const char*;
	using MetadataValue = const char*;

	namespace Meta_Key
	{
		inline constexpr MetadataKey DataType		= "DataType";
		inline constexpr MetadataKey Widget			= "Widget";
		inline constexpr MetadataKey Min			= "Min";
		inline constexpr MetadataKey Max			= "Max";
		inline constexpr MetadataKey Speed			= "Speed";
		inline constexpr MetadataKey AssetType		= "AssetType";
		inline constexpr MetadataKey SaveData		= "SaveData";
		inline constexpr MetadataKey NoSerialize	= "NoSerialize";
	}

	namespace Serialize_Data_Field
	{
		inline constexpr MetadataValue Position		= "Position";
		inline constexpr MetadataValue Rotation		= "Rotation";
		inline constexpr MetadataValue Scale			= "Scale";
		inline constexpr MetadataValue ColorVector4	= "ColorVector4";
		inline constexpr MetadataValue QuaternionVector4 = "QuaternionVector4";
		inline constexpr MetadataValue TextureTag	= "TextureTag";
		inline constexpr MetadataValue ModelTag		= "ModelTag";
		inline constexpr MetadataValue GameObject	= "GameObject";
		inline constexpr MetadataValue NoSerialize	= "NoSerialize";
	}

	namespace Widget_Type
	{
		inline constexpr MetadataValue DragFloat		= "DragFloat";
		inline constexpr MetadataValue DragFloat3	= "DragFloat3";
		inline constexpr MetadataValue ColorPicker	= "ColorPicker";
		inline constexpr MetadataValue SliderFloat	= "SliderFloat";
		inline constexpr MetadataValue SliderInt		= "SliderInt";
		inline constexpr MetadataValue AssetDrop		= "AssetDrop";
	}
}
