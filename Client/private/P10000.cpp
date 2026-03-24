#include "pch.h"
#include "P10000.h"

#include <Game.h>
#include <SpdLogger.h>

#include "Shader.h"
#include "Model.h"
#include "P10000Body.h"

#include "StateMachine.h"
#include "State2B_Idle.h"
#include "State2B_Jump.h"
#include "State2B_Run.h"
#include "State2B_Sprint.h"
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
	if (FAILED(ContainerObject::Initialize(arg))) {
		LOG_ERROR(L"Failed to Initialize GameObject {}", m_ObjectName);
		return E_FAIL;
	}

	if (FAILED(Ready_PartObjects())) {
		LOG_ERROR(L"Failed to Ready PartObjects {}", m_ObjectName);
		return E_FAIL;
	}
	if (FAILED(Ready_Components())) {
		LOG_ERROR(L"Failed to Ready Components {}", m_ObjectName);
		return E_FAIL;
	}

	return S_OK;
}

void P10000::On_Destroy()
{
	
	ContainerObject::On_Destroy();
}

void P10000::Priority_Update(Float timeDelta)
{
	Update_KeyInput(timeDelta);
	
}

void P10000::Update(Float timeDelta)
{
	


	m_P10000States->Update_State(timeDelta);
}

void P10000::Late_Update(Float timeDelta)
{
	/*
	 키 입력 IDLE -> RUN 
	 
	 */
	
}

void P10000::Fixed_Update(Float fixedDelta)
{
	
}

HRESULT P10000::Render()
{
	


	return S_OK;
}

void P10000::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

void P10000::Update_KeyInput(Float timeDelta)
{
	if (GetKeyState(VK_UP) & 0x8000)
	{
		if (m_States & P10000_STATE::IDLE_2B)
			m_States ^= P10000_STATE::IDLE_2B;

		m_States |= P10000_STATE::RUN_2B;
	}
	else
	{
		if (m_States & P10000_STATE::RUN_2B)
			m_States ^= P10000_STATE::RUN_2B;

		m_States |= P10000_STATE::IDLE_2B;
	}
}

HRESULT P10000::Ready_PartObjects()
{
	if (FAILED(Add_PartObject(ETOI(LEVEL::GAMEPLAY), L"P10000Body", L"P10000Body")))
		return E_FAIL;
	if (FAILED(Add_PartObject(ETOI(LEVEL::GAMEPLAY), L"WP0070Body", L"WP0070Body")))
		return E_FAIL;
	if (FAILED(Add_PartObject(ETOI(LEVEL::GAMEPLAY), L"WP0220Body", L"WP0220Body")))
		return E_FAIL;

	// POD

	return S_OK;
}

HRESULT P10000::Ready_Components()
{
	if ((m_P10000States = Add_Component<StateMachine>(ETOI(LEVEL::GAMEPLAY))))
	{
		
		auto p10000 = static_pointer_cast<P10000>(shared_from_this());
		m_P10000States->Add_State(State2B_Idle::Create(
			Helper::To_wString(magic_enum::enum_name(IDLE_2B)), p10000));
		m_P10000States->Add_State(State2B_Walk::Create(
			Helper::To_wString(magic_enum::enum_name(WALK_2B)), p10000));
		m_P10000States->Add_State(State2B_Run::Create(
			Helper::To_wString(magic_enum::enum_name(RUN_2B)), p10000));
		m_P10000States->Add_State(State2B_Sprint::Create(
			Helper::To_wString(magic_enum::enum_name(SPRINT_2B)), p10000));
		m_P10000States->Add_State(State2B_Jump::Create(
			Helper::To_wString(magic_enum::enum_name(JUMP_2B)), p10000));


		m_P10000States->Change_State(Helper::To_wString(magic_enum::enum_name(IDLE_2B)));
	}
	else
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
