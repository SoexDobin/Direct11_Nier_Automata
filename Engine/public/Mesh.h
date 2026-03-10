#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class Mesh final : public VIBuffer
{
	RTTR_ENABLE(VIBuffer)
public:
	explicit Mesh();
	explicit Mesh(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Mesh(const Mesh& rhs);
	~Mesh() override = default;

public:
	COMPONENT_TYPE Get_ComponentType() const override { return COMPONENT_TYPE::MESH; }
	HRESULT Initialize_Prototype(const aiMesh* aiMesh, const Matrix& preTransformMatrix);
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg = nullptr) override;
	void On_Destroy() override;

public:
	uint32 Get_MaterialIndex() const { return m_MaterialIndex; }

private:
	uint32 m_MaterialIndex{};

public:
	static Shared<Mesh> CreatePrototype();
	static Shared<Mesh> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, const aiMesh* aiMesh, const Matrix& preTransformMatrix);
	Shared<Component> Clone(void* arg = nullptr) override;
};

NS_END