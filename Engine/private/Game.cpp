#include "LayerRegistry.h"
#include "TagRegistry.h"

#include "Game.h"

#include "Camera.h"
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
    m_TimeManager.reset();
    m_InputDevice.reset();

    m_ResourceManager->On_Destroy();
    m_ResourceManager.reset();

    m_LevelManager->On_Destroy();
    m_LevelManager.reset();

    m_ObjectManager->On_Destroy();
    m_ObjectManager.reset();

    m_Renderer->On_Destroy();
    m_Renderer.reset();

    m_PrototypeManager->On_Destroy();
    m_PrototypeManager.reset();

    m_CameraManager->On_Destroy();
    m_CameraManager.reset();

    m_LayerRegistry.reset();
    m_TagRegistry.reset();

    m_LightManager->On_Destroy();
    m_LightManager.reset();

    m_GraphicDevice.reset();

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

  if (nullptr ==
      (m_InputDevice = InputDevice::Create(engineDesc.hWnd, engineDesc.hInst)))
    return E_FAIL;

  if (nullptr == (m_Pipeline = Pipeline::Create()))
    return E_FAIL;

  if (nullptr == (m_LevelManager = LevelManager::Create()))
    return E_FAIL;

  if (nullptr == (m_PrototypeManager = PrototypeManager::Create(engineDesc.levelCount)))
    return E_FAIL;

  if (nullptr == (m_ObjectManager = ObjectManager::Create()))
    return E_FAIL;

  if (nullptr == (m_ResourceManager = ResourceManager::Create(m_GraphicDevice->Get_Device(), m_GraphicDevice->Get_Context(), engineDesc.levelCount)))
      return E_FAIL;

  if (nullptr == (m_CameraManager = CameraManager::Create()))
    return E_FAIL;

  if (nullptr == (m_Renderer = Renderer::Create(m_GraphicDevice->Get_Device(),m_GraphicDevice->Get_Context())))
    return E_FAIL;

  if (nullptr == (m_LightManager = LightManager::Create()))
    return E_FAIL;

  return S_OK;
}

void Game::Update_Engine() {
  const Float delta = m_TimeManager->Update_Timers();

  m_InputDevice->Update();

  m_ObjectManager->PriorityUpdate(delta);

  m_ObjectManager->Update(delta);

  m_ObjectManager->LateUpdate(delta);

  while (m_TimeManager->Is_FixedUpdate()) {
    Float fixedDelta = m_TimeManager->Get_MainTimer()->GetFixedDeltaTime();
    m_ObjectManager->FixedUpdate(fixedDelta);
    m_TimeManager->Get_MainTimer()->ConsumeFixedDeltaTime();
    m_TimeManager->Has_FixedUpdate();
  }

  m_ObjectManager->Submit_RenderGroup();

  m_ObjectManager->Cleanup_GameObjects();
  m_CameraManager->Bind_MainCamera_Transform();
  m_Pipeline->Update_Pipeline();

  m_LevelManager->Update(delta);
}

HRESULT Game::Draw() const {
    m_Renderer->Draw();
    return S_OK;
}
HRESULT Game::Draw_NoClearing() const {
    m_Renderer->Draw_NoClearing();
    return S_OK;
}

void Game::Clear_AllResource() const {
    m_PrototypeManager->Clear_Prototypes();
    m_ObjectManager->Clear_GameObjects();
    m_Renderer->Clear_RenderGroup();
    m_CameraManager->Clear_Cameras();
    m_LightManager->Clear_Lights();
    m_ResourceManager->Clear_AllResources();
}

void Game::Clear_Resource(uint32 levIndex) const {
  if (FAILED(m_PrototypeManager->Clear_Prototypes(levIndex))) {
    LOG_CRITICAL(L"Failed To Clear Level{} Prototypes", levIndex);
  }

  if (FAILED(m_ObjectManager->Clear_GameObjects())) {
    LOG_CRITICAL(L"Failed To Clear GameObjects");
  }

  if (FAILED(m_ResourceManager->Clear_Resource(levIndex))) {
      LOG_CRITICAL(L"Failed To Clear Resources");
  }

  if (FAILED(m_Renderer->Clear_RenderGroup())) {
    LOG_CRITICAL(L"Failed To Clear RenderGroup");
  }

  if (FAILED(m_CameraManager->Clear_Cameras())) {
    LOG_CRITICAL(L"Failed To Clear Cameras");
  }

  if (FAILED(m_LightManager->Clear_Lights())) {
    LOG_CRITICAL(L"Failed To Clear Lights");
  }
}

void Game::Update_Input() const { m_InputDevice->Update(); }

HRESULT Game::Clear_BackBufferView(const Shared<Float4> &clearColor) const {
  if (FAILED(m_GraphicDevice->Clear_BackBufferView(clearColor)))
    return E_FAIL;
  if (FAILED(m_GraphicDevice->Clear_DepthStencilView()))
    return E_FAIL;

  return S_OK;
}

HRESULT Game::Present() const { return m_GraphicDevice->Present(); }

HRESULT Game::OnResize(uint32 width, uint32 height, uint32 offscreenIndex) {
  if (nullptr == m_GraphicDevice)
    return S_OK;

  m_CameraManager->Get_MainCamera()->Bind_Aspect(width / height);
  for (auto camera : m_CameraManager->Get_Cameras())
      camera->Set_Aspect(width / height);

  return m_GraphicDevice->OnResize(width, height, offscreenIndex);
}

HRESULT Game::Begin_RenderOffScreen(uint32 screenIndex) const {
  return m_GraphicDevice->Begin_RenderOffScreen(screenIndex);
}
HRESULT Game::End_RenderOffScreen() const {
  return m_GraphicDevice->End_RenderOffScreen();
}
ComPtr<ID3D11ShaderResourceView>
Game::Get_OffScreenSRV(uint32 screenIndex) const {
  return m_GraphicDevice->Get_OffscreenSRV(screenIndex);
}

Byte Game::Get_DIKeyState(uByte byKeyID) const {
  return m_InputDevice->Get_DIKeyState(byKeyID);
}

Byte Game::Get_DIMouseState(DIMB mouseInput) const {
  return m_InputDevice->Get_DIMouseState(mouseInput);
}

Long Game::Get_DIMouseMove(DIMM mouseState) const {
  return m_InputDevice->Get_DIMouseMove(mouseState);
}

HRESULT Game::Add_Timer(const wstring &timerTag) const {
  if (FAILED(m_TimeManager->Add_Timer(timerTag))) {
    MSG_BOX("Failed To Add Timer");
    return E_FAIL;
  }

  return S_OK;
}

void Game::Set_TimeScale(Float timeScale) const {
  return m_TimeManager->Get_MainTimer()->SetTimeScale(timeScale);
}

Float Game::Get_FPS() const { return m_TimeManager->Get_MainTimer()->GetFPS(); }
Float Game::Compute_TimeDelta() const {
  return m_TimeManager->Get_MainTimer()->GetDeltaTime();
}
Float Game::Compute_UnscaledTimeDelta() const {
  return m_TimeManager->Get_MainTimer()->GetUnscaledDeltaTime();
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

uint32 Game::Get_ObjectIDFromPrototypeTag(const wstring& prototypeTag, uint32 levIndex) const
{
    return m_PrototypeManager->Get_ObjectIDFromPrototypeTag(prototypeTag, levIndex);
}

const tChar* Game::Get_PrototypeTagFromObjectID(uint32 objectID, uint32 levIndex) const
{
    return m_PrototypeManager->Get_PrototypeTagFromObjectID(objectID, levIndex);
}

HRESULT Game::Add_GameObject(const Shared<GameObject> &gameObject) const {
  if (FAILED(m_ObjectManager->Add_GameObject(gameObject))) {
    LOG_ERROR(L"Failed To Add GameObject");
    return E_FAIL;
  }

  return S_OK;
}

void Game::Submit_RenderGroup() const { m_ObjectManager->Submit_RenderGroup(); }

const unordered_map<uint32, Shared<GameObject>>& Game::Get_GameObjects() const {
    return m_ObjectManager->Get_GameObjects();
}

HRESULT Game::Add_Camera(const Shared<Camera> &camera) const { return m_CameraManager->Add_Camera(camera); }
HRESULT Game::Set_MainCamera(const Shared<Camera> &camera) const { return m_CameraManager->Set_MainCamera(camera); }
Shared<Camera> Game::Get_MainCamera() const { return m_CameraManager->Get_MainCamera(); }

HRESULT Game::Load_Shader(uint32 levIndex, const tChar* shaderFilePath, const D3D11_INPUT_ELEMENT_DESC* elements, uint32 numElements, const wstring& descriptionTag) const
{
    return m_ResourceManager->Load_Shader(levIndex, shaderFilePath, elements, numElements, descriptionTag);
}

Shared<Shader> Game::Get_Shader(uint32 levIndex, const tChar* shaderFilePath) const
{
    return m_ResourceManager->Get_Shader(levIndex, shaderFilePath);
}

HRESULT Game::Load_Texture(uint32 levIndex, const tChar* textureFilePath, uint32 numSRVs, const wstring& descriptionTag) const
{
    return m_ResourceManager->Load_Texture(levIndex, textureFilePath, numSRVs, descriptionTag);
}

const Texture::TEXTURE_DESC& Game::Get_TextureDesc(uint32 levIndex, const wstring& descriptionTag) const
{
    return m_ResourceManager->Get_TextureDescByTag(levIndex, descriptionTag);
}

const ComPtr<ID3D11ShaderResourceView>& Game::Get_Texture(uint32 levIndex, const tChar* textureFilePath) const
{
    return m_ResourceManager->Get_Texture(levIndex, textureFilePath);
}

void Game::Add_RenderGroup(RENDERGROUP group, const Shared<GameObject> &gameObject) const {
	m_Renderer->Add_RenderGroup(group, gameObject);
}

HRESULT Game::Bind_CameraPosition(const Shared<Shader> &shader,
                                  const Char *constantName) const {
	return m_Pipeline->Bind_CameraPosition(shader, constantName);
}

HRESULT Game::Bind_TransformMatrix(const Shared<Shader> &shader,
                                   const Char *constantName,
                                   D3DTS transformState) {
	return m_Pipeline->Bind_TransformMatrix(shader, constantName, transformState);
}

HRESULT Game::Bind_TransformMatrix_Inverse(const Shared<Shader> &shader,
                                           const Char *constantName,
                                           D3DTS transformState) {
	return m_Pipeline->Bind_TransformMatrix_Inverse(shader, constantName, transformState);
}

Matrix Game::Get_Transform(D3DTS transformState) const {
	return m_Pipeline->Get_Transform(transformState);
}

Matrix Game::Get_InvTransform(D3DTS transformState) const {
    return m_Pipeline->Get_InvTransform(transformState);
}

Vector4 Game::Get_CamTransform() const {
	return m_Pipeline->Get_CamTransform();
}

void Game::Set_Transform(D3DTS transformState, Matrix transformStateMatrix) {
	m_Pipeline->Set_Transform(transformState, transformStateMatrix);
}

void Game::Update_Pipeline() const { m_Pipeline->Update_Pipeline(); }

const LIGHT_DESC *Game::Get_LightDesc(uint32 index) const {
  return m_LightManager->Get_LightDesc(index);
}
HRESULT Game::Add_Light(const LIGHT_DESC &lightDesc) const {
  return m_LightManager->Add_Light(lightDesc);
}
HRESULT Game::Remove_Light(uint32 index) const {
  return m_LightManager->Remove_Light(index);
}

Shared<const Object> Game::Find_Prototype(PROTOTYPE prototype, uint32 objectID, uint32 levIndex) const {
    uint32 level = levIndex == MAXINT32 ? m_LevelManager->Get_CurrentLevelIndex() : levIndex;

    if (Shared<Object> object = m_PrototypeManager->Find_Prototype(prototype, level, objectID)) {
		return static_pointer_cast<Object>(object);
    }

    return nullptr;
}

HRESULT Game::Add_Prototype_Internal(uint32 levIndex, const Shared<Object>& object, const wstring& prototypeTag) const 
{
    return m_PrototypeManager->Add_Prototype(levIndex, object, prototypeTag);
}
Shared<Object> Game::Instantiate_Internal(PROTOTYPE protoType, uint32 objectID, uint32 levIndex, void* arg) const
{
    uint32 level = (levIndex == UINT_MAX) ? m_LevelManager->Get_CurrentLevelIndex() : levIndex;
    
    Shared<Object> pPrototype = m_PrototypeManager->Find_Prototype(protoType, level, objectID);
    if (!pPrototype) return nullptr;
    Shared<Object> pCloned = nullptr;
    if (protoType == PROTOTYPE::GAMEOBJECT) {
        auto pGameObject = std::static_pointer_cast<GameObject>(pPrototype);
        pCloned = pGameObject->Clone(arg);
        m_ObjectManager->Add_GameObject(std::static_pointer_cast<GameObject>(pCloned));
    }
    else {
        auto pComponent = std::static_pointer_cast<Component>(pPrototype);
        pCloned = pComponent->Clone(arg);
    }
    return pCloned;
}

Shared<Object> Game::Instantiate_Internal(PROTOTYPE protoType, const wstring& prototypeTag, uint32 levIndex, void* arg) const
{
    uint32 level = (levIndex == UINT_MAX) ? m_LevelManager->Get_CurrentLevelIndex() : levIndex;

    uint32 objectID = m_PrototypeManager->Get_ObjectIDFromPrototypeTag(prototypeTag, level);
    if (objectID == 0)
    {
        MSG_BOX("Failed to Instantiate By prototypeTag");
        return nullptr;
    }

    return Instantiate_Internal(protoType, objectID, level, arg);
}
