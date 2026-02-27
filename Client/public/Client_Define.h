#pragma once

#include <windows.h>

namespace Client
{
	enum class LEVEL { STATIC, LOADING, LOGO, GAMEPLAY, LEVEL_END };
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
extern ENGINE_DESC g_projectSettings;

using namespace Client;

#include "Client_Function.h"