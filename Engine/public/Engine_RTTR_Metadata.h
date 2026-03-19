#pragma once

namespace Engine
{
	using MetadataKey = const char*;
	using MetadataValue = const char*;

	namespace Serialize_Data_Field
	{
		inline constexpr MetadataKey DataType		= "DataType";
		inline constexpr MetadataKey Widget		= "Widget";
		inline constexpr MetadataKey Min			= "Min";
		inline constexpr MetadataKey Max			= "Max";
		inline constexpr MetadataKey Speed		= "Speed";
		inline constexpr MetadataKey AssetType	= "AssetType";
		inline constexpr MetadataKey SaveData		= "SaveData";
		inline constexpr MetadataKey NoSerialize	= "NoSerialize";
	}

	namespace Serialize_Data_Type
	{
		inline constexpr MetadataValue Vector3		= "Vector3";
		inline constexpr MetadataValue Matrix		= "Matrix";
		inline constexpr MetadataValue ColorPicker	= "ColorPicker";
		inline constexpr MetadataValue SliderFloat	= "SliderFloat";
		inline constexpr MetadataValue SliderInt	= "SliderInt";
		inline constexpr MetadataValue DragFloat	= "DragFloat";
		inline constexpr MetadataValue DragFloat3	= "DragFloat3";
		inline constexpr MetadataValue GameObject	= "GameObject";
		inline constexpr MetadataValue AssetDrop	= "AssetDrop";
		inline constexpr MetadataValue Texture		= "Texture";
		inline constexpr MetadataValue Model		= "Model";
	}
}
