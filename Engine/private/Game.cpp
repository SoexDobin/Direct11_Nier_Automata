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
#include "LevelSerializer.h"
#include "PrefabManager.h"
#include "Timer.h"
#include "TypeCatalog.h"

IMPLEMENT_SINGLETON(Game);

Game::Game() = default;

Game::~Game() {
	m_PrefabManager.reset();
	m_LevelSerializer.reset();

    m_TimeManager.reset();
    m_InputDevice.reset();

    m_CollisionManager->On_Destroy();
    m_CollisionManager.reset();

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

	if (m_Registry)
		m_Registry->On_Destroy();
	m_Registry.reset();

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
	m_Registry = make_unique<Registry>();
	if (FAILED(m_Registry->Initialize(nullptr)))
		return E_FAIL;

    if (nullptr == (m_GraphicDevice = GraphicDevice::Create(engineDesc)))
		return E_FAIL;

    if (nullptr == (m_TimeManager = TimeManager::Create()))
		return E_FAIL;

    if (nullptr == (m_InputDevice = InputDevice::Create(engineDesc.hWnd, engineDesc.hInst)))
		return E_FAIL;

    if (nullptr == (m_Pipeline = Pipeline::Create()))
		return E_FAIL;

    if (nullptr == (m_LevelManager = LevelManager::Create()))
		return E_FAIL;

    if (nullptr == (m_PrototypeManager = PrototypeManager::Create(engineDesc.levelCount)))
		return E_FAIL;

    if (nullptr == (m_ObjectManager = ObjectManager::Create(engineDesc.levelCount)))
		return E_FAIL;

    if (nullptr == (m_ResourceManager = ResourceManager::Create(m_GraphicDevice->Get_Device(), m_GraphicDevice->Get_Context(), engineDesc.levelCount)))
        return E_FAIL;

    if (nullptr == (m_CameraManager = CameraManager::Create(engineDesc.levelCount)))
		return E_FAIL;

    if (nullptr == (m_RenderTargetManager = RenderTargetManager::Create(m_GraphicDevice->Get_Device(), m_GraphicDevice->Get_Context())))
        return E_FAIL;

    if (nullptr == (m_Renderer = Renderer::Create(m_GraphicDevice->Get_Device(),m_GraphicDevice->Get_Context())))
		return E_FAIL;

    if (nullptr == (m_LightManager = LightManager::Create()))
		return E_FAIL;

    if (nullptr == (m_LevelSerializer = LevelSerializer::Create()))
        return E_FAIL;
	m_PrefabManager = make_unique<PrefabManager>();

    if (nullptr == (m_FontManager = FontManager::Create(m_GraphicDevice->Get_Device(), m_GraphicDevice->Get_Context())))
        return E_FAIL;

    if (nullptr == (m_SoundManager = SoundManager::Create()))
        return E_FAIL;

    if (nullptr == (m_EventManager = EventManager::Create(engineDesc.levelCount)))
        return E_FAIL;

    if (nullptr == (m_CollisionManager = CollisionManager::Create()))
        return E_FAIL;

    if (nullptr == (m_NavigationBuilder = NavigationBuilder::Create()))
        return E_FAIL;


    return S_OK;
}

void Game::Update_Engine() {
    const Float delta = m_TimeManager->Update_Timers();

    m_InputDevice->Update();

    m_ObjectManager->PriorityUpdate(delta);

    m_ObjectManager->Update(delta);

    m_ObjectManager->LateUpdate(delta);

    m_CameraManager->Bind_MainCamera_Transform();
    m_Pipeline->Update_Pipeline();


    while (m_TimeManager->Is_FixedUpdate()) {
        Float fixedDelta = m_TimeManager->Get_MainTimer()->GetFixedDeltaTime();
        m_ObjectManager->FixedUpdate(fixedDelta);
        m_TimeManager->Get_MainTimer()->ConsumeFixedDeltaTime();
        m_TimeManager->Has_FixedUpdate();
    }

    m_ObjectManager->Submit_RenderGroup();

    m_ObjectManager->Cleanup_GameObjects(0);
    m_ObjectManager->Cleanup_GameObjects(GAME_INSTANCE->Get_CurrentLevelIndex());

    m_CollisionManager->Update_Collision();

    m_LevelManager->Update(delta);
    m_EventManager->Execute_Events();
}

