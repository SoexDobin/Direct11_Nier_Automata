#include "pch.h"
#include "SkyBoxLayer.h"
#include <Game.h>

#include "VICube.h"
#include "Shader.h"
#include "Texture.h"

SkyBoxLayer::SkyBoxLayer(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: GameObject{ device, context } {
}
SkyBoxLayer::SkyBoxLayer(const SkyBoxLayer& rhs)
	: GameObject{ rhs } {
}

HRESULT SkyBoxLayer::Initialize_Prototype()
{
	return GameObject::Initialize_Prototype();
}

HRESULT SkyBoxLayer::Initialize(void* arg)
{
	if (FAILED(GameObject::Initialize(arg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_Transform->Set_LocalScale({ 10.f, 10.f, 10.f });

	m_Texture->Set_RGBA(Color{ 95.f / 255.f, 20.f / 255.f, 95.f / 255.f, 1.f });

	return S_OK;
}

void SkyBoxLayer::Priority_Update(Float timeDelta)
{

}

void SkyBoxLayer::Update(Float timeDelta)
{

}

void SkyBoxLayer::Late_Update(Float timeDelta)
{

}

void SkyBoxLayer::Fixed_Update(Float fixedDelta)
{

}

HRESULT SkyBoxLayer::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_Shader->Begin(1)))
		return E_FAIL;

	if (FAILED(m_CubeBuffer->Bind_Resources()))
		return E_FAIL;

	if (FAILED(m_CubeBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

void SkyBoxLayer::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::PRIORITY, shared_from_this());
}

HRESULT SkyBoxLayer::Bind_ShaderResources()
{
	if (FAILED(m_Transform->Bind_ShaderResource(m_Shader, WorldMatrix)))
		return E_FAIL;

	if (FAILED(GAME_INSTANCE->Bind_TransformMatrix(m_Shader, ViewMatrix, D3DTS::VIEW)))
		return E_FAIL;

	if (FAILED(GAME_INSTANCE->Bind_TransformMatrix(m_Shader, ProjMatrix, D3DTS::PROJ)))
		return E_FAIL;

	if (FAILED(m_Texture->Bind_ShaderResourceView(m_Shader, DefaultMap, 0)))
		return E_FAIL;

	if (FAILED(m_Shader->Bind_RawValue(DefaultColor, *m_Texture->Get_RGBA_Absolute(), sizeof(Color))))
		return E_FAIL;

	return S_OK;
}

HRESULT SkyBoxLayer::Ready_Components()
{
	Shader::SHADER_DESC shaderDesc{ VTXCUBE::Tag, VTXCUBE::Elements, VTXCUBE::numElements };
	m_Shader = Add_Component<Shader>(ETOI(LEVEL::STATIC), &shaderDesc);

	auto textureDesc = Texture::TEXTURE_DESC{ ETOI(LEVEL::STATIC), L"Skybox_Default" };
	m_Texture = Add_Component<Texture>(ETOI(LEVEL::STATIC), &textureDesc);
	if (nullptr == m_Texture)
		return E_FAIL;

	m_CubeBuffer = Add_Component<VICube>(ETOI(LEVEL::STATIC));

	return S_OK;
}

Shared<SkyBoxLayer> SkyBoxLayer::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<SkyBoxLayer>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : SkyBoxLayer");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> SkyBoxLayer::Clone(void* arg)
{
	auto instance = make_shared<SkyBoxLayer>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : SkyBoxLayer");
		return nullptr;
	}

	return instance;
}



