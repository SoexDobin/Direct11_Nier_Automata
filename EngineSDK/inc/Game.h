#pragma once
#include "Engine_Define.h"

#include "CameraManager.h"
#include "GraphicDevice.h"
#include "InputDevice.h"
#include "LevelManager.h"
#include "LightManager.h"
#include "ObjectManager.h"
#include "Pipeline.h"
#include "PrototypeManager.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "TimeManager.h"

#include "GameObject.h"
#include "Component.h"

NS_BEGIN(Engine)

class LayerRegistry;
class TagRegistry;

class ENGINE_DLL Game {
  DECLARE_SINGLETON(Game)
protected:
  explicit Game() = default;
  ~Game();

public:
  HRESULT Initialize_Engine(const ENGINE_DESC &engineDesc);
  void Update_Engine();
  HRESULT Draw() const;
  HRESULT Draw_NoClearing() const;
  void Clear_Resource(uint32 levIndex) const;
  void Clear_AllResource() const;

public: /* For Editor / Tool */
    const D3D11_VIEWPORT& Get_ViewportDesc() const { return m_GraphicDevice->Get_ViewportDesc(); }
    ComPtr<ID3D11Device> Get_Device() const { return m_GraphicDevice->Get_Device(); }
    ComPtr<ID3D11DeviceContext> Get_Context() const { return m_GraphicDevice->Get_Context(); }
    Shared<LayerRegistry> Get_LayerRegister() const { return m_LayerRegistry; }
    Shared<TagRegistry> Get_TagRegister() const { return m_TagRegistry; }

public: /* For Input Manager */
	void Update_Input() const;

public: /* For GraphicDevice */
    HRESULT Clear_BackBufferView(const Shared<Float4> &clearColor) const;
    HRESULT Present() const;
    HRESULT OnResize(uint32 width, uint32 height, uint32 offScreenIndex = UINT_MAX);
    HRESULT Begin_RenderOffScreen(uint32 screenIndex) const;
    HRESULT End_RenderOffScreen() const;
    ComPtr<ID3D11ShaderResourceView> Get_OffScreenSRV(uint32 screenIndex) const;
    void Set_DepthStencilState(ID3D11DepthStencilState* state, UINT ref) const;

public:
  Byte Get_DIKeyState(uByte byKeyID) const;
  Byte Get_DIMouseState(DIMB mouseInput) const;
  Long Get_DIMouseMove(DIMM mouseState) const;

public: /* For TimeManager */
    HRESULT Add_Timer(const wstring &timerTag) const;
    void Set_TimeScale(Float timeScale) const;
    Float Get_FPS() const;
    Float Compute_TimeDelta() const;
    Float Compute_UnscaledTimeDelta() const;
    Float Compute_TimeDelta(const wstring &timerTag) const;

public: /* For LevelManager */
    Bool LevelLoad_Finished() const { return m_LevelManager->Is_LoadFinished(); }
    uint32 Get_CurrentLevelIndex() const { return m_LevelManager->Get_CurrentLevelIndex(); }
    HRESULT Change_Level(uint32 levIndex, const Shared<class Level>& newLevel);

public: /* For PrototypeManager */
    uint32 Get_ObjectIDFromPrototypeTag(const wstring& prototypeTag, uint32 levIndex) const;
    const tChar* Get_PrototypeTagFromObjectID(uint32 objectID, uint32 levIndex) const;
	const auto &Get_Prototype_Components() const { return m_PrototypeManager->Get_Components(); }
	Shared<const Object> Find_Prototype(PROTOTYPE prototype, uint32 objectID, uint32 levIndex = UINT_MAX) const;

private: /* For ObjectManager */
	HRESULT Add_GameObject(const Shared<GameObject>& GameObject) const;

public: /* For ObjectManager */
	void Submit_RenderGroup() const;
	const unordered_map<uint32, Shared<GameObject>>& Get_GameObjects() const;

public: /* For CameraManager */
    HRESULT Add_Camera(const Shared<class Camera> &camera) const;
    HRESULT Set_MainCamera(const Shared<class Camera> &camera) const;
    Shared<class Camera> Get_MainCamera() const;

public: /* For ResourceManager */
    HRESULT Load_Shader(uint32 levIndex, const tChar* shaderFilePath, const D3D11_INPUT_ELEMENT_DESC* elements, uint32 numElements, const wstring& descriptionTag) const;
    Shared<Shader> Get_Shader(uint32 levIndex, const tChar* shaderFilePath) const;

