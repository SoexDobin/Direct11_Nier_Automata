#pragma once
#include "Object.h"

NS_BEGIN(Engine)

class ENGINE_DLL GameObject abstract : public Object, enable_shared_from_this<GameObject>
{
public:
	explicit GameObject(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& context);
	explicit GameObject(const Shared<GameObject>& prototype);
	virtual ~GameObject() override = default;

public:
	constexpr static PROTOTYPE Get_Prototype() { return PROTOTYPE::GAMEOBJECT; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(Shared<void> arg) override;
	
public:
	virtual void Priority_Update(Float timeDelta);
	virtual void Update(Float timeDelta);
	virtual void Late_Update(Float timeDelta);
	virtual HRESULT Render();

protected:
	ComPtr<ID3D11Device>		m_Device = { nullptr };
	ComPtr<ID3D11DeviceContext> m_Context = { nullptr };
	// TODO : Transform
	// TODO : Parent GameObject
	// TODO : Child GameObjects

public:
	virtual Shared<GameObject> Clone(Shared<void> arg) PURE;

private:
	using Object::m_ObjectDesc;
};

NS_END