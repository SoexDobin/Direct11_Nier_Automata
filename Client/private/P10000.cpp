#include "pch.h"
#include "P10000.h"

#include <Game.h>
#include <SpdLogger.h>

#include "ClientSettingManager.h"
#include "Shader.h"
#include "Model.h"
#include "ThirdPersonCamera.h"

#include "StateMachine.h"
#include "State2B_Idle.h"
#include "State2B_Walk.h"

namespace Client {

P10000::P10000() : Playable {} {}
P10000::P10000(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Playable{ device, context } {}
P10000::P10000(const P10000& rhs)
	: Playable{ rhs } {}

HRESULT P10000::Initialize_Prototype()
{
	return Playable::Initialize_Prototype();
}

HRESULT P10000::Initialize(void* arg)
{
	if (FAILED(Playable::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize GameObject {}", m_ObjectName);
		return E_FAIL;
	}

	if (FAILED(Ready_PlayerCamera()))
	{
		LOG_ERROR(L"Failed to Ready Camera At {}", m_ObjectName);
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
	Playable::On_Destroy();
	m_StateMachine->On_Destroy();
}

void P10000::Priority_Update(Float timeDelta)
{
	Playable::Priority_Update(timeDelta);
}

void P10000::Update(Float timeDelta)
{
	Playable::Update(timeDelta);
	if (nullptr != m_Model)
		m_Model->Update_ModelAnimation(timeDelta);
	if (nullptr != m_StateMachine)
		m_StateMachine->Update_State(timeDelta);

	if (GAME_INSTANCE->Get_DIKeyState(DIKEYBOARD_DOWN))
		m_Model->Set_AnimationIndex(++m_tempIdx);
}

void P10000::Late_Update(Float timeDelta)
{
	Playable::Late_Update(timeDelta);
	
}

void P10000::Fixed_Update(Float fixedDelta)
{
	Playable::Fixed_Update(fixedDelta);
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

	return Playable::Render();
}

void P10000::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

HRESULT P10000::Ready_PlayerCamera()
{
	ThirdPersonCamera::THIRD_PERSON_CAMERA_DESC desc{};
	desc.eye = Vector4{ 0.f, 10.f, -10.f, 1.f };
	desc.at = Vector4{ 0.f, 0.f, 0.f, 1.f };
	desc.up = Vector4{ 0.f, 1.f, 0.f, 1.f };
	desc.fovY = XMConvertToRadians(60.f);
	desc.aspect = static_cast<Float>(ClientSettingManager::g_EngineDesc.viewportWidth) / static_cast<Float>(ClientSettingManager::g_EngineDesc.viewportHeight);
	desc.nearPlane = 0.1f;
	desc.farPlane = 500.f;

	desc.distance = 10.f;
	desc.minDistance = 3.f;
	desc.maxDistance = 30.f;
	desc.offset = Vector3{ 0.f, 1.f, 0.f };
	desc.mouseSensitive = 0.1f;
	desc.wheelSensitive = 0.01f;

	if (auto mainCamera = GAME_INSTANCE->Instantiate<ThirdPersonCamera>(L"ThirdPersonCamera", ETOI(0), &desc))
	{
		if (FAILED(GAME_INSTANCE->Set_MainCamera(mainCamera)))
		{
			return E_FAIL;
		}
		mainCamera->Set_Target(shared_from_this());
	}
		

	return S_OK;
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
