#pragma once

#include <array>
#include <string_view>

namespace Engine
{
	namespace ShaderCB
	{
		inline constexpr std::string_view Material = "MaterialBuffer";
		inline constexpr std::string_view Camera = "CameraBuffer";
		inline constexpr std::string_view Object = "ObjectBuffer";
		inline constexpr std::string_view Light = "LightBuffer";

		inline constexpr std::array<std::string_view, ETOI(ConstantBuffer::END)> Names =
		{
			Material,
			Camera,
			Object,
			Light,
		};
	}

	constexpr const Char* WorldMatrix = "g_WorldMatrix";
	constexpr const Char* ViewMatrix = "g_ViewMatrix";
	constexpr const Char* ProjMatrix = "g_ProjMatrix";


	constexpr const Char* DefaultMap = "g_Texture";
	constexpr const Char* DiffuseMap = "g_DiffuseTexture";
	constexpr const Char* NormalMap = "g_NormalTexture";
	constexpr const Char* ShadeMap = "g_ShadeTexture";


	constexpr const Char* DirectionLight = "g_LightDir";
	constexpr const Char* DiffuseLight = "g_LightDiffuse";
	constexpr const Char* SpecularLight = "g_LightSpecular";
	constexpr const Char* AmbientLight = "g_LightAmbient";

	constexpr const Char* LightDirection = "g_LightDirection";


	const std::wstring RT_DIFFUSE = L"RenderTargetDiffuse";
	const std::wstring RT_NORMAL = L"RenderTargetNormal";
	const std::wstring RT_SHADE = L"RenderTargetShade";


	const std::wstring MRT_GameObject = L"MultiRenderTargetGameObjects";
	const std::wstring MRT_LIGHT = L"MultiRenderTargetLights";
}
