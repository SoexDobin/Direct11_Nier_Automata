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
#include "String_Helper.h"
#include "TimeManager.h"
#include "ResourceManager.h"

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
    ComPtr<ID3D11Device> Get_Device() const { return m_GraphicDevice->Get_Device(); }
    ComPtr<ID3D11DeviceContext> Get_Context() const { return m_GraphicDevice->Get_Context(); }
    Shared<LayerRegistry> Get_LayerRegister() const { return m_LayerRegistry; }
    Shared<TagRegistry> Get_TagRegister() const { return m_TagRegistry; }

public: /* For Input Manager */
    void Update_Input() const;

public: /* For GraphicDevice */
    HRESULT Clear_BackBufferView(const Shared<Float4> &clearColor) const;
    HRESULT Present() const;
    HRESULT OnResize(uint32 width, uint32 height);
    HRESULT Begin_RenderOffScreen(uint32 screenIndex) const;
    HRESULT End_RenderOffScreen() const;
    ComPtr<ID3D11ShaderResourceView> Get_OffScreenSRV(uint32 screenIndex) const;

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
    uint32 Get_CurrentLevelIndex() const { return m_LevelManager->Get_CurrentLevelIndex(); }
    HRESULT Change_Level(uint32 levIndex, Unique<class Level> newLevel);

public: /* For PrototypeManager */
  const auto &Get_Prototype_Components() const {
    return m_PrototypeManager->Get_Components();
  }

private: /* For ObjectManager */
  HRESULT Add_GameObject(const Shared<GameObject>& GameObject) const;

public: /* For ObjectManager */
	void Submit_RenderGroup() const;
    unordered_map<uint32, Shared<GameObject>>& Get_GameObjects() const;

public: /* For CameraManager */
  HRESULT Add_Camera(const Shared<class Camera> &camera) const;
  HRESULT Set_MainCamera(const Shared<class Camera> &camera) const;
  Shared<class Camera> Get_MainCamera() const;

public: /* For ResourceManager */
    uint32 Get_ResourceTypeID(const wstring& resourcePath) const;
    HRESULT Add_ResourceTypeID(const wstring& resourcePath, uint32 typeID) const; //  TODO : 이거는 Engine 내부적으로만 써야하는데

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
  Vector4 Get_CamTransform() const;
  void Set_Transform(D3DTS transformState, Matrix transformStateMatrix);
  void Update_Pipeline() const;

public: /* For.Light_Manager */
  const LIGHT_DESC *Get_LightDesc(uint32 index) const;
  HRESULT Add_Light(const LIGHT_DESC &lightDesc) const;
  HRESULT Remove_Light(uint32 index) const;

public:
    template <typename T> requires std::is_base_of_v<Object, T>
    HRESULT Add_Prototype(const Shared<T>& prototype, uint32 levIndex = UINT_MAX) {
        uint32 level = levIndex == UINT_MAX ? m_LevelManager->Get_CurrentLevelIndex() : levIndex;
        return m_PrototypeManager->Add_Prototype(level, prototype->Get_ObjectID());
    }
    template <typename T> requires std::is_base_of_v<Object, T>
    Shared<T> Instantiate(uint32 levIndex = UINT_MAX, uint32 objectID = UINT_MAX, void* arg = nullptr) {
        uint32 level = levIndex == UINT_MAX ? m_LevelManager->Get_CurrentLevelIndex() : levIndex;
        PROTOTYPE protoType = std::is_base_of_v<GameObject, T> ? PROTOTYPE::GAMEOBJECT : PROTOTYPE::COMPONENT;

        uint32 typeID = rttr::type::get<T>().get_id();
        auto& vecType = m_PrototypeManager->Find_Prototypes(level, typeID);
        if (vecType.empty()) {
            MSG_BOX("Failed to find Prototypes by TypeID");
        }

        if (vecType.size() > 1)
        {
	        
        }
        else
        {
	        
        }

        Shared<Object> original = m_PrototypeManager->Find_Prototype(protoType, level, objectID);

        // 2. 없으면 nullptr 반환
        if (!original) return nullptr;

        // 3. Clone()을 호출하여 복제본(Instance) 생성 및 T타입으로 캐스팅 반환
        return std::static_pointer_cast<T>(original->Clone(arg));
    }

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