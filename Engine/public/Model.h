#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class Mesh;

class ENGINE_DLL Model final : public Component, public enable_shared_from_this<Model>
{

public:
	explicit Model();
	explicit Model(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Model(const Model& rhs);
	~Model() override = default;

public:
	COMPONENT_TYPE Get_ComponentType() const override { return COMPONENT_TYPE::MODEL; }
	HRESULT Initialize_Prototype(const wstring& path);
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;

public:
	HRESULT Render();
	HRESULT Ready_Meshes();

private:
	uint32 m_NumMeshes = {};
	vector<Shared<Mesh>> m_Meshes = {};

public:
	static Shared<Model> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, const wstring& path);
	Shared<Component> Clone(void* arg = nullptr) override;
};

NS_END