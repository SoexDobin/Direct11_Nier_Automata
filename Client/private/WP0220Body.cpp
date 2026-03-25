#include "pch.h"
#include "WP0220Body.h"

#include <SpdLogger.h>

#include "Game.h"
#include "Model.h"


WP0220Body::WP0220Body() : P10000Parts{} {}
WP0220Body::WP0220Body(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: P10000Parts{ device, context } {
}
WP0220Body::WP0220Body(const WP0220Body& rhs)
	: P10000Parts{ rhs } {
}

HRESULT WP0220Body::Initialize_Prototype()
{
	return P10000Parts::Initialize_Prototype();
}

HRESULT WP0220Body::Initialize(void* arg)
{
	if (FAILED(P10000Parts::Initialize(arg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
	{
		LOG_ERROR(L"Failed To Ready_Components : P10000Body");
		return E_FAIL;
	}

	return S_OK;
}

void WP0220Body::On_Destroy()
{
	PartObject::On_Destroy();
}

void WP0220Body::Priority_Update(Float timeDelta)
{

}

void WP0220Body::Update(Float timeDelta)
{

}

void WP0220Body::Late_Update(Float timeDelta)
{

}

void WP0220Body::Fixed_Update(Float fixedDelta)
{

}

HRESULT WP0220Body::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	size_t numMeshes = m_Model->Get_NumMeshes();
	for (uint32 i = 0; i < numMeshes; ++i)
	{
		m_Model->Bind_Material(m_Shader, DiffuseMap, i, 1, 0);
		m_Model->Bind_BoneMatrices(m_Shader, BoneMatrices, i);

		if (FAILED(m_Shader->Begin(0)))
			return E_FAIL;

		m_Model->Render(i);
	}

	return S_OK;
}

void WP0220Body::Submit_RenderGroup()
{
	if (Is_Active())
		GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

HRESULT WP0220Body::Bind_ShaderResources()
{
	if (FAILED(m_Transform->Bind_ShaderResource(m_Shader, WorldMatrix)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_TransformMatrix(m_Shader, ViewMatrix, D3DTS::VIEW)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_TransformMatrix(m_Shader, ProjMatrix, D3DTS::PROJ)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Bind_CameraPosition(m_Shader, CameraPosition)))
		return E_FAIL;

	const LIGHT_DESC* lightDesc = GAME_INSTANCE->Get_LightDesc(0);
	if (nullptr == lightDesc)
		return E_FAIL;

	if (FAILED(m_Shader->Bind_RawValue(DirectionLight, &lightDesc->direction, sizeof(Float4))))
		return E_FAIL;
	if (FAILED(m_Shader->Bind_RawValue(DiffuseLight, &lightDesc->diffuse, sizeof(Float4))))
		return E_FAIL;
	if (FAILED(m_Shader->Bind_RawValue(AmbientLight, &lightDesc->ambient, sizeof(Float4))))
		return E_FAIL;
	if (FAILED(m_Shader->Bind_RawValue(SpecularLight, &lightDesc->specular, sizeof(Float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT WP0220Body::Ready_Components()
{
	Shader::SHADER_DESC shaderDesc{ VTXANIMMESH::Tag,  VTXANIMMESH::Elements, VTXANIMMESH::numElements };
	m_Shader = Add_Component<Shader>(ETOI(LEVEL::STATIC), &shaderDesc);
	if (nullptr == m_Shader)
		return E_FAIL;

	Model::MODEL_DESC modelDesc{ L"wp0220" };
	m_Model = Add_Component<Model>(ETOI(LEVEL::STATIC), &modelDesc);
	if (nullptr == m_Model)
		return E_FAIL;

	return S_OK;
}

Shared<WP0220Body> WP0220Body::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<WP0220Body>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : WP0220Body");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> WP0220Body::Clone(void* arg)
{
	auto instance = make_shared<WP0220Body>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : WP0220Body");
		return nullptr;
	}

	return instance;
}


