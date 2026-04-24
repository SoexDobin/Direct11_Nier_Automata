#pragma once

namespace Engine
{
	constexpr unsigned int g_MaxWidth = 8192;
	constexpr unsigned int g_MaxHeight = 8192;


	constexpr const Char* DefaultColor = "g_DefaultColor";

	constexpr const Char* WorldMatrix = "g_WorldMatrix";
	constexpr const Char* ViewMatrix = "g_ViewMatrix";
	constexpr const Char* ProjMatrix = "g_ProjMatrix";
	constexpr const Char* InverseViewMatrix = "g_ViewMatrixInverse";
	constexpr const Char* InverseProjMatrix = "g_ProjMatrixInverse";

	constexpr const Char* DefaultMap = "g_Texture";
	constexpr const Char* DiffuseMap = "g_DiffuseTexture";
	constexpr const Char* NormalMap = "g_NormalTexture";
	constexpr const Char* SpecularMap = "g_SpecularTexture";
	constexpr const Char* ShadeMap = "g_ShadeTexture";
	constexpr const Char* DepthMap = "g_DepthTexture";

	constexpr const Char* FarPlane = "g_FarPlane";
	constexpr const Char* LightDirection = "g_LightDirection";
	constexpr const Char* LightPosition = "g_LightPosition";
	constexpr const Char* LightRange = "g_LightRange";

	constexpr const Char* DiffuseLight = "g_LightDiffuse";
	constexpr const Char* SpecularLight = "g_LightSpecular";
	constexpr const Char* AmbientLight = "g_LightAmbient";

	constexpr const Char* LightViewMatrix = "g_LightViewMatrix";
	constexpr const Char* LightProjMatrix = "g_LightProjMatrix";
	constexpr const Char* LightDepthMap = "g_LightDepthTexture";


	const std::wstring RT_DIFFUSE = L"RenderTargetDiffuse";
	const std::wstring RT_NORMAL = L"RenderTargetNormal";
	const std::wstring RT_SHADE = L"RenderTargetShade";
	const std::wstring RT_SPECULAR = L"RenderTargetSpecular";
	const std::wstring RT_DEPTH = L"RenderTargetDepth";
	const std::wstring RT_LIGHT_DEPTH = L"RenderTargetLightDepth";

	const std::wstring MRT_GameObject = L"MultiRenderTargetGameObjects";
	const std::wstring MRT_LIGHT = L"MultiRenderTargetLights";
	const std::wstring MRT_SHADOW = L"MultiRenderTargetShadow";
}
