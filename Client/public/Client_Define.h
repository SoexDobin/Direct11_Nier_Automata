#pragma once
#include <windows.h>

namespace Client
{
	enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, LEVEL_END };


	constexpr const Char* WorldMatrix = "g_WorldMatrix";
	constexpr const Char* ViewMatrix = "g_ViewMatrix";
	constexpr const Char* ProjMatrix = "g_ProjMatrix";

	constexpr const Char* CameraPosition = "g_CameraPosition";

	constexpr const Char* DiffuseMap = "g_DiffuseTexture";
	constexpr const Char* NormalMap = "g_NormalTexture";

	constexpr const Char* BoneMatrices = "g_BoneMatrices";
}

using namespace Client;