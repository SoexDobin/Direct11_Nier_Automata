#include "pch.h"
#include "Loader.h"
#include "SpdLogger.h"

#include "ClientSettingManager.h"


Loader::Loader(const ComPtr<ID3D11Device> &device,
               const ComPtr<ID3D11DeviceContext> &context)
    : Level{device, context} {}

Loader::~Loader() {}

HRESULT Loader::Initialize_Prototype() { return Level::Initialize_Prototype(); }

HRESULT Loader::Render_Level() { return Level::Render_Level(); }

uint32 APIENTRY ThreadMain(void* arg) 
{
    Shared<Loader>* loaderPtr = static_cast<Shared<Loader>*>(arg);
    Shared<Loader> loader = *loaderPtr;
    delete loaderPtr;

    if (!loader) return 1;
    if (FAILED(loader->Loading())) return 1;

    return 0;
}
HRESULT Loader::Initialize(void *arg) 
{
    m_NextLevelID = static_cast<LEVEL>(reinterpret_cast<size_t>(arg));
    InitializeCriticalSection(&m_CriticalSection);

	auto pSharedPtrToPass = new Shared<Loader>(shared_from_this());

  m_Thread = reinterpret_cast<HANDLE>(
      _beginthreadex(nullptr, 0, ThreadMain, pSharedPtrToPass, 0, nullptr));

  if (m_Thread == nullptr) {
      delete pSharedPtrToPass; // 실패 시 메모리 해제
      return E_FAIL;
  }
  
  return Level::Initialize(arg);
}

void Loader::On_Destroy() {
  WaitForSingleObject(m_Thread, INFINITE);
  CloseHandle(m_Thread);
  DeleteCriticalSection(&m_CriticalSection);

  Level::On_Destroy();
}

void Loader::Update_Level(Float timeDelta) { Level::Update_Level(timeDelta); }

HRESULT Loader::Loading() {
  HRESULT hr = {};

  EnterCriticalSection(&m_CriticalSection);
  CoInitializeEx(nullptr, COINIT_MULTITHREADED);

  switch (m_NextLevelID) {
  case LEVEL::STATIC:
	hr = Loading_Global_Prototype();
	  break;
  case LEVEL::LOGO:
    hr = Loading_For_LogoLevel();
    break;
  case LEVEL::GAMEPLAY:
    hr = Loading_For_GamePlayLevel();
    break;

  default:
    hr = E_FAIL;
  }

  CoUninitialize();
  LeaveCriticalSection(&m_CriticalSection);

  return hr;
}

HRESULT Loader::Print_LoadingText() 
{
	SetWindowText(g_hWnd, m_LoadingText);
	return S_OK;
}

HRESULT Loader::Loading_For_LogoLevel() {
	m_isFinished = false;

    lstrcpy(m_LoadingText, TEXT("Loading Logo Level Texture... "));
    if (FAILED(ClientSettingManager::GetInstance()->Load_Texture(LEVEL::LOGO)))
    {
        LOG_ERROR(L"Failed To Load Level Texture");
        return E_FAIL;
    }

	m_isFinished = true;
	return S_OK;
}

HRESULT Loader::Loading_For_GamePlayLevel() {
    m_isFinished = false;

    lstrcpy(m_LoadingText, TEXT("Loading GamePlay Level Texture... "));
    if (FAILED(ClientSettingManager::GetInstance()->Load_Texture(LEVEL::GAMEPLAY)))
    {
        LOG_ERROR(L"Failed To Load GamePlay Texture");
        return E_FAIL;
    }

    m_isFinished = true;
	return S_OK;
}

HRESULT Loader::Loading_Global_Prototype()
{
    m_isFinished = false;

    lstrcpy(m_LoadingText, TEXT("Loading Static Level... "));
    if (FAILED(ClientSettingManager::GetInstance()->Load_Texture(LEVEL::STATIC)))
    {
        LOG_ERROR(L"Failed To Load Global Texture");
        return E_FAIL;
    }
    if (FAILED(ClientSettingManager::GetInstance()->Load_Shader()))
    {
        LOG_ERROR(L"Failed To Load Shader");
        return E_FAIL;
    }

    m_isFinished = true;
    return S_OK;
}

Shared<Loader> Loader::Create(const ComPtr<ID3D11Device> &device, const ComPtr<ID3D11DeviceContext> &context, LEVEL nextLevelID)
{
    Shared<Loader> loader = make_shared<Loader>(device, context);

    if (FAILED(loader->Initialize(reinterpret_cast<void*>(nextLevelID)))) {
		MSG_BOX("Failed to Created : Loader");
    }

    return loader;
}
