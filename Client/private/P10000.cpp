#include "pch.h"
#include "P10000.h"

#include <Game.h>
#include <SpdLogger.h>

#include "Shader.h"
#include "Model.h"

#include "StateMachine.h"
#include "State2B_Idle.h"
#include "State2B_Walk.h"

namespace Client {

P10000::P10000() : ContainerObject{} {}
P10000::P10000(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: ContainerObject{ device, context } {}
P10000::P10000(const P10000& rhs)
	: ContainerObject{ rhs } {}

HRESULT P10000::Initialize_Prototype()
{
	return ContainerObject::Initialize_Prototype();
}

HRESULT P10000::Initialize(void* arg)
{
	if (FAILED(ContainerObject::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize GameObject {}", m_ObjectName);
		return E_FAIL;
	}

	if (FAILED(Ready_Components()))
	{
		LOG_ERROR(L"Failed to Ready Components {}", m_ObjectName);
		return E_FAIL;
	}

	return S_OK;
}

void P10000::On_Destroy()
{
	ContainerObject::On_Destroy();
	m_StateMachine->On_Destroy();
}

void P10000::Priority_Update(Float timeDelta)
{
	
}

void P10000::Update(Float timeDelta)
{
	
	if (nullptr != m_Model)
		m_Model->Update_ModelAnimation(timeDelta);
	if (nullptr != m_StateMachine)
		m_StateMachine->Update_State(timeDelta);

	if (GAME_INSTANCE->Get_DIKeyState(static_cast<uByte>(DIKEYBOARD_DOWN)))
		m_Model->Set_AnimationIndex(++m_tempIdx);
}

void P10000::Late_Update(Float timeDelta)
{
	
	
}

void P10000::Fixed_Update(Float fixedDelta)
{
	
}

HRESULT P10000::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	uint32 numMeshes = m_Model->Get_NumMeshes();
	for (uint32 i = 0; i < numMeshes; ++i)
	{
		// 텍스처 및 본 행렬 바인딩
		m_Model->Bind_Material(m_Shader, DiffuseMap, i, 1);
		m_Model->Bind_BoneMatrices(m_Shader, BoneMatrices, i);
		m_Shader->Begin(0); // 첫 번째 패스 사용
		m_Model->Render(i);
	}

	return S_OK;
}

void P10000::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

HRESULT P10000::Ready_Components()
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
	if (m_StateMachine)
	{
		m_StateMachine->Add_State(make_shared<State2B_Idle>(L"State2B_Idle", static_pointer_cast<P10000>(shared_from_this())));
		m_StateMachine->Add_State(make_shared<State2B_Walk>(L"State2B_Walk", static_pointer_cast<P10000>(shared_from_this())));
		m_StateMachine->Change_State(L"State2B_Idle");
	}

	return S_OK;
}

HRESULT P10000::Bind_ShaderResources()
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

	if (FAILED(m_Shader->Bind_RawValue("g_LightDir", &lightDesc->direction, sizeof(Float4))))
		return E_FAIL;
	if (FAILED(m_Shader->Bind_RawValue("g_LightDiffuse", &lightDesc->diffuse, sizeof(Float4))))
		return E_FAIL;
	if (FAILED(m_Shader->Bind_RawValue("g_LightAmbient", &lightDesc->ambient, sizeof(Float4))))
		return E_FAIL;
	if (FAILED(m_Shader->Bind_RawValue("g_LightSpecular", &lightDesc->specular, sizeof(Float4))))
		return E_FAIL;

	return S_OK;
}

Shared<P10000> P10000::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	Shared<P10000> pInstance = make_shared<P10000>(device, context);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		LOG_ERROR(L"Failed to Created : 2B");
		MSG_BOX("Failed to Created : 2B");
	}
	return pInstance;
}

Shared<GameObject> P10000::Clone(void* arg)
{
	Shared<P10000> pInstance = make_shared<P10000>(*this);

	if (FAILED(pInstance->Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Cloned : 2B");
		MSG_BOX("Failed to Cloned : 2B");
	}
	return pInstance;
}

}
