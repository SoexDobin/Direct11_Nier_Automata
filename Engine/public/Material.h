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
	HRESULT Initialize_Prototype(const aiMaterial* aiMaterial, const Char* modelFilePath);
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;

public:
	HRESULT Bind_Material(const Shared<Shader>& shader, const Char* constantName, aiTextureType materialType, uint32 textureIndex);
	
public:
	vector<ComPtr<ID3D11ShaderResourceView>> m_MaterialTextures[AI_TEXTURE_TYPE_MAX];

public:
	static Shared<Material> CreatePrototype();
	static Shared<Material> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, const aiMaterial* aiMaterial, const Char* modelFilePath);
	Shared<Component> Clone(void* arg = nullptr) override;
};

NS_END