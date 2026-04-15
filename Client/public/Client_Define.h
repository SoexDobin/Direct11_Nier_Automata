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
	enum class ATK_TYPE { LIGHT, HEAVY, POD, END };

	constexpr const Char* CameraPosition = "g_CameraPosition";


	constexpr const Char* BoneMatrices = "g_BoneMatrices";

	// 특수 값
	constexpr const Char* HpRatio = "g_HpRatio"; // Tex Pass1
}

using namespace Client;