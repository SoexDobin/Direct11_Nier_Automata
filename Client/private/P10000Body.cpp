#include "pch.h"
#include "P10000Body.h"

#include <SpdLogger.h>

#include "Game.h"

#include "Model.h"
#include "StateMachine.h"

#include "State2B_Idle.h"
#include "State2B_Walk.h"
#include "State2B_Run.h"
#include "State2B_Sprint.h"
#include "State2B_Jump.h"

P10000Body::P10000Body() : PartObject{} {}
P10000Body::P10000Body(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: PartObject{device, context} {}
P10000Body::P10000Body(const P10000Body& rhs)
	: PartObject{rhs} {}

HRESULT P10000Body::Initialize_Prototype()
{
	return PartObject::Initialize_Prototype();
}

HRESULT P10000Body::Initialize(void* arg)
{
	if (FAILED(PartObject::Initialize(arg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
	{
		LOG_ERROR(L"Failed To Ready_Components : P10000Body");
		return E_FAIL;
	}

	if (FAILED(Ready_States()))
	{
		LOG_ERROR(L"Failed To Ready_States : P10000Body");
		return E_FAIL;
	}

	return S_OK;
}

void P10000Body::On_Destroy()
{
	PartObject::On_Destroy();
}

void P10000Body::Priority_Update(Float timeDelta)
{
	
}

void P10000Body::Update(Float timeDelta)
{
	
}

void P10000Body::Late_Update(Float timeDelta)
{
	
}

void P10000Body::Fixed_Update(Float fixedDelta)
{
	
}

HRESULT P10000Body::Render()
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

void P10000Body::Submit_RenderGroup()
{
	if (Is_Active())
		GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

HRESULT P10000Body::Bind_ShaderResources()
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

HRESULT P10000Body::Ready_Components()
{
	Shader::SHADER_DESC shaderDesc{ VTXANIMMESH::Tag,  VTXANIMMESH::Elements, VTXANIMMESH::numElements };
	m_Shader = Add_Component<Shader>(&shaderDesc);
	if (nullptr == m_Shader)
		return E_FAIL;

	Model::MODEL_DESC modelDesc{ L"p10000" };
	m_Model = Add_Component<Model>(&modelDesc);
	if (nullptr == m_Model)
		return E_FAIL;

	m_StateMachine = Add_Component<StateMachine>();
	if (nullptr == m_StateMachine)
		return E_FAIL;

	return S_OK;
}

HRESULT P10000Body::Ready_States()
{
	if (m_StateMachine)
	{
		auto this_2B = static_pointer_cast<P10000Body>(shared_from_this());

		m_StateMachine->Add_State(State2B_Idle::Create(L"State2B_Idle", this_2B));
		m_StateMachine->Add_State(State2B_Walk::Create(L"State2B_Walk", this_2B));
		m_StateMachine->Add_State(State2B_Idle::Create(L"State2B_Run", this_2B));
		m_StateMachine->Add_State(State2B_Sprint::Create(L"State2B_Sprint", this_2B));
		m_StateMachine->Add_State(State2B_Jump::Create(L"State2B_Jump", this_2B));

		m_StateMachine->Change_State(L"State2B_Idle");
	}

	return S_OK;
}

Shared<P10000Body> P10000Body::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	auto prototype = make_shared<P10000Body>(device, context);

	if (FAILED(prototype->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : P10000Body");
		return nullptr;
	}

	return prototype;
}

Shared<GameObject> P10000Body::Clone(void* arg)
{
	auto instance = make_shared<P10000Body>(*this);

	if (FAILED(instance->Initialize(arg)))
	{
		MSG_BOX("Failed to Clone : P10000Body");
		return nullptr;
	}

	return instance;
}


