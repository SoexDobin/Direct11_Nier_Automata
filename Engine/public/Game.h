#pragma once
#include "Engine_Define.h"

#include "GraphicDevice.h"
#include "LevelManager.h"
#include "ObjectManager.h"
#include "PrototypeManager.h"
#include "Renderer.h"
#include "String_Helper.h"
#include "TimeManager.h"
#include "Pipeline.h"

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
  HRESULT Draw();
  void Clear_Resource(uint32 levIndex);

public: /* For Editor / Tool */
  ComPtr<ID3D11Device> Get_Device() const {
    return m_GraphicDevice->Get_Device();
  }
  ComPtr<ID3D11DeviceContext> Get_Context() const {
    return m_GraphicDevice->Get_Context();
  }
  Shared<LayerRegistry> Get_LayerRegister() const { return m_LayerRegistry; }
  Shared<TagRegistry> Get_TagRegister() const { return m_TagRegistry; }

public: /* For GraphicDevice */
  HRESULT Clear_BackBufferView(const Shared<Float4> &clearColor) const;
  HRESULT Present() const;
  HRESULT OnResize(uint32 width, uint32 height);
  HRESULT Begin_RenderOffScreen(const wstring &rtTag) const;
  HRESULT End_RenderOffScreen() const;
  ComPtr<ID3D11ShaderResourceView> Get_OffScreenSRV(const wstring &rtTag) const;

public: /* For TimeManager */
  HRESULT Add_Timer(const wstring &timerTag) const;
  Float Compute_TimeDelta(const wstring &timerTag) const;

public: /* For LevelManager */
  uint32 Get_CurrentLevelIndex() const {
    return m_LevelManager->Get_CurrentLevelIndex();
  }
  HRESULT Change_Level(uint32 levIndex, Unique<class Level> newLevel);

public: /* For PrototypeManager */
  uint32 Get_PrototypeID(const wstring &name) const {
    return m_PrototypeManager->Get_TypeByName(name);
  }
  const wstring &Get_PrototypeName(uint32 typeID) const {
    return m_PrototypeManager->Get_NameByType(typeID);
  }
  HRESULT Add_Prototype(uint32 levIndex,
                        const Shared<class Object> &prototype) const;

  const auto &Get_Prototype_Components() const {
    return m_PrototypeManager->Get_Components();
  }
  const auto &Get_Prototype_NameMap() const {
    return m_PrototypeManager->Get_NameByTypes();
  }

private: /* For ObjectManager */
  HRESULT Add_GameObject(const Shared<GameObject> &GameObject) const;

public: /* For Renderer */
  void Add_RenderGroup(RENDERGROUP group, const Shared<class GameObject> &gameObject) const;

public:
    HRESULT Bind_TransformMatrix(const Shared<class Shader>& shader, const Char* constantName, D3DTS transformState);
    HRESULT Bind_TransformMatrix_Inverse(const Shared<class Shader>& shader, const Char* constantName, D3DTS transformState);
	const Matrix& Get_Transform(D3DTS transformState) const;
	const Vector3& Get_CamTransform() const;
	void Set_Transform(D3DTS transformState, Matrix transformStateMatrix);

public:
  template <typename T>
  Shared<const T> Find_Prototype(PROTOTYPE prototype, uint32 levIndex = MAXINT32) const 
	{
    uint32 level = levIndex == MAXINT32
                       ? m_LevelManager->Get_CurrentLevelIndex()
                       : levIndex;
    const wstring &className = Helper::To_wString(typeid(T).name());
    if (Shared<Object> object =
            m_PrototypeManager->Find_Prototype(prototype, level, className)) {
      return static_pointer_cast<const T>(object);
    }
    return nullptr;
  }

  inline Shared<const Object> Find_Prototype(PROTOTYPE prototype, uint32 typeID,
                                             uint32 levIndex = MAXINT32) const;
  inline Shared<const Object> Find_Prototype(PROTOTYPE prototype,
                                             const wstring &className,
                                             uint32 levIndex = MAXINT32) const;

  template <typename T> Shared<T> Instantiate(void *arg = nullptr) const {
    uint32 level = m_LevelManager->Get_CurrentLevelIndex();
    PROTOTYPE typeTag = std::is_base_of_v<class GameObject, T>
                            ? PROTOTYPE::GAMEOBJECT
                            : PROTOTYPE::COMPONENT;
    const wstring &className = Helper::To_wString(typeid(T).name());
    if (auto instance = Instantiate_Internal(typeTag, level, className, arg)) {
      return static_pointer_cast<T>(instance);
    }
    return nullptr;
  }

  template <typename T>
  Shared<T> Instantiate(uint32 typeID, void *arg = nullptr) const {
    uint32 level = m_LevelManager->Get_CurrentLevelIndex();
    PROTOTYPE typeTag = std::is_base_of_v<class GameObject, T>
                            ? PROTOTYPE::GAMEOBJECT
                            : PROTOTYPE::COMPONENT;
    if (auto instance = Instantiate_Internal(typeTag, level, typeID, arg)) {
      return static_pointer_cast<T>(instance);
    }
    return nullptr;
  }

  template <typename T>
  Shared<T> Instantiate(const wstring &className, void *arg = nullptr) const {
    uint32 level = m_LevelManager->Get_CurrentLevelIndex();
    PROTOTYPE typeTag = std::is_base_of_v<class GameObject, T>
                            ? PROTOTYPE::GAMEOBJECT
                            : PROTOTYPE::COMPONENT;
    if (auto instance = Instantiate_Internal(typeTag, level, className, arg)) {
      return static_pointer_cast<T>(instance);
    }
    return nullptr;
  }

private:
  inline Shared<Object> Instantiate_Internal(PROTOTYPE protoType,
                                             uint32 levIndex, uint32 typeID,
                                             void *arg = nullptr) const;
  inline Shared<Object> Instantiate_Internal(PROTOTYPE protoType,
                                             uint32 levIndex,
                                             const wstring &className,
                                             void *arg = nullptr) const;

private:
	Shared<LayerRegistry> m_LayerRegistry = {nullptr};
	Shared<TagRegistry> m_TagRegistry = {nullptr};

	Unique<GraphicDevice> m_GraphicDevice = {nullptr};
	Unique<TimeManager> m_TimeManager = {nullptr};
	Unique<Pipeline> m_Pipeline = { nullptr };
	Unique<LevelManager> m_LevelManager = {nullptr};
	Unique<PrototypeManager> m_PrototypeManager = {nullptr};
	Unique<ObjectManager> m_ObjectManager = {nullptr};
	Unique<Renderer> m_Renderer = {nullptr};
};

NS_END