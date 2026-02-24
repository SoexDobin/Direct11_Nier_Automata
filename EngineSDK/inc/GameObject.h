#pragma once
#include "Component.h"
#include "Game.h"
#include "LayerRegistry.h"
#include "Object.h"
#include "TagRegistry.h"
#include <concepts>

NS_BEGIN(Engine)

class Component;
class Transform;

class ENGINE_DLL GameObject abstract
    : public Object,
      public enable_shared_from_this<GameObject> {
public:
  explicit GameObject();
  explicit GameObject(const ComPtr<ID3D11Device> &pDevice,
                      const ComPtr<ID3D11DeviceContext> &context);
  explicit GameObject(const Shared<GameObject> &prototype);
  virtual ~GameObject() override = default;

  operator Shared<Transform>() const { return m_Transform; }
  Shared<Transform> Get_Transform() const { return m_Transform; }

public:
  LAYER_MASK &Get_LayerMask() { return m_LayerMask; }
  TAG_MASK &Get_TagMask() { return m_TagMask; }

public:
  virtual HRESULT Initialize_Prototype() override;
  virtual HRESULT Initialize(void *arg) override;
  void On_Destroy() override;
  void On_Enable() override;
  void On_Disable() override;
  PROTOTYPE Get_Prototype() const final { return PROTOTYPE::GAMEOBJECT; }

public:
  virtual void Priority_Update(Float timeDelta);
  virtual void Update(Float timeDelta);
  virtual void Late_Update(Float timeDelta);
  virtual void Fixed_Update(Float fixedDelta);
  virtual HRESULT Render();

public: // 충돌 함수
protected:
  ComPtr<ID3D11Device> m_Device = {nullptr};
  ComPtr<ID3D11DeviceContext> m_Context = {nullptr};
  Shared<Transform> m_Transform = {nullptr};
  LayerMask m_LayerMask = {};
  TagMask m_TagMask = {};

protected: /* Parent Child */
  Weak<GameObject> m_Parent = {};
  vector<Shared<GameObject>> m_Children;

public:
  Bool Has_Parent() const { return !m_Parent.expired(); }
  HRESULT Set_Parent(const Shared<GameObject> &parent);
  HRESULT Remove_Parent();
  HRESULT Add_Child(const Shared<GameObject> &child);
  HRESULT Remove_Child(const Shared<GameObject> &child);
  Shared<GameObject> Get_Parent() const;
  const vector<Shared<GameObject>> &Get_Children() const;

protected: /* Component */
  map<uint32, Shared<Component>> m_Components;
  unordered_map<uint32, Shared<Component>> m_Scripts;

public:
  template <typename T> Shared<T> Get_Component() {
    static_assert(std::is_base_of_v<Component, T>,
                  "T must derive from Component");
    uint32 level = Game::GetInstance()->Get_CurrentLevelIndex();
    if (Shared<const T> prototype = Game::GetInstance()->Find_Prototype<T>(
            PROTOTYPE::COMPONENT, level)) {
      if (prototype->Get_ComponentType() == COMPONENT_TYPE::SCRIPT &&
          m_Scripts.contains(prototype->Get_ObjectID())) {
        return static_pointer_cast<T>(m_Scripts[prototype->Get_ObjectID()]);
      }
      if (m_Components.contains(ETOI(prototype->Get_ComponentType()))) {
        return static_pointer_cast<T>(
            m_Components[ETOI(prototype->Get_ComponentType())]);
      }
    }
    return nullptr;
  }

  inline Shared<Component> Get_Component(uint32 objectID);

protected:
  template <typename T> Shared<T> Add_Component(void *arg = nullptr) {
    static_assert(std::is_base_of_v<Component, T>,
                  "T must derive from Component");
    if (Shared<T> instance = Game::GetInstance()->Instantiate<T>(arg)) {
      if (instance->Get_ComponentType() == COMPONENT_TYPE::SCRIPT &&
          !m_Scripts.contains(instance->Get_ObjectID())) {
        m_Scripts.emplace(instance->Get_ObjectID(), instance);
      } else if (!m_Components.contains(ETOI(instance->Get_ComponentType()))) {
        m_Components.emplace(ETOI(instance->Get_ComponentType()), instance);
      }
      instance->Set_Owner(shared_from_this());
      return instance;
    }
    return nullptr;
  }

public:
  virtual Shared<GameObject> Clone(void *arg) PURE;

private:
  using Object::m_ObjectDesc;
};

NS_END
