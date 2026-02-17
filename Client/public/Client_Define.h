#pragma once

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

namespace Client
{
	enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, LEVEL_END };
}

extern inline ENGINE_DESC g_projectSettings = {
	nullptr,
	WINMODE::WIN,
	L"NieRAutomata",
	1920,
	1080,
	ETOI(Client::LEVEL::LOGO),
	false,
	0
};



using namespace Client;

#include "Client_Function.h"