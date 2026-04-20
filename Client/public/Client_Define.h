#pragma once
#include <windows.h>

#ifdef CLIENT_EXPORTS
#define CLIENT_DLL __declspec(dllexport)
#else
#define CLIENT_DLL __declspec(dllimport)
#endif

namespace Client
{
	enum class LEVEL { STATIC, LOADING, TITLE, GAMEPLAY, GAMEPLAY2, LEVEL_END };
	enum class ATK_TYPE { LIGHT, HEAVY, POD, END };

	constexpr const Char* DiffuseMap1 = "g_DiffuseTexture1";
	constexpr const Char* DiffuseMap2 = "g_DiffuseTexture2";
	constexpr const Char* NormalMap1 = "g_NormalTexture1";
	constexpr const Char* NormalMap2 = "g_NormalTexture2";

	constexpr const Char* CameraPosition = "g_CameraPosition";

	constexpr const Char* BlendWeight = "g_BlendWeight";
	constexpr const Char* BoneMatrices = "g_BoneMatrices";

	// 특수 값
	constexpr const Char* HpRatio = "g_HpRatio"; // Tex Pass1
	constexpr const Char* TargetPosition = "g_TargetWorldPosition";


	// VTXWORLDMESH SHADER
	constexpr const tChar* VTXWORLDMESH = L"VTX_WORLD_MESH";
}

using namespace Client;