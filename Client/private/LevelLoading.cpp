#include "pch.h"
#include "LevelLoading.h"

#include "Game.h"
#include "Loader.h"
#include <SpdLogger.h>

LevelLoading::LevelLoading(const ComPtr<ID3D11Device> &device,
                           const ComPtr<ID3D11DeviceContext> &context)
    : Level{device, context}, m_Loader{nullptr}, m_NextLevel{LEVEL::LEVEL_END} {
}

HRESULT LevelLoading::Initialize(void *arg) {
    m_NextLevel = *static_cast<LEVEL*>(arg);

    m_Loader = Loader::Create(m_Device, m_Context, m_NextLevel, shared_from_this());
    if (nullptr == m_Loader)
      return E_FAIL;

    m_IsFinished = m_Loader->Is_Finished();
    return Level::Initialize(arg);
}

void LevelLoading::On_Destroy() { Level::On_Destroy(); }

void LevelLoading::Update_Level(Float timeDelta) {
    m_IsFinished = m_Loader->Is_Finished();

    if (true == m_IsFinished) {
        if (4 <= ETOI(m_NextLevel)) {
        		MSG_BOX("Failed to Created : NextLevel");
              return;
        } else
          LOG_INFO(L"LoadEnd Next Level : {}", ETOI(m_NextLevel));
    }
    Level::Update_Level(timeDelta);
}

HRESULT LevelLoading::Render_Level() { return Level::Render_Level(); }

Shared<LevelLoading> LevelLoading::Create(const ComPtr<ID3D11Device> &device,
                     const ComPtr<ID3D11DeviceContext> &context,
                     LEVEL nextLevelID) {
  auto loadingLevel = make_shared<LevelLoading>(device, context);

  if (FAILED(loadingLevel->Initialize(&nextLevelID))) {
    LOG_ERROR(L"Failed To Create LoadingBackground");
    return nullptr;
  }

  return loadingLevel;
}
