#pragma once
#include "Object.h"
#include "LayerRegistry.h"
#include "TagRegistry.h"

NS_BEGIN(Engine)

class Component;
class Transform;

class ENGINE_DLL GameObject abstract : public Object, enable_shared_from_this<GameObject>
{
public:
	explicit GameObject(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& context);
	explicit GameObject(const Shared<GameObject>& prototype);
	virtual ~GameObject() override = default;


	operator Shared<Transform>() const { return m_Transform; }
	Shared<Transform> Get_Transform() const { return m_Transform; }
public:
	LAYER_MASK& Get_LayerMask() { return m_LayerMask; }
	TAG_MASK& Get_TagMask() { return m_TagMask; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const Shared<void>& arg) override;
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
	ComPtr<ID3D11Device>		m_Device = { nullptr };
	ComPtr<ID3D11DeviceContext> m_Context = { nullptr };
	Shared<Transform>			m_Transform = { nullptr };
	LayerMask					m_LayerMask = {};
	TagMask						m_TagMask = {};

protected: /* Parent Child */
	Weak<GameObject>				m_Parent = {};
	vector<Shared<GameObject>>		m_Children;

public: 
	Bool Has_Parent() const { return !m_Parent.expired(); }
	HRESULT Set_Parent(const Shared<GameObject>& parent);
	HRESULT Remove_Parent();
	HRESULT Add_Child(const Shared<GameObject>& child);
	HRESULT Remove_Child(const Shared<GameObject>& child);
	Shared<GameObject> Get_Parent() const;
	const vector<Shared<GameObject>>& Get_Children() const;

protected: /* Component */
	map<uint32, Shared<Component>>				m_Components;
	unordered_map<uint32, Shared<Component>>	m_Scripts;

public: /* GameObject Util At GameObjectUtil.cpp */
	template<typename T>
	constexpr Shared<T> Get_Component();
	inline Shared<Component> Get_Component(uint32 objectID);

protected: /* GameObject Util At GameObjectUtil.cpp */
	template<typename T>
	constexpr Shared<T> Add_Component(const Shared<void>& arg = nullptr);

public:
	virtual Shared<GameObject> Clone(const Shared<void>& arg) PURE;

private:
	using Object::m_ObjectDesc;
};

NS_END