    HRESULT Load_Texture(uint32 levIndex, const tChar* textureFilePath, uint32 numSRVs, const wstring& descriptionTag) const;
    const Texture::TEXTURE_DESC* Get_TextureDesc(uint32 levIndex, const wstring& descriptionTag) const;
    const ComPtr<ID3D11ShaderResourceView>& Get_Texture(uint32 levIndex, const tChar *textureFilePath) const;

    HRESULT Load_Model(uint32 levIndex, const tChar* modelFilePath, const wstring& descriptionTag, const Matrix& preTransformMatrix) const;
    Shared<Model> Get_Model(uint32 levIndex, const tChar* modelFilePath) const;
    int32 Get_ContainLevelByModelTag(const wstring& modelTag) const;

public: /* For Renderer */
	void Add_RenderGroup(RENDERGROUP group, const Shared<class GameObject> &gameObject) const;

public: /* For Pipeline */
  HRESULT Bind_CameraPosition(const Shared<class Shader> &shader,
                              const Char *constantName) const;
  HRESULT Bind_TransformMatrix(const Shared<class Shader> &shader,
                               const Char *constantName, D3DTS transformState);
  HRESULT Bind_TransformMatrix_Inverse(const Shared<class Shader> &shader,
                                       const Char *constantName,
                                       D3DTS transformState);
  Matrix Get_Transform(D3DTS transformState) const;
  Matrix Get_InvTransform(D3DTS transformState) const;
  Vector4 Get_CamTransform() const;
  void Set_Transform(D3DTS transformState, Matrix transformStateMatrix);
  void Update_Pipeline() const;

public: /* For.Light_Manager */
  const LIGHT_DESC *Get_LightDesc(uint32 index) const;
  HRESULT Add_Light(const LIGHT_DESC &lightDesc) const;
  HRESULT Remove_Light(uint32 index) const;

public: /* Prototype & Instantiate Facade */
    template <typename T>
    HRESULT Add_Prototype(uint32 levIndex, const Shared<T>& prototype, const wstring& prototypeTag = L"") {
        return Add_Prototype_Internal(levIndex, std::static_pointer_cast<Object>(prototype), prototypeTag);
    }
    template <typename T>
    Shared<T> Instantiate(uint32 objectID, uint32 levIndex = UINT_MAX, void* arg = nullptr) {
        PROTOTYPE protoType = std::is_base_of_v<GameObject, T> ? PROTOTYPE::GAMEOBJECT : PROTOTYPE::COMPONENT;
        Shared<Object> cloned = Instantiate_Internal(protoType, objectID, levIndex, arg);
        return std::static_pointer_cast<T>(cloned);
    }
    template <typename T>
    Shared<T> Instantiate(const wstring& prototypeTag, uint32 levIndex = UINT_MAX, void* arg = nullptr) {
        PROTOTYPE protoType = std::is_base_of_v<GameObject, T> ? PROTOTYPE::GAMEOBJECT : PROTOTYPE::COMPONENT;
        Shared<Object> cloned = Instantiate_Internal(protoType, prototypeTag, levIndex, arg);
        return std::static_pointer_cast<T>(cloned);
    }
    Shared<Object> Instantiate(const wstring& prototypeTag, uint32 levIndex = UINT_MAX, void* arg = nullptr) const {
        Shared<Object> cloned = Instantiate_Internal(PROTOTYPE::COMPONENT, prototypeTag, levIndex, arg);
        return cloned;
    }
private: /* Internal Implementation (Non-Template) */
    HRESULT Add_Prototype_Internal(uint32 levIndex, const Shared<Object>& object, const wstring& prototypeTag = L"") const;
    Shared<Object> Instantiate_Internal(PROTOTYPE protoType, uint32 objectID, uint32 levIndex, void* arg = nullptr) const;
    Shared<Object> Instantiate_Internal(PROTOTYPE protoType, const wstring& prototypeTag, uint32 levIndex, void* arg = nullptr) const;

private:
  Shared<LayerRegistry> m_LayerRegistry = {nullptr};
  Shared<TagRegistry> m_TagRegistry = {nullptr};

  Unique<GraphicDevice> m_GraphicDevice = {nullptr};
  Unique<TimeManager> m_TimeManager = {nullptr};
  Unique<InputDevice> m_InputDevice = {nullptr};
  Unique<Pipeline> m_Pipeline = {nullptr};
  Unique<LevelManager> m_LevelManager = {nullptr};
  Unique<PrototypeManager> m_PrototypeManager = {nullptr};
  Unique<ObjectManager> m_ObjectManager = {nullptr};
  Unique<CameraManager> m_CameraManager = {nullptr};
  Unique<ResourceManager> m_ResourceManager = {nullptr};
  Unique<Renderer> m_Renderer = {nullptr};
  Unique<LightManager> m_LightManager = {nullptr};
};

NS_END