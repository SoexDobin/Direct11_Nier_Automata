#include "Editor.h"
#include "EditorApp.h"
#include "framework.h"
#include "pch.h"

#define MAX_LOADSTRING 100

WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow) {
  if constexpr (_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  // Initialize Global Variables
  wcscpy_s(szTitle, g_projectSettings.windowTitle.c_str());
  wcscpy_s(szWindowClass, L"EDITOR_WND_CLASS");

  MyRegisterClass(hInstance);

  if (!InitInstance(hInstance, nCmdShow)) {
    return FALSE;
  }

  HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EDITOR));

  MSG msg = {};

  Unique<EditorApp> pEditorApp = EditorApp::Create();
  if (nullptr == pEditorApp)
    return FALSE;

  while (true) {
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      if (WM_QUIT == msg.message)
        break;

      if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }

    pEditorApp->Update();
    pEditorApp->Render();
  }

  pEditorApp.reset();

  return static_cast<int32>(msg.wParam);
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
  WNDCLASSEXW wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EDITOR));
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = nullptr;
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
  g_hInst = hInstance;

  RECT rc = {0, 0, static_cast<int32>(g_projectSettings.viewportWidth),
             static_cast<int32>(g_projectSettings.viewportHeight)};
  AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

  HWND hWnd =
      CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                    0, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr,
                    hInstance, nullptr);

  if (!hWnd) {
    return FALSE;
  }

  g_hWnd = hWnd;

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
                         LPARAM lParam) {
  if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
    return true;

  switch (message) {
  case WM_COMMAND: {
    int wmId = LOWORD(wParam);
    switch (wmId) {
    case IDM_EXIT:
      DestroyWindow(hWnd);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
    }
  } break;
  case WM_KEYDOWN: {
    if (wParam == VK_ESCAPE)
      PostQuitMessage(0);
  }
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}
