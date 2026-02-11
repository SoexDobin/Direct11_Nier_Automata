#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL Texture final : public Component, enable_shared_from_this<Texture>
{
public:
	Texture(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Texture(const Shared<Texture>& rhs);
	~Texture() override = default;

public:
	COMPONENT_TYPE Get_ComponentType() const override { return COMPONENT_TYPE::TEXTURE; }
	HRESULT Initialize_Prototype(const tChar* textureFilePath, uint32 numSRVs);
	HRESULT Initialize(const Shared<void>& arg) override;
	void On_Destroy() override;

private:
	uint32										m_NumSRVs = {};
	vector<ComPtr<ID3D11ShaderResourceView>>	m_SRVs;

public:
	static Shared<Texture> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, const tChar* textureFilePath, uint32 numSRVs);
	Shared<Component> Clone(const Shared<void>& arg = nullptr) override;
};

NS_END