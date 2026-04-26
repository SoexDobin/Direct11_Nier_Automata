#include "pch.h"
#include "Loader.h"
#include "ClientApp.h"
#include "SpdLogger.h"

#include "ClientSettingManager.h"
#include "Game.h"
#include "NavigationManager.h"


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
    m_LoadStatic = desc.isLoadStatic;
    InitializeCriticalSection(&m_CriticalSection);

	auto sharedPtrToPass = new Shared<Loader>(static_pointer_cast<Loader>(shared_from_this()));
    
	m_Thread = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, ThreadMain, sharedPtrToPass, 0, nullptr));
    
    if (m_Thread == nullptr) {
        delete sharedPtrToPass; // 실패 시 메모리 해제
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
        if (m_LoadStatic)
            hr = Loading_Global_Prototype();

        switch (m_NextLevelID) {
        case LEVEL::TITLE:
            hr = Loading_For_TitleLevel();
            break;
        case LEVEL::GAMEPLAY:
            hr = Loading_For_GamePlayLevel();
            break;
        case LEVEL::GAMEPLAY2:
        	hr = Loading_For_GamePlayLevel2();
            break;
        default:
            hr = E_FAIL;
        }
    }

    CoUninitialize();
    LeaveCriticalSection(&m_CriticalSection);

    return hr;
}

HRESULT Loader::Loading_For_TitleLevel() {
	m_isFinished = false;

    if (SUCCEEDED(ClientSettingManager::GetInstance()->Sync_TextureJson_FromCSV())) {
        if (FAILED(ClientSettingManager::GetInstance()->Load_Textures_FromJson(LEVEL::TITLE))) {
            LOG_ERROR(L"Failed to Load TITLE Textures");
            return E_FAIL;
        }
    }
    if (SUCCEEDED(ClientSettingManager::GetInstance()->Sync_ModelJson_FromCSV())) {
        if (FAILED(ClientSettingManager::GetInstance()->Load_Model_FromJson(LEVEL::TITLE))) {
            LOG_ERROR(L"Failed to Load TITLE Model");
            return E_FAIL;
        }
    }

    if (FAILED(ClientSettingManager::GetInstance()->Ready_Client_Prototypes(LEVEL::TITLE))) {
        LOG_ERROR(L"Failed to Ready Client TITLE Prototypes");
        return E_FAIL;
    }

    m_isFinished = true;
    if (!m_OwnerLevel.expired())
		m_OwnerLevel.lock()->Set_LoadFinishFlag(m_isFinished);
	return S_OK;
}

HRESULT Loader::Loading_For_GamePlayLevel() {
    if (FAILED(GAME_INSTANCE->Clear_Lights()))
    {
        return E_FAIL;
    }
    m_isFinished = false;

    if (FAILED(ClientSettingManager::GetInstance()->Load_Shader()))
        return E_FAIL;
    if (SUCCEEDED(ClientSettingManager::GetInstance()->Sync_TextureJson_FromCSV())) {
        if (FAILED(ClientSettingManager::GetInstance()->Load_Textures_FromJson(LEVEL::GAMEPLAY))) {
            LOG_ERROR(L"Failed to Load GAMEPLAY Textures");
            return E_FAIL;
        }
    }
    if (SUCCEEDED(ClientSettingManager::GetInstance()->Sync_ModelJson_FromCSV())) {
        if (FAILED(ClientSettingManager::GetInstance()->Load_Model_FromJson(LEVEL::GAMEPLAY))) {
            LOG_ERROR(L"Failed to Load GAMEPLAY Model");
            return E_FAIL;
        }
    }

    if (FAILED(ClientSettingManager::GetInstance()->Ready_Client_Prototypes(LEVEL::GAMEPLAY))) {
        LOG_ERROR(L"Failed to Ready Client GAMEPLAY Prototypes");
        return E_FAIL;
    }

    LIGHT_DESC			LightDesc{};
    LightDesc.type = LIGHT::DIRECTIONAL;
    LightDesc.direction = Vector4(1.f, -1.f, 1.f, 0.f);
    LightDesc.diffuse = Vector4(1.f, 1.f, 1.f, 1.f);
    LightDesc.ambient = Vector4(0.2f, 0.2f, 0.2f, 1.f);
    LightDesc.specular = Vector4(0.4f, 0.4f, 0.4f, 1.f);

    if (FAILED(GAME_INSTANCE->Add_Light(LightDesc)))
        return E_FAIL;


    SHADOW_LIGHT_DESC		shadowLightDesc{};
    shadowLightDesc.at = Vector4(250.f, 0.f, 100.f, 1.f);
    shadowLightDesc.eye = Vector4(250.f, 400.f, -150.f, 1.f);
    shadowLightDesc.fovy = XMConvertToRadians(60.f);
    shadowLightDesc.aspect = 2.5f;
    shadowLightDesc.nearPlane = 0.1f;
    shadowLightDesc.farPlane = 1000.f;
    if (FAILED(GAME_INSTANCE->Add_ShadowLight(shadowLightDesc)))
        return E_FAIL;

    GAME_INSTANCE->PlaySoundFX(L"CityRuins", SOUNDCHANNEL::CHANNEL_1, 0.3f);

    m_isFinished = true;
    if (!m_OwnerLevel.expired())
        m_OwnerLevel.lock()->Set_LoadFinishFlag(m_isFinished);
	return S_OK;
}

HRESULT Loader::Loading_For_GamePlayLevel2()
{
    if (FAILED(GAME_INSTANCE->Clear_Lights()))
    {
        return E_FAIL;
    }
    m_isFinished = false;

    if (FAILED(ClientSettingManager::GetInstance()->Load_Shader()))
        return E_FAIL;
    if (SUCCEEDED(ClientSettingManager::GetInstance()->Sync_TextureJson_FromCSV())) {
        if (FAILED(ClientSettingManager::GetInstance()->Load_Textures_FromJson(LEVEL::GAMEPLAY2))) {
            LOG_ERROR(L"Failed to Load GAMEPLAY2 Textures");
            return E_FAIL;
        }
    }
    if (SUCCEEDED(ClientSettingManager::GetInstance()->Sync_ModelJson_FromCSV())) {
        if (FAILED(ClientSettingManager::GetInstance()->Load_Model_FromJson(LEVEL::GAMEPLAY2))) {
            LOG_ERROR(L"Failed to Load GAMEPLAY2 Model");
            return E_FAIL;
        }
    }

    if (FAILED(ClientSettingManager::GetInstance()->Ready_Client_Prototypes(LEVEL::GAMEPLAY2))) {
        LOG_ERROR(L"Failed to Ready Client GAMEPLAY2 Prototypes");
        return E_FAIL;
    }

    LIGHT_DESC			LightDesc{};
    LightDesc.type = LIGHT::DIRECTIONAL;
    LightDesc.direction = Vector4(1.f, -1.f, 1.f, 0.f);
    LightDesc.diffuse = Vector4(0.4f, 0.2f, 1.f, 1.f);
    LightDesc.ambient = Vector4(0.5, 0.5, 0.5, 1.f);
    LightDesc.specular = Vector4(0.5f, 0.5f, 0.5f, 1.f);

    if (FAILED(GAME_INSTANCE->Add_Light(LightDesc)))
        return E_FAIL;

    SHADOW_LIGHT_DESC		shadowLightDesc{};
    shadowLightDesc.eye = Vector4(0.f, 35.f, 30.f, 1.f);
    shadowLightDesc.at = Vector4(10.f, 0.f, 50.f, 1.f);
    shadowLightDesc.fovy = XMConvertToRadians(100.f);
    shadowLightDesc.aspect = GAME_INSTANCE->Get_ViewportDesc().Width / GAME_INSTANCE->Get_ViewportDesc().Height;
    shadowLightDesc.nearPlane = 0.1f;
    shadowLightDesc.farPlane = 1000.f;
    if (FAILED(GAME_INSTANCE->Add_ShadowLight(shadowLightDesc)))
        return E_FAIL;

    GAME_INSTANCE->PlaySoundFX(L"AmusementPark", SOUNDCHANNEL::CHANNEL_1, 0.3f);

    m_isFinished = true;
    if (!m_OwnerLevel.expired())
        m_OwnerLevel.lock()->Set_LoadFinishFlag(m_isFinished);

    return S_OK;
}

HRESULT Loader::Loading_Global_Prototype()
{
    m_isFinished = false;

    if (FAILED(ClientSettingManager::GetInstance()->Load_Shader()))
        return E_FAIL;
    if (SUCCEEDED(ClientSettingManager::GetInstance()->Sync_TextureJson_FromCSV())) {
        if (FAILED(ClientSettingManager::GetInstance()->Load_Textures_FromJson(LEVEL::STATIC))) {
            LOG_ERROR(L"Failed to Load Textures");
            return E_FAIL;
        }
    }
    if (SUCCEEDED(ClientSettingManager::GetInstance()->Sync_ModelJson_FromCSV())) {
        if (FAILED(ClientSettingManager::GetInstance()->Load_Model_FromJson(LEVEL::STATIC))) {
            LOG_ERROR(L"Failed to Load Model");
            return E_FAIL;
        }
    }

    if (SUCCEEDED(ClientSettingManager::GetInstance()->Sync_SoundJson_FromCSV())) {
        if (FAILED(ClientSettingManager::GetInstance()->Load_Sound_FromJson())) {
            LOG_ERROR(L"Failed to Load Sounds");
            return E_FAIL;
        }
    }

    if (FAILED(ClientSettingManager::GetInstance()->Ready_Client_Prototypes(LEVEL::STATIC))) {
        LOG_ERROR(L"Failed to Ready Client Prototypes");
        return E_FAIL;
    }

    if (FAILED(ClientSettingManager::GetInstance()->Load_Navigation_FromBinary())) {
        LOG_ERROR(L"Failed to Ready Load_Navigation_FromBinary");
        return E_FAIL;
    }

    return S_OK;
}

Shared<Loader> Loader::Create(const ComPtr<ID3D11Device> &device, const ComPtr<ID3D11DeviceContext> &context, LEVEL nextLevelID, const Shared<Level>& ownerLevel, Bool IsLoadStatic)
{
    Shared<Loader> loader = make_shared<Loader>(device, context);

    LOADER_DESC desc{};
    desc.isLoadStatic = IsLoadStatic;
    desc.nextLevelID = nextLevelID;
    desc.ownerLevel = ownerLevel;

    if (FAILED(loader->Initialize(&desc))) {
		MSG_BOX("Failed to Created : Loader");
    }

    return loader;
}
