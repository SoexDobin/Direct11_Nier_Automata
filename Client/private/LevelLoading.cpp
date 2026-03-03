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

    if (true == m_Loader->Is_Finished()) {

        idididi = true;


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
