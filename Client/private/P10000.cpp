#include "pch.h"
#include "P10000.h"

#include <Game.h>
#include <SpdLogger.h>

#include "Shader.h"
#include "Model.h"
#include "P10000Body.h"
#include "P10000StateMachine.h"

#include "StateMachine.h"
#include "State2B_Idle.h"
#include "State2B_Jump.h"
#include "State2B_Run.h"
#include "State2B_Sprint.h"
#include "State2B_Walk.h"
#include "State2B_Dash.h"

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
	m_P10000Input->Update_P10000_InputState(timeDelta);
}

void P10000::Update(Float timeDelta)
{
	auto curEnum = m_P10000States->Get_CurP10000State();

	// --- 이동 입력 판별 ---
	Bool isMoveInput = m_P10000Input->Is_WASD_Press();
	Bool isSprintHold = m_P10000Input->Is_WASD_Hold(0.5f);
	Bool isDash = m_P10000Input->Is_WASD_DoubleClick();

}

void P10000::Late_Update(Float timeDelta)
{
	m_P10000States->Update_State(timeDelta);

	m_Transform->Update_WorldMatrix();
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

HRESULT P10000::Ready_PartObjects()
{
	P10000Body::P10000BODY_DESC desc{};
	desc.parentMatrix = m_Transform->Get_WorldMatrixPtr();

	if (FAILED(Add_PartObject(ETOI(LEVEL::GAMEPLAY), L"P10000Body", L"P10000Body", &desc)))
		return E_FAIL;
	if (FAILED(Add_PartObject(ETOI(LEVEL::GAMEPLAY), L"WP0070Body", L"WP0070Body", &desc)))
		return E_FAIL;
	if (FAILED(Add_PartObject(ETOI(LEVEL::GAMEPLAY), L"WP0220Body", L"WP0220Body", &desc)))
		return E_FAIL;
	// TODO : POD

	return S_OK;
}

HRESULT P10000::Ready_Components()
{
	m_P10000Input = Add_Component<P10000Input>(ETOI(LEVEL::GAMEPLAY));
	if (nullptr == m_P10000Input)
		return E_FAIL;


	// StateMachine은 마지막에 처리
	if ((m_P10000States = Add_Component<P10000StateMachine>(ETOI(LEVEL::GAMEPLAY))))
	{
		auto p10000 = static_pointer_cast<P10000>(shared_from_this());

		if (FAILED(m_P10000States->Add_State(State2B_Idle::Create(
			Helper::To_wString(magic_enum::enum_name(IDLE)), p10000))))
			return E_FAIL;
		if (FAILED(m_P10000States->Add_State(State2B_Run::Create(
			Helper::To_wString(magic_enum::enum_name(RUN)), p10000))))
			return E_FAIL;
		if (FAILED(m_P10000States->Add_State(State2B_Sprint::Create(
			Helper::To_wString(magic_enum::enum_name(SPRINT)), p10000))))
			return E_FAIL;
		if (FAILED(m_P10000States->Add_State(State2B_Jump::Create(
			Helper::To_wString(magic_enum::enum_name(JUMP)), p10000))))
			return E_FAIL;
		if (FAILED(m_P10000States->Add_State(State2B_Dash::Create(
			Helper::To_wString(magic_enum::enum_name(DASH)), p10000))))
			return E_FAIL;

		m_P10000States->Change_State(IDLE);
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
