#pragma once
#include <windows.h>

#ifdef CLIENT_EXPORTS
#define CLIENT_DLL __declspec(dllexport)
#else
#define CLIENT_DLL __declspec(dllimport)
#endif

namespace Client
{
	enum class LEVEL { STATIC, LOADING, TITLE, GAMEPLAY, LEVEL_END };
	enum class ATK_TYPE { LIGHT, HEAVY, END };

	constexpr const Char* WorldMatrix = "g_WorldMatrix";
	constexpr const Char* ViewMatrix = "g_ViewMatrix";
	constexpr const Char* ProjMatrix = "g_ProjMatrix";

	constexpr const Char* CameraPosition = "g_CameraPosition";

	constexpr const Char* DefaultMap = "g_Texture";
	constexpr const Char* DiffuseMap = "g_DiffuseTexture";
	constexpr const Char* NormalMap = "g_NormalTexture";

	constexpr const Char* BoneMatrices = "g_BoneMatrices";

	constexpr const Char* DirectionLight = "g_LightDir";
	constexpr const Char* DiffuseLight = "g_LightDiffuse";
	constexpr const Char* SpecularLight = "g_LightSpecular";
	constexpr const Char* AmbientLight = "g_LightAmbient";
}

using namespace Client;