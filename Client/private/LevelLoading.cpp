#include "pch.h"
#include "LevelLoading.h"

#include "Game.h"
#include "Loader.h"
#include <SpdLogger.h>

#include "FreeCamera.h"
#include "LoadingBackground.h"
#include "Terrain.h"

LevelLoading::LevelLoading(const ComPtr<ID3D11Device> &device,
                           const ComPtr<ID3D11DeviceContext> &context)
    : Level{device, context}, m_Loader{nullptr}, m_NextLevel{LEVEL::LEVEL_END} {
}

HRESULT LevelLoading::Initialize(void *arg) {
  m_NextLevel = *static_cast<LEVEL *>(arg);

  m_Loader = Loader::Create(m_Device, m_Context, m_NextLevel);
  if (nullptr == m_Loader)
    return E_FAIL;

  return Level::Initialize(arg);
}

void LevelLoading::On_Destroy() { Level::On_Destroy(); }

void LevelLoading::Update_Level(Float timeDelta) {

    if (idididi) return;

    if (true == m_Loader->Is_Finished() && GetKeyState(VK_RETURN) & 0x8000) {

        idididi = true;

        LIGHT_DESC			LightDesc{};
        LightDesc.type = LIGHT::DIRECTIONAL;
        LightDesc.direction = Vector4(1.f, -1.f, 1.f, 0.f);
        LightDesc.diffuse = Vector4(1.f, 1.f, 1.f, 1.f);
        LightDesc.ambient = Vector4(1.f, 1.f, 1.f, 1.f);
        LightDesc.specular = Vector4(1.f, 1.f, 1.f, 1.f);

        if (FAILED(GAME_INSTANCE->Add_Light(LightDesc)))
            return;

        FreeCamera::FREE_CAMERA_DESC desc{};
        desc.mouseSensitive = 1.f;
        desc.eye = Vector4{ 0.f, 10.f, -10.f, 1.f };
        desc.at = Vector4{ 0.f, 0.f, 0.f, 1.f };
        desc.up = Vector4{ 0.f, 1.f, 0.f, 1.f };
        desc.fovY = XMConvertToRadians(60.f);
        desc.nearPlane = 0.1f;
        desc.farPlane = 500.f;
      
        GAME_INSTANCE->Instantiate<Terrain>();
    	GAME_INSTANCE->Instantiate<FreeCamera>(&desc);
//        GAME_INSTANCE->Instantiate<LoadingBackground>();

        if (4 <= ETOI(m_NextLevel)) {
        		MSG_BOX("Failed to Created : NextLevel");
              return;
        } else
          LOG_INFO(L"LoadEnd Next Level : {}", ETOI(m_NextLevel));
    }
    Level::Update_Level(timeDelta);
}

HRESULT LevelLoading::Render_Level() { return Level::Render_Level(); }

Unique<LevelLoading>
LevelLoading::Create(const ComPtr<ID3D11Device> &device,
                     const ComPtr<ID3D11DeviceContext> &context,
                     LEVEL nextLevelID) {
  auto loadingLevel = make_unique<LevelLoading>(device, context);

  if (FAILED(loadingLevel->Initialize(&nextLevelID))) {
    LOG_ERROR(L"Failed To Create LoadingBackground");
    return nullptr;
  }

  return loadingLevel;
}
