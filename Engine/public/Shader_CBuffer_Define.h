#pragma once

#include "Engine_Enum.h"
#include "Engine_Typedef.h"

namespace Engine
{
	enum MATERIAL_TEXTURE_MASK : uint32
	{
		MATERIAL_TEXTURE_BASE_COLOR = 1u << 0,
		MATERIAL_TEXTURE_NORMAL = 1u << 1,
		MATERIAL_TEXTURE_ORM = 1u << 2,
		MATERIAL_TEXTURE_EMISSIVE = 1u << 3,
		MATERIAL_TEXTURE_OPACITY = 1u << 4,
	};

	struct alignas(16) MaterialCB
	{
		static constexpr ConstantBuffer CBType = ConstantBuffer::Material;
		Vector4 baseColorTint{ 1.f, 1.f, 1.f, 1.f };
		Vector4 emissiveColor{ 0.f, 0.f, 0.f, 1.f };
		Float emissiveIntensity{};
		Float normalStrength{ 1.f };
		Float opacityMaskClipValue{ 0.3f };
		Float aoScale{ 1.f };
		Float roughnessScale{ 1.f };
		Float metalnessScale{ 1.f };
		uint32 textureMask{};
		Float padding{};
	};

	struct alignas(16) CameraCB
	{
		static constexpr ConstantBuffer CBType = ConstantBuffer::Camera;
		Matrix viewMatrix{ Matrix::Identity };
		Matrix projMatrix{ Matrix::Identity };
		Matrix viewInverseMatrix{ Matrix::Identity };
		Matrix projInverseMatrix{ Matrix::Identity };
		Vector4 cameraPosition{ 0.f, 0.f, 0.f, 1.f };
	};

	struct alignas(16) ObjectCB
	{
		static constexpr ConstantBuffer CBType = ConstantBuffer::Object;
		Matrix worldMatrix{ Matrix::Identity };
	};

	struct alignas(16) LightCB
	{
		static constexpr ConstantBuffer CBType = ConstantBuffer::Light;
		Vector4 direction{};
		Vector4 position{};
		Vector4 diffuse{};
		Vector4 ambient{};
		Vector4 specular{};
		Float range{};
		Float attenuationPower{ 1.f };
		Float2 padding{};
	};

	static_assert(sizeof(MaterialCB) == 64);
	static_assert(sizeof(CameraCB) == 272);
	static_assert(sizeof(ObjectCB) == 64);
	static_assert(sizeof(LightCB) == 96);
}
