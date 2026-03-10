#pragma once
#include "Transform.h"
#include "LayerRegistry.h"
#include "Object.h"
#include "ScriptComponent.h"
#include "TagRegistry.h"
#include "String_Helper.h"

NS_BEGIN(Engine)
class Game;
class Component;
class ScriptComponent;
class Transform;

class ENGINE_DLL GameObject abstract : public Object, public enable_shared_from_this<GameObject> {
	RTTR_ENABLE(Object)
public:
	typedef struct tagGameObjectDesc : public OBJECT_DESC {} GAMEOBJECT_DESC;

public:
    explicit GameObject();
    explicit GameObject(const ComPtr<ID3D11Device> &pDevice,
                        const ComPtr<ID3D11DeviceContext> &context);
    explicit GameObject(const GameObject &prototype);
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
    virtual void Submit_RenderGroup();

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
    map<uint32, Shared<ScriptComponent>> m_Scripts;

public:
    inline Shared<Component> Get_Component(uint32 objectID);
    const vector<Shared<Component>> Get_Components();
    const vector<Shared<ScriptComponent>> Get_Scripts();
    HRESULT Add_Component(const Shared<Component> &component);
    Shared<Component> Add_Component(uint32 objectID, void* arg = nullptr);
    Shared<Component> Add_Component(const wstring& prototypeTag, void* arg = nullptr);

    template <typename T>
    Shared<T> Add_Component(void* arg = nullptr)
    {
        return static_pointer_cast<T>(Add_Component(Helper::To_wString(rttr::type::get<T>().get_name()), arg));
    }

protected:
     template <typename T>
     requires is_base_of_v<Component, T>
      	Shared<T> Get_Component() {
        uint32 typeID = rttr::type::get<T>().get_id();

        for (auto &[objectID, component] : m_Components) {
            if (component->Get_TypeID() == typeID) {
              return std::static_pointer_cast<T>(component);
            }
        }
        for (auto &[objectID, script] : m_Scripts) {
            if (script->Get_TypeID() == typeID) {
				auto base = std::static_pointer_cast<Component>(script);
				return std::static_pointer_cast<T>(base);
            }
        }

        return nullptr;
    }
      	
public:
	virtual Shared<GameObject> Clone(void *arg) PURE;

private:
	using Object::m_DescID;
};

NS_END
