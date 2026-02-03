#pragma once
#include "Object.h"

NS_BEGIN(Engine)

class ENGINE_DLL Component abstract : public Object, enable_shared_from_this<Component>
{
public:
	explicit Component(const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& context);
	explicit Component(const Shared<Component>& prototype);
	virtual ~Component() override = default;
	
public:
	constexpr static PROTOTYPE Get_Prototype() { return PROTOTYPE::COMPONENT; }

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(Shared<void> arg) override;

protected:
	ComPtr<ID3D11Device>		m_Device = { nullptr };
	ComPtr<ID3D11DeviceContext> m_Context = { nullptr };

public:
	virtual Shared<Component> Clone(Shared<void> arg) PURE;
private:
	using Object::m_ObjectDesc;
};

NS_END