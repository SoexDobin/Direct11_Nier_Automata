#include "pch.h"
#include "Loader.h"
#include "ClientApp.h"
#include "SpdLogger.h"

#include "ClientSettingManager.h"
#include "FreeCamera.h"
#include "Game.h"
#include "LoadingBackground.h"
#include "LoadingLogo.h"

#include "Monster.h"
#include "Terrain.h"



Loader::Loader(const ComPtr<ID3D11Device> &device, const ComPtr<ID3D11DeviceContext> &context)
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
    if (nullptr == arg)
    {
        LOG_CRITICAL(L"Failed to Initialize Loader By LoaderDesc");
        return E_FAIL;
    }

    LOADER_DESC& desc = *static_cast<LOADER_DESC*>(arg);

    m_NextLevelID = desc.nextLevelID;
    m_OwnerLevel = desc.ownerLevel;
    InitializeCriticalSection(&m_CriticalSection);

	auto pSharedPtrToPass = new Shared<Loader>(static_pointer_cast<Loader>(shared_from_this()));
    
	m_Thread = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, ThreadMain, pSharedPtrToPass, 0, nullptr));
    
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

void Loader::Update_Level(Float timeDelta)
{
	Level::Update_Level(timeDelta);
}

HRESULT Loader::Loading() {
  HRESULT hr = {};

  EnterCriticalSection(&m_CriticalSection);
  if (SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
  {
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
  }

  CoUninitialize();
  LeaveCriticalSection(&m_CriticalSection);

  return hr;
}

HRESULT Loader::Loading_For_LogoLevel() {
	m_isFinished = false;

    lstrcpy(m_LoadingText, TEXT("Loading Logo Level Texture... "));


    if (FAILED(GAME_INSTANCE->Add_Prototype(ETOI(m_NextLevelID),
        Terrain::Create(m_Device, m_Context), L"MainTerrain")))
        return E_FAIL;

    if (FAILED(GAME_INSTANCE->Add_Prototype(ETOI(m_NextLevelID),
        LoadingBackground::Create(m_Device, m_Context), L"UI_Loading_BackGround")))
        return E_FAIL;
    if (FAILED(GAME_INSTANCE->Add_Prototype(ETOI(m_NextLevelID),
        LoadingLogo::Create(m_Device, m_Context), L"UI_Loading_Logo")))
        return E_FAIL;

    if (FAILED(GAME_INSTANCE->Add_Prototype(ETOI(m_NextLevelID),
        Monster::Create(m_Device, m_Context), L"Monster")))
        return E_FAIL;

    if (FAILED(GAME_INSTANCE->Add_Prototype(ETOI(m_NextLevelID),
        FreeCamera::Create(m_Device, m_Context), L"MainCamera")))
        return E_FAIL;

    LIGHT_DESC			LightDesc{};
    LightDesc.type = LIGHT::DIRECTIONAL;
    LightDesc.direction = Vector4(1.f, -1.f, 1.f, 0.f);
    LightDesc.diffuse = Vector4(1.f, 1.f, 1.f, 1.f);
    LightDesc.ambient = Vector4(1.f, 1.f, 1.f, 1.f);
    LightDesc.specular = Vector4(1.f, 1.f, 1.f, 1.f);

    if (FAILED(GAME_INSTANCE->Add_Light(LightDesc)))
        return E_FAIL;

    FreeCamera::FREE_CAMERA_DESC desc{};
    desc.mouseSensitive = 5.f;
    desc.eye = Vector4{ 0.f, 10.f, -10.f, 1.f };
    desc.at = Vector4{ 0.f, 0.f, 0.f, 1.f };
    desc.up = Vector4{ 0.f, 1.f, 0.f, 1.f };
    desc.fovY = XMConvertToRadians(60.f);
    desc.aspect = static_cast<Float>(ClientSettingManager::g_EngineDesc.viewportWidth) / static_cast<Float>(ClientSettingManager::g_EngineDesc.viewportHeight);
	desc.nearPlane = 0.1f;
    desc.farPlane = 500.f;

    GAME_INSTANCE->Instantiate<Terrain>(L"MainTerrain", ETOI(m_NextLevelID));
    auto back = GAME_INSTANCE->Instantiate<LoadingBackground>(L"UI_Loading_BackGround", ETOI(m_NextLevelID));
    auto logo = GAME_INSTANCE->Instantiate<LoadingLogo>(L"UI_Loading_Logo", ETOI(m_NextLevelID));
    //back->Add_Child(logo);

    GAME_INSTANCE->Instantiate<Monster>(L"Monster", ETOI(m_NextLevelID));

    auto cam = GAME_INSTANCE->Instantiate<FreeCamera>(L"MainCamera", ETOI(m_NextLevelID), &desc);
	GAME_INSTANCE->Set_MainCamera(cam);


    m_isFinished = true;
    if (!m_OwnerLevel.expired())
		m_OwnerLevel.lock()->Set_LoadFinishFlag(m_isFinished);
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
    if (!m_OwnerLevel.expired())
        m_OwnerLevel.lock()->Set_LoadFinishFlag(m_isFinished);
	return S_OK;
}

HRESULT Loader::Loading_Global_Prototype()
{
    m_isFinished = false;

    lstrcpy(m_LoadingText, TEXT("Loading Static Level... "));
    lstrcpy(m_LoadingText, TEXT("Loading Static Level Texture... "));
    if (FAILED(ClientSettingManager::GetInstance()->Load_Texture(LEVEL::STATIC)))
    {
        LOG_ERROR(L"Failed To Load Global Texture");
        return E_FAIL;
    }
    lstrcpy(m_LoadingText, TEXT("Loading Static Level Shader... "));
    if (FAILED(ClientSettingManager::GetInstance()->Load_Shader()))
    {
        LOG_ERROR(L"Failed To Load Shader");
        return E_FAIL;
    }

    lstrcpy(m_LoadingText, TEXT("Loading Static Level Shader... "));

    if (FAILED(GAME_INSTANCE->Add_Prototype(ETOI(LEVEL::STATIC),
        Terrain::Create(m_Device, m_Context))))
        return E_FAIL;

    m_isFinished = true;
    if (!m_OwnerLevel.expired())
        m_OwnerLevel.lock()->Set_LoadFinishFlag(m_isFinished);
    return S_OK;
}

Shared<Loader> Loader::Create(const ComPtr<ID3D11Device> &device, const ComPtr<ID3D11DeviceContext> &context, LEVEL nextLevelID, const Shared<Level>& ownerLevel)
{
    Shared<Loader> loader = make_shared<Loader>(device, context);

    LOADER_DESC desc{};
    desc.nextLevelID = nextLevelID;
    desc.ownerLevel = ownerLevel;

    if (FAILED(loader->Initialize(&desc))) {
		MSG_BOX("Failed to Created : Loader");
    }

    return loader;
}
