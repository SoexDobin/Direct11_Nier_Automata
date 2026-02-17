#pragma once

#include <imgui.h>
#include <windows.h>

extern inline HWND g_hWnd = { nullptr };
extern inline HINSTANCE g_hInst = { nullptr };
extern inline ENGINE_DESC g_projectSettings = {
	nullptr, 
	WINMODE::WIN, 
	L"Editor", 
	1920, 
	1080, 
	0, 
	true, 
	2
};

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


namespace Editor
{
	#define EDITOR		Editor::EditorManager::GetInstance()
}

using namespace Editor;