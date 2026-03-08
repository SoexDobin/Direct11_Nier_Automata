#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL Mesh final : public VIBuffer
{
	RTTR_ENABLE(VIBuffer)
public:
	Mesh();
	Mesh(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Mesh(const Mesh& rhs);
	~Mesh() override = default;

public:
	COMPONENT_TYPE Get_ComponentType() const override { return COMPONENT_TYPE::MESH; }
	HRESULT Initialize_Prototype(void* aiMesh);
	HRESULT Initialize(void* arg = nullptr) override;
	void On_Destroy() override;

public:
	static Shared<Mesh> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, void* aiMesh);
	Shared<Component> Clone(void* arg = nullptr) override;
};

NS_END