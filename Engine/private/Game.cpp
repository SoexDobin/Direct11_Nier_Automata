#include "LayerRegistry.h"
#include "TagRegistry.h"

#include "Game.h"
#include "GraphicDevice.h"
#include "LevelManager.h"
#include "ObjectManager.h"
#include "PrototypeManager.h"
#include "Renderer.h"
#include "TimeManager.h"

#include "Level.h"
#include "SpdLogger.h"
#include "Timer.h"

#include "Component.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(Game);

Game::~Game() {
  m_GraphicDevice.reset();

  m_LevelManager->On_Destroy();
  m_LevelManager.reset();

  m_ObjectManager->On_Destroy();
  m_ObjectManager.reset();

  m_Renderer->On_Destroy();
  m_Renderer.reset();

  m_PrototypeManager->On_Destroy();
  m_PrototypeManager.reset();

  m_TimeManager.reset();
  m_GraphicDevice.reset();
  m_LayerRegistry.reset();
  m_TagRegistry.reset();

  LOG_SHUTDOWN(); /* Debug Helper SpdLogger */
}

HRESULT Game::Initialize_Engine(const ENGINE_DESC &engineDesc) {
  LOG_INIT(); /* Debug Helper SpdLogger - 먼저 초기화 */

  m_LayerRegistry = make_shared<LayerRegistry>();
  m_TagRegistry = make_shared<TagRegistry>();

  if (nullptr == (m_GraphicDevice = GraphicDevice::Create(engineDesc)))
    return E_FAIL;

  if (nullptr == (m_TimeManager = TimeManager::Create()))
    return E_FAIL;

  if (nullptr == (m_LevelManager = LevelManager::Create()))
    return E_FAIL;

  if (nullptr ==
      (m_PrototypeManager = PrototypeManager::Create(engineDesc.levCount))) {
    return E_FAIL;
  }

  // RTTR 리플렉션을 통한 프로토타입 자동 등록
  if (FAILED(m_PrototypeManager->Create_Reflection(
          m_GraphicDevice->Get_Device(), m_GraphicDevice->Get_Context()))) {
    LOG_ERROR(L"Failed to Create RTTR Reflection in PrototypeManager");
  }

  if (nullptr == (m_ObjectManager = ObjectManager::Create())) {
    return E_FAIL;
  }

  if (nullptr ==
      (m_Renderer = Renderer::Create(m_GraphicDevice->Get_Device(),
                                     m_GraphicDevice->Get_Context())))
    return E_FAIL;

  return S_OK;
}

void Game::Update_Engine() {
  const Float delta = m_TimeManager->Update_Timers();

  m_ObjectManager->PriorityUpdate(delta);

  m_ObjectManager->Update(delta);

  m_ObjectManager->LateUpdate(delta);

  while (m_TimeManager->Is_FixedUpdate()) {
    Float fixedDelta = m_TimeManager->Get_MainTimer()->GetFixedDeltaTime();
    m_ObjectManager->FixedUpdate(fixedDelta);
    m_TimeManager->Get_MainTimer()->ConsumeFixedDeltaTime();
    m_TimeManager->Has_FixedUpdate();
  }

  m_ObjectManager->Cleanup_GameObjects();
}

HRESULT Game::Draw() {
  m_Renderer->Draw();

  return S_OK;
}

void Game::Clear_Resource(uint32 levIndex) {
  if (FAILED(m_PrototypeManager->Clear_Prototypes(levIndex))) {
    LOG_CRITICAL(L"Failed To Clear Level{} Prototypes", levIndex);
  }

  if (FAILED(m_ObjectManager->Clear_GameObjects())) {
    LOG_CRITICAL(L"Failed To Clear GameObjects");
  }
}

HRESULT Game::Clear_BackBufferView(const Shared<Float4> &clearColor) const {
  if (FAILED(m_GraphicDevice->Clear_BackBufferView(clearColor)))
    return E_FAIL;
  if (FAILED(m_GraphicDevice->Clear_DepthStencilView()))
    return E_FAIL;

  return S_OK;
}

HRESULT Game::Present() const { return m_GraphicDevice->Present(); }

HRESULT Game::OnResize(uint32 width, uint32 height) {
  if (nullptr == m_GraphicDevice)
    return S_OK;

  return m_GraphicDevice->OnResize(width, height);
}

HRESULT Game::Begin_RenderOffScreen(const wstring &rtTag) const {
  return m_GraphicDevice->Begin_RenderOffScreen(rtTag);
}

HRESULT Game::End_RenderOffScreen() const {
  return m_GraphicDevice->End_RenderOffScreen();
}

ComPtr<ID3D11ShaderResourceView>
Game::Get_OffScreenSRV(const wstring &rtTag) const {
  return m_GraphicDevice->Get_OffscreenSRV(rtTag);
}

HRESULT Game::Add_Timer(const wstring &timerTag) const {
  if (FAILED(m_TimeManager->Add_Timer(timerTag))) {
    MSG_BOX("Failed To Add Timer");
    return E_FAIL;
  }

  return S_OK;
}

Float Game::Compute_TimeDelta(const wstring &timerTag) const {
  return m_TimeManager->Get_Timer(timerTag)->GetDeltaTime();
}

HRESULT Game::Change_Level(uint32 levIndex, Unique<Level> newLevel) {
  if (FAILED(m_LevelManager->Change_Level(levIndex, std::move(newLevel)))) {
    MSG_BOX("Change To New Level Got a Trouble");
    return E_FAIL;
  }

  return S_OK;
}

HRESULT Game::Add_Prototype(uint32 levIndex,
                            const Shared<Object> &prototype) const {
  if (FAILED(m_PrototypeManager->Add_Prototype(levIndex, prototype))) {
    return E_FAIL;
  }

  return S_OK;
}

HRESULT Game::Add_GameObject(const Shared<GameObject> &gameObject) const {
  if (FAILED(m_ObjectManager->Add_GameObject(gameObject))) {
    LOG_ERROR(L"Failed To Add GameObject");
    return E_FAIL;
  }

  return S_OK;
}

void Game::Add_RenderGroup(RENDERGROUP group,
                           const Shared<GameObject> &gameObject) const {
  m_Renderer->Add_RenderGroup(group, gameObject);
}

Shared<Object> Game::Instantiate_Internal(PROTOTYPE prototype, uint32 levIndex,
                                          uint32 typeID, void *arg) const {
  auto prototypeInstance =
      m_PrototypeManager->Find_Prototype(prototype, levIndex, typeID);
  if (!prototypeInstance)
    return nullptr;

  if (prototypeInstance->Get_Prototype() == PROTOTYPE::GAMEOBJECT) {
    auto gameObject = static_pointer_cast<GameObject>(prototypeInstance);
    m_ObjectManager->Add_GameObject(gameObject->Clone(arg));

    return gameObject;
  } else if (prototypeInstance->Get_Prototype() == PROTOTYPE::COMPONENT) {
    return static_pointer_cast<Component>(prototypeInstance)->Clone(arg);
  }

  LOG_CRITICAL(L"Prototype Miss Match In Instantiate Internal");
  MSG_BOX("Prototype Miss Match In Instantiate Internal");
  return nullptr;
}

Shared<Object> Game::Instantiate_Internal(PROTOTYPE prototype, uint32 levIndex,
                                          const wstring &className,
                                          void *arg) const {
  auto prototypeInstance =
      m_PrototypeManager->Find_Prototype(prototype, levIndex, className);

  if (!prototypeInstance) {
    return nullptr;
  }

  if (prototypeInstance->Get_Prototype() == PROTOTYPE::GAMEOBJECT) {
    auto gameObject =
        static_pointer_cast<GameObject>(prototypeInstance)->Clone(arg);
    m_ObjectManager->Add_GameObject(gameObject);

    return gameObject;
  } else if (prototypeInstance->Get_Prototype() == PROTOTYPE::COMPONENT) {
    return static_pointer_cast<Component>(prototypeInstance)->Clone(arg);
  }

  LOG_CRITICAL(L"Prototype Miss Match In Instantiate Internal");
  MSG_BOX("Prototype Miss Match In Instantiate Internal");
  return nullptr;
}
