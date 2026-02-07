#pragma once
#include "Object.h"
#include "LayerRegistry.h"
#include "TagRegistry.h"

NS_BEGIN(Engine)

class Component;
class ScriptComponent;
class Transform;

class ENGINE_DLL GameObject abstract : public Object, enable_shared_from_this<GameObject>
{
public:
	explicit GameObject(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& context);
	explicit GameObject(const Shared<GameObject>& prototype);
	virtual ~GameObject() override = default;

public:
	LAYER_MASK& Get_LayerMask() { return m_LayerMask; }
	TAG_MASK& Get_TagMask() { return m_TagMask; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const Shared<void>& arg) override;
	// TODO : Disable child
	// TODO : Destroy child
	PROTOTYPE Get_Prototype() const final { return PROTOTYPE::GAMEOBJECT; }
	
public:
	virtual void Priority_Update(Float timeDelta);
	virtual void Update(Float timeDelta);
	virtual void Late_Update(Float timeDelta);
	virtual void Fixed_Update(Float fixedDelta);
	virtual HRESULT Render();

protected:
	ComPtr<ID3D11Device>		m_Device = { nullptr };
	ComPtr<ID3D11DeviceContext> m_Context = { nullptr };
	Shared<Transform>			m_Transform = { nullptr };
	LayerMask					m_LayerMask = {};
	TagMask						m_TagMask = {};

protected:
	unordered_map<uint32, list<Shared<Component>>>			m_Components;
	unordered_map<uint32, list<Shared<ScriptComponent>>>	m_Scripts;

	// TODO : Parent GameObject 
	// TODO : Child GameObjects ?????

public: /* GameObject Util At GameObjectUtil.cpp */
	template<typename T>
	constexpr Shared<T> Get_Component() const;
	inline Shared<Component> Get_Component(uint32 id, Bool Is_ObjectID) const;
	template<typename T>
	constexpr Shared<T> Get_Components() const;
	inline Shared<Component> Get_Components(uint32 id) const;

protected: /* GameObject Util At GameObjectUtil.cpp */
	template<typename T>
	constexpr Shared<T> Add_Component(const Shared<void>& arg = nullptr);
	inline Shared<Component> Add_Component(uint32 typeID, const Shared<void>& arg = nullptr);

public:
	constexpr virtual Shared<GameObject> Clone(const Shared<void>& arg) PURE;

private:
	using Object::m_ObjectDesc;
};

NS_END