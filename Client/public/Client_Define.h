#pragma once

#include <Windows.h>

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

namespace Client
{
	const unsigned int g_WinSizeX = { 1280 }; 	
	const unsigned int g_WinSizeY = { 720 };

	enum class LEVEL
	{
		STATIC,
		LOADING,
		LOGO,
		GAMEPLAY,
		END
	};

	
}

using namespace Client;
