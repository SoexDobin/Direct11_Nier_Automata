#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL VIBuffer abstract : public Component
{
public:
	VIBuffer(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context);
	VIBuffer(const Shared<VIBuffer>& rhs);
	virtual ~VIBuffer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const Shared<void>& arg = nullptr) override;
	virtual void On_Destroy() override { Component::On_Destroy(); }
	virtual void On_Disable() override { Component::On_Disable(); }
	virtual void On_Enable() override { Component::On_Enable(); }
	virtual void Set_Active(Bool isActive) override { Component::Set_Active(isActive); }
	virtual COMPONENT_TYPE Get_ComponentType() const override PURE;

public:
	virtual HRESULT Bind_Resources();
	virtual HRESULT Render();

protected:
	ComPtr<ID3D11Buffer>				m_VB = { nullptr };
	ComPtr<ID3D11Buffer>				m_IB = { nullptr };
	
protected:
	uint32								m_NumVtxBuffers = {};
	uint32								m_NumVertices = {};
	uint32								m_VtxStride = {};
	
	uint32								m_NumIndices = {};
	uint32								m_IndexStride = {};
	D3D11_PRIMITIVE_TOPOLOGY			m_PrimitiveType = {};

public:
	virtual Shared<Component> Clone(const Shared<void>& arg) override PURE;
};

NS_END