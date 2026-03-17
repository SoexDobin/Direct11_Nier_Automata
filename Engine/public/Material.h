#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class Shader;

class Material final : public Component
{
	RTTR_ENABLE(Component)
public:
	explicit Material();
	explicit Material(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Material(const Material& rhs);
	~Material() override = default;

public:
	COMPONENT_TYPE Get_ComponentType() const override { return COMPONENT_TYPE::MATERIAL; }
	void On_Destroy() override;
	void On_Disable() override { Component::On_Disable(); }
	void On_Enable() override { Component::On_Enable(); }
	void Set_Active(Bool isActive) override { Component::Set_Active(isActive); }
	
public:
	HRESULT Initialize_Prototype(const MODEL_MATERIAL& materialData);
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;

public:
	HRESULT Bind_Material(const Shared<Shader>& shader, const Char* constantName, uint32 textureTypeIndex, uint32 textureIndex);

private:
	uint32 m_TextureTypeMax{};
	Shared<vector<ComPtr<ID3D11ShaderResourceView>>[]> m_MaterialTextures;


public:
	static Shared<Material> CreatePrototype();
	static Shared<Material> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, const MODEL_MATERIAL& modelMaterial);
	Shared<Component> Clone(void* arg = nullptr) override { return nullptr; }
};

NS_END