HRESULT Game::Draw() const {
    m_Renderer->Draw();
    return S_OK;
}
HRESULT Game::Draw_NoClearing() const {
    m_Renderer->Draw_NoClearing();
    return S_OK;
}

void Game::Update_CameraPipeline()
{
    m_CameraManager->Bind_MainCamera_Transform();
    m_Pipeline->Update_Pipeline();
}

void Game::Clear_AllResource() const {
    m_PrototypeManager->Clear_Prototypes();
    m_ObjectManager->Clear_AllGameObjects();
    m_Renderer->Clear_RenderGroup();
    m_CameraManager->Clear_AllCameras();
    m_LightManager->Clear_Lights();
    m_ResourceManager->Clear_AllResources();
    m_LevelManager->Clear_LevelMembers();

    m_FontManager->Clear_Fonts();
    m_SoundManager->Clear_SoundSources();
    m_EventManager->Clear_AllEvents();
}

void Game::Clear_Resource(uint32 levIndex) const {

	if (FAILED(m_PrototypeManager->Clear_Prototypes(levIndex)))
		LOG_CRITICAL(L"Failed To Clear Level{} Prototypes", levIndex);
  
    if (FAILED(m_ObjectManager->Clear_GameObjects(levIndex)))
        LOG_CRITICAL(L"Failed To Clear Level{} GameObjects", levIndex);

	if (FAILED(m_ResourceManager->Clear_Resource(levIndex)))
		LOG_CRITICAL(L"Failed To Clear Resources");

	if (FAILED(m_Renderer->Clear_RenderGroup()))
      LOG_CRITICAL(L"Failed To Clear RenderGroup");

	if (FAILED(m_CameraManager->Clear_Cameras(levIndex)))
      LOG_CRITICAL(L"Failed To Clear Cameras");

	if (FAILED(m_LightManager->Clear_Lights())) 
		LOG_CRITICAL(L"Failed To Clear Lights");
	
    if (FAILED(m_EventManager->Clear_Events(levIndex)))
        LOG_CRITICAL(L"Failed To Clear Events");

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

HRESULT Game::OnResize(uint32 width, uint32 height, uint32 offscreenIndex)
{
	if (nullptr == m_GraphicDevice)
	{
		return S_OK;
	}

	Float fAspect = 1.6f;
	if (height > 0)
	{
		fAspect = static_cast<Float>(width) / height;
	}

	if (nullptr != m_CameraManager->Get_MainCamera())
	{
		m_CameraManager->Get_MainCamera()->Bind_Aspect(fAspect);
	}

	for (auto &camera : m_CameraManager->Get_Cameras(GAME_INSTANCE->Get_CurrentLevelIndex()))
	{
		camera->Set_Aspect(fAspect);
	}

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

void Game::Set_DepthStencilState(ID3D11DepthStencilState* state, UINT ref) const
{
    m_GraphicDevice->Set_DepthStencilState(state, ref);
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

Shared<Level> Game::Get_CurrentLevel()
{
    return m_LevelManager->Get_CurrentLevel();
}

void Game::Update_Level() const
{
    m_LevelManager->Update(0.f);
}

HRESULT Game::Change_Level(uint32 levIndex, const Shared<Level>& newLevel) {
  if (FAILED(m_LevelManager->Change_Level(levIndex, newLevel))) {
    MSG_BOX("Change To New Level Got a Trouble");
    return E_FAIL;
  }

  return S_OK;
}

const unordered_map<uint32, Shared<GameObject>>& Game::Get_Prototypes(uint32 levIndex) const
{
    return m_PrototypeManager->Get_GameObjects()[levIndex];
}

uint32 Game::Get_ObjectIDFromPrototypeTag(const wstring& prototypeTag, uint32 levIndex) const
{
    return m_PrototypeManager->Get_ObjectIDFromPrototypeTag(prototypeTag, levIndex);
}

wstring Game::Get_PrototypeTagFromObjectID(uint32 objectID, uint32 levIndex) const
{
    return m_PrototypeManager->Get_PrototypeTagFromObjectID(objectID, levIndex);
}

HRESULT Game::Add_GameObject(const Shared<GameObject> &gameObject, uint32 levIndex) const {
  if (FAILED(m_ObjectManager->Add_GameObject(levIndex, gameObject))) {
    LOG_ERROR(L"Failed To Add GameObject");
    return E_FAIL;
  }

  return S_OK;
}

void Game::Clearing_ObjectManager(uint32 levIndex) const
{
    m_ObjectManager->Cleanup_GameObjects(levIndex);
}

Shared<GameObject> Game::Find_ByInstanceID(uint32 levIndex, uint32 instanceID) const
{
    Shared<GameObject> pObj = m_ObjectManager->Find_ByInstanceID(levIndex, instanceID);
    if (!pObj && levIndex != 0)
    {
        pObj = m_ObjectManager->Find_ByInstanceID(0, instanceID);
    }
    return pObj;
}

Shared<GameObject> Game::Find_ObjectByObjectID(uint32 levIndex, uint32 objectID) const
{
    Shared<GameObject> pObj = m_ObjectManager->Find_ObjectByObjectID(levIndex, objectID);
    if (!pObj && levIndex != 0)
    {
        pObj = m_ObjectManager->Find_ObjectByObjectID(0, objectID);
    }
    return pObj;
}

Shared<GameObject> Game::Find_ObjectByObjectTag(uint32 levIndex, const wstring& tag) const
{
    return m_ObjectManager->Find_ObjectByObjectTag(levIndex, tag);
}

Shared<GameObject> Game::Find(ObjectGuid objectGuid) const
{
    return m_ObjectManager->Find_ByObjectGuid(objectGuid);
}

Shared<GameObject> Game::Find(RuntimeObjectId runtimeObjectId) const
{
    return m_ObjectManager->Find_ByRuntimeObjectId(runtimeObjectId);
}

HRESULT Game::Destroy(ObjectGuid objectGuid) const
{
    return m_ObjectManager->Destroy(objectGuid);
}

vector<Shared<GameObject>> Game::FindAll_Internal(RuntimeTypeId runtimeTypeId) const
{
    return m_ObjectManager->Find_AllByRuntimeTypeId(runtimeTypeId);
}

void Game::Submit_RenderGroup() const { m_ObjectManager->Submit_RenderGroup(); }

const unordered_map<uint32, Shared<GameObject>>& Game::Get_GameObjects(uint32 levIndex) const {
    return m_ObjectManager->Get_GameObjects(levIndex);
}

HRESULT Game::Clear_AllGameObjects() const
{
	if (FAILED(m_ObjectManager->Clear_AllGameObjects()))
	{
		LOG_ERROR(L"[Game] Failed To Clear All GameObjects");
		return E_FAIL;
	}
	if (FAILED(m_Renderer->Clear_RenderGroup()))
	{
		LOG_ERROR(L"[Game] Failed To Clear RenderGroup");
		return E_FAIL;
	}
	m_CameraManager->Clear_AllCameras();
	return S_OK;
}

HRESULT Game::Add_Camera(uint32 levIndex, const Shared<Camera> &camera) const
{
	return m_CameraManager->Add_Camera(levIndex, camera);
}
vector<Shared<Camera>> Game::Get_Cameras(uint32 levIndex) const
{
    return m_CameraManager->Get_Cameras(levIndex);
}
HRESULT Game::Set_MainCamera(const Shared<Camera> &camera) const
{
	return m_CameraManager->Set_MainCamera(camera);
}
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

const Texture::TEXTURE_DESC* Game::Get_TextureDesc(uint32 levIndex, const wstring& descriptionTag) const
{
    return m_ResourceManager->Get_TextureDescByTag(levIndex, descriptionTag);
}

vector<wstring> Game::Get_TextureTags(uint32 levIndex) const
{
    return m_ResourceManager->Get_TextureTags(levIndex);
}

const ComPtr<ID3D11ShaderResourceView>& Game::Get_Texture(uint32 levIndex, const tChar* textureFilePath) const
{
    return m_ResourceManager->Get_Texture(levIndex, textureFilePath);
}

HRESULT Game::Load_Model(uint32 levIndex, const tChar* modelFilePath, const wstring& descriptionTag, const Matrix& preTransformMatrix) const
{
    return m_ResourceManager->Load_Model(levIndex, modelFilePath, descriptionTag, preTransformMatrix);
}

HRESULT Game::Load_ModelAnimations(uint32 levIndex, const wstring& modelTag,
	const vector<wstring>& animationFilePaths) const
{
	return m_ResourceManager->Load_ModelAnimations(levIndex, modelTag, animationFilePaths);
}

Shared<Model> Game::Get_Model(uint32 levIndex, const tChar* modelFilePath) const
{
    return m_ResourceManager->Get_Model(levIndex, modelFilePath);
}

int32 Game::Get_ContainLevelByModelTag(const wstring& modelTag) const
{
    return m_ResourceManager->Get_ContainLevelByModelTag(modelTag);
}

vector<Shared<Model>> Game::Get_Models(uint32 levIndex) const
{
    return m_ResourceManager->Get_Models(levIndex);
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
HRESULT Game::Render_Lights(const Shared<class Shader>& shader, const Shared<class VIBuffer_Rect>& buffer) const
{
    return m_LightManager->Render_Lights(shader, buffer);
}

Shared<const Object> Game::Find_Prototype(PROTOTYPE prototype, uint32 objectID, uint32 levIndex) const {
    uint32 level = levIndex == MAXINT32 ? m_LevelManager->Get_CurrentLevelIndex() : levIndex;

    if (Shared<Object> object = m_PrototypeManager->Find_Prototype(prototype, level, objectID)) {
		return static_pointer_cast<Object>(object);
    }

    return nullptr;
}

HRESULT Game::Refresh_ReflectionRegistry() const
{
    return m_Registry ? m_Registry->Refresh() : E_FAIL;
}

HRESULT Game::Register_ReflectedPrototypes(uint32 levIndex) const
{
    if (!m_Registry || !m_PrototypeManager)
        return E_FAIL;

    const rttr::type gameObjectType = rttr::type::get<GameObject>();
    const rttr::type componentType = rttr::type::get<Component>();
    for (const Registry::Entry& entry : m_Registry->Get_ObjectTypes()) {
        const Bool isGameObject = entry.reflectedType.is_derived_from(gameObjectType);
        const Bool isComponent = entry.reflectedType.is_derived_from(componentType);
        if (!isGameObject && !isComponent)
            continue;

        const rttr::method createMethod = entry.reflectedType.get_method("Create");
        if (!createMethod.is_valid())
            continue;

        const rttr::variant levelMetadata = createMethod.get_metadata("Level");
        if (!levelMetadata.is_valid())
            continue;

        rttr::variant convertedLevel = levelMetadata;
        if (!convertedLevel.convert(rttr::type::get<uint32>())) {
            LOG_ERROR(L"Invalid Level metadata on reflected type {}",
                      Helper::To_wString(entry.registeredName));
            return E_FAIL;
        }
        if (convertedLevel.get_value<uint32>() != levIndex)
            continue;

        const rttr::variant result = createMethod.invoke(
            {}, m_GraphicDevice->Get_Device(), m_GraphicDevice->Get_Context());
        if (!result.is_valid()) {
            LOG_ERROR(L"Failed to invoke reflected Create for {}",
                      Helper::To_wString(entry.registeredName));
            return E_FAIL;
        }

        Shared<Object> prototype;
        if (isGameObject && result.is_type<Shared<GameObject>>())
            prototype = result.get_value<Shared<GameObject>>();
        else if (isComponent && result.is_type<Shared<Component>>())
            prototype = result.get_value<Shared<Component>>();

        const HRESULT addResult = prototype
            ? Add_Prototype_Internal(levIndex, prototype, Helper::To_wString(entry.registeredName))
            : E_FAIL;
        if (FAILED(addResult)) {
            LOG_ERROR(L"Failed to auto-register reflected prototype {}",
                      Helper::To_wString(entry.registeredName));
            return E_FAIL;
        }
    }

    return S_OK;
}

string Game::Find_RegisteredName(RuntimeTypeId runtimeTypeId) const
{
    return m_Registry ? m_Registry->Find_RegisteredName(runtimeTypeId) : string{};
}

RuntimeTypeId Game::Find_RuntimeTypeId(std::string_view registeredName) const
{
    return m_Registry ? m_Registry->Find_RuntimeTypeId(registeredName) : RuntimeTypeId{};
}

HRESULT Game::Add_Prototype_Internal(uint32 levIndex, const Shared<Object>& object, const wstring& prototypeTag) const 
{
    return m_PrototypeManager->Add_Prototype(levIndex, object, prototypeTag);
}
HRESULT Game::SerializeLevel(uint32 levIndex, const wstring &path) const
{
    return m_LevelSerializer->SerializeLevel(levIndex, path);
}

HRESULT Game::DeSerializeLevel(const wstring &path) const
{
    return m_LevelSerializer->DeSerializeLevel(path);
}

HRESULT Game::Register_Prefab(PrefabGuid prefabGuid, const wstring& path) const
{
	return m_PrefabManager->Register_Prefab(prefabGuid, path);
}

HRESULT Game::Unregister_Prefab(PrefabGuid prefabGuid) const
{
	return m_PrefabManager->Unregister_Prefab(prefabGuid);
}

wstring Game::Find_PrefabPath(PrefabGuid prefabGuid) const
{
	return m_PrefabManager->Find_PrefabPath(prefabGuid);
}

HRESULT Game::SerializePrefabDocument(PrefabGuid prefabGuid, uint32 levIndex) const
{
	return m_PrefabManager->SerializePrefabDocument(prefabGuid, levIndex);
}

HRESULT Game::DeSerializePrefabDocument(PrefabGuid prefabGuid) const
{
	return m_PrefabManager->DeSerializePrefabDocument(prefabGuid);
}

ObjectGuid Game::Consume_RestoredObjectGuid() const
{
	return m_PrefabManager->Consume_RestoredObjectGuid();
}

HRESULT Game::Add_Font(const wstring& fontTag, const tChar* fontFilePath)
{
    return m_FontManager->Add_Font(fontTag, fontFilePath);
}

void Game::Draw_Font(const wstring& fontTag, const tChar* text, const Vector2& position, const Color& color)
{
    m_FontManager->Draw(fontTag, text, position, color);
}

HRESULT Game::LoadSoundFile(const wstring& path) const
{
    return m_SoundManager->LoadSoundFile(path);
}

HRESULT Game::Load_Sound(const wstring& soundTag, const wstring& soundFilePath) const
{
    return m_SoundManager->Load_Sound(soundTag, soundFilePath);
}

void Game::PlaySoundFX(const wstring& soundKey, SOUNDCHANNEL id, Float volume) const
{
    m_SoundManager->PlaySoundFx(soundKey, id, volume);
}

void Game::PlaySoundFXOnce(const wstring& soundKey, SOUNDCHANNEL id, Float volume) const
{
    m_SoundManager->PlaySoundFxOnce(soundKey, id, volume);
}

void Game::PlaySoundLoopSection(const wstring& soundKey, SOUNDCHANNEL id, Float volume, uint32 loopStartMs, uint32 loopEndMs, Bool playIntro) const
{
    m_SoundManager->PlaySoundLoopSection(soundKey, id, volume, loopStartMs, loopEndMs, playIntro);
}

void Game::StopSound(SOUNDCHANNEL targetChannel) const
{
    if (targetChannel == SOUNDCHANNEL::MAX_CHANNELS)
        m_SoundManager->StopAll();
    else
        m_SoundManager->StopChannel(targetChannel);
}

HRESULT Game::Add_Instance_Event(uint32 levIndex, const wstring& eventTag, const std::function<void()>& callback) const
{
    return m_EventManager->Add_EventOnce(levIndex, eventTag, callback);
}

HRESULT Game::Add_Permanent_Event(uint32 levIndex, const wstring& eventTag, const std::function<void()>& callback) const
{
    return m_EventManager->Add_EventPermanent(levIndex, eventTag, callback);
}

HRESULT Game::Remove_Event(uint32 levIndex, const wstring& eventTag) const
{
    return m_EventManager->Remove_Event(levIndex, eventTag);
}

void Game::Add_Collider(const Shared<class Collider>& collider) const
{
    m_CollisionManager->Add_Collider(collider);
}

void Game::Remove_Collider(const Shared<class Collider>& collider) const
{
    m_CollisionManager->Remove_Collider(collider);
}

void Game::Update_Collision() const
{
    m_CollisionManager->Update_Collision();
}

NavigationBuilder::NAV_BUILD_RESULT Game::Build_Navigation(const Float* vertices, int32 numVertices,
	const int32* triangles, int32 numTriangles, const NavigationBuilder::NAV_BUILD_PARAMS_DESC& params)
{
    return m_NavigationBuilder->Build(vertices, numVertices, triangles, numTriangles, params);
}

vector<NavCellBinary> Game::Bake_Navigation(const Shared<Model>& model, const Matrix& worldMatrix, const rcConfig& config) const
{
    return m_NavigationBuilder->Bake_Navigation(model, worldMatrix, config);
}

HRESULT Game::Export_Navigation(const string& fileName, const Shared<Model>& model, const Matrix& worldMatrix, const rcConfig& config) const
{
    return m_NavigationBuilder->Export_Binary(fileName, model, worldMatrix, config);
}

vector<NavCell> Game::Import_Navigation(const string& filePath) const
{
    return m_NavigationBuilder->Import_Binary(filePath);
}


HRESULT Game::Add_RenderTarget(const wstring& renderTargetTag, uint32 sizeX, uint32 sizeY, DXGI_FORMAT pixelFormat, const Color& color) const
{
    return m_RenderTargetManager->Add_RenderTarget(renderTargetTag, sizeX, sizeY, pixelFormat, color);
}

HRESULT Game::Add_MultiRenderTarget(const wstring& multiRenderTargetTag, const wstring& renderTargetTag) const
{
    return m_RenderTargetManager->Add_MultiRenderTarget(multiRenderTargetTag, renderTargetTag);
}

HRESULT Game::Begin_MultiRenderTarget(const wstring& multiRenderTargetTag) const
{
    return m_RenderTargetManager->Begin_MultiRenderTarget(multiRenderTargetTag);
}

HRESULT Game::End_MultiRenderTarget() const
{
    return m_RenderTargetManager->End_MultiRenderTarget();
}

HRESULT Game::Bind_RenderTarget_ShaderResource(const Shared<Shader>& shader, const Char* constantName, const wstring& renderTargetTag) const
{
    return m_RenderTargetManager->Bind_ShaderResource(shader, constantName, renderTargetTag);
}


Shared<Object> Game::Instantiate_Internal(PROTOTYPE protoType, uint32 objectID, uint32 levIndex,
                                          void* arg, ObjectGuid objectGuid) const
{
    Shared<Object> protoObject = nullptr;
	uint32 targetLevel = (levIndex == UINT_MAX) ? m_LevelManager->Get_CurrentLevelIndex() : levIndex;

	// 1. Search in target level
	protoObject = m_PrototypeManager->Find_Prototype(protoType, targetLevel, objectID);
    
	// 2. Search in Static level (0)
	if (!protoObject && targetLevel != 0)
        protoObject = m_PrototypeManager->Find_Prototype(protoType, 0, objectID);

    // 3. Search in Current level (if targetLevel was something else)
    if (!protoObject && targetLevel != m_LevelManager->Get_CurrentLevelIndex())
        protoObject = m_PrototypeManager->Find_Prototype(protoType, m_LevelManager->Get_CurrentLevelIndex(), objectID);

    if (!protoObject) 
        return nullptr;

    Shared<Object> cloned = nullptr;
    if (protoType == PROTOTYPE::GAMEOBJECT) {
        auto GameObjectPrototype = std::static_pointer_cast<GameObject>(protoObject);
		if (objectGuid.Is_Valid()) {
			if (m_ObjectManager->Find_ByObjectGuid(objectGuid) ||
				FAILED(m_PrefabManager->Prepare_RestoredObjectGuid(objectGuid))) {
				return nullptr;
			}
		}

		cloned = GameObjectPrototype->Clone(arg);
		if (objectGuid.Is_Valid()) {
			const ObjectGuid unconsumedObjectGuid = m_PrefabManager->Consume_RestoredObjectGuid();
			if (unconsumedObjectGuid.Is_Valid()) {
				LOG_ERROR(L"GameObject Clone did not call GameObject::Initialize before returning");
				if (cloned) {
					const Shared<GameObject> gameObject = std::static_pointer_cast<GameObject>(cloned);
					gameObject->On_Destroy();
					Object::Destroy(gameObject);
				}
				return nullptr;
			}
		}
        if (cloned)
        {
            const Shared<GameObject> gameObject = std::static_pointer_cast<GameObject>(cloned);
            if (FAILED(m_ObjectManager->Add_GameObject(targetLevel, gameObject))) {
                gameObject->On_Destroy();
                Object::Destroy(gameObject);
                return nullptr;
            }

            if (GameObjectPrototype->Get_GameObjectType() == GAMEOBJECTTYPE::CAMERA &&
                FAILED(Add_Camera(targetLevel, static_pointer_cast<Camera>(cloned)))) {
                m_ObjectManager->Remove_GameObject(targetLevel, gameObject);
                return nullptr;
            }
        }
    }
    else {
        auto pComponent = std::static_pointer_cast<Component>(protoObject);
        cloned = pComponent->Clone(arg);
    }

    return cloned;
}

Shared<Object> Game::Instantiate_Internal(PROTOTYPE protoType, const wstring& prototypeTag, uint32 levIndex, void *arg) const
{
	if (prototypeTag.empty())
	{
        LOG_ERROR("There is no Tag to prototype");
		return nullptr;
	}

    uint32 objectID = 0;
    objectID = m_PrototypeManager->Get_ObjectIDFromPrototypeTag(prototypeTag, levIndex);
    if (objectID == 0)
    {
        objectID = m_PrototypeManager->Get_ObjectIDFromPrototypeTag(prototypeTag, 0);
    }

    if (objectID == 0)
    {
        LOG_WARN(L"[Game] Failed to find prototype by tag: {}. (Searched Level 0 and {})", prototypeTag, 
            (levIndex == UINT_MAX) ? m_LevelManager->Get_CurrentLevelIndex() : levIndex);
        return nullptr;
    }

    return Instantiate_Internal(protoType, objectID, levIndex, arg);
}

Shared<Object> Game::Instantiate_ByRuntimeTypeId(PROTOTYPE protoType, RuntimeTypeId runtimeTypeId,
                                                 uint32 levIndex, void* arg,
                                                 ObjectGuid objectGuid) const
{
    const uint32 targetLevel = levIndex == UINT_MAX
        ? m_LevelManager->Get_CurrentLevelIndex()
        : levIndex;

    Shared<Object> prototype = m_PrototypeManager->Find_DefaultPrototype(protoType, targetLevel, runtimeTypeId);
    if (!prototype && targetLevel != 0)
        prototype = m_PrototypeManager->Find_DefaultPrototype(protoType, 0, runtimeTypeId);

    return prototype
        ? Instantiate_Internal(protoType, prototype->Get_ObjectID(), targetLevel, arg, objectGuid)
        : nullptr;
}

Shared<GameObject> Game::Instantiate_GameObject(std::string_view registeredName,
                                                uint32 levIndex,
                                                void* arg,
                                                ObjectGuid objectGuid) const
{
    const RuntimeTypeId runtimeTypeId = Find_RuntimeTypeId(registeredName);
    if (runtimeTypeId == 0) {
        LOG_ERROR(L"Unknown GameObject RTTR RegisteredName {}", Helper::To_wString(registeredName));
        return nullptr;
    }

    return static_pointer_cast<GameObject>(
        Instantiate_ByRuntimeTypeId(PROTOTYPE::GAMEOBJECT, runtimeTypeId, levIndex, arg, objectGuid));
}

Bool Game::Can_Instantiate(PROTOTYPE prototypeType, std::string_view registeredName,
	uint32 levIndex) const
{
	const RuntimeTypeId runtimeTypeId = Find_RuntimeTypeId(registeredName);
	const uint32 targetLevel = levIndex == UINT_MAX
		? m_LevelManager->Get_CurrentLevelIndex()
		: levIndex;
	return runtimeTypeId != 0 &&
		m_PrototypeManager->Find_DefaultPrototype(prototypeType, targetLevel, runtimeTypeId) != nullptr;
}

#ifdef _DEBUG
void Game::Render_CollisionDebug() const
{
    m_CollisionManager->Render_Debug();
}

Bool Game::Toggle_RenderDebug() const
{
    return m_CollisionManager->Toggle_DebugMode();
}

HRESULT Game::Ready_RenderTarget_Debug(const wstring& renderTargetTag, Float x, Float y, Float sizeX, Float sizeY) const
{
    return m_RenderTargetManager->Ready_RenderTarget_Debug(renderTargetTag, x, y, sizeX, sizeY);
}

HRESULT Game::Render_RenderTarget_Debug(const Shared<class VIBuffer_Rect>& buffer, const Shared<class Shader>& shader, const wstring& multiRenderTargetTag) const
{
    return m_RenderTargetManager->Render_RenderTarget_Debug(buffer, shader, multiRenderTargetTag);
}

#endif
