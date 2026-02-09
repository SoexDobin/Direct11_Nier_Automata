#pragma once
#include "Object.h"

NS_BEGIN(Engine)

class GameObject;

class ENGINE_DLL Component abstract : public Object
{
public:
	explicit Component(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& context);
	explicit Component(const Shared<Component>& prototype);
	virtual ~Component() override = default;
	
public:
	Shared<GameObject> Get_Owner() const { return m_Owner.lock(); }
private:
	friend GameObject;
	void Set_Owner(const Shared<GameObject>& owner) { m_Owner = owner; }

public:
	virtual COMPONENT_TYPE Get_ComponentType() const PURE;
	PROTOTYPE Get_Prototype() const final { return PROTOTYPE::COMPONENT; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const Shared<void>& arg) override;

protected:
	ComPtr<ID3D11Device>		m_Device = { nullptr };
	ComPtr<ID3D11DeviceContext> m_Context = { nullptr };
	Weak<GameObject>			m_Owner = {};

public:
	constexpr virtual Shared<Component> Clone(const Shared<void>& arg) PURE;

private:
	using Object::m_ObjectDesc;
};

NS_END