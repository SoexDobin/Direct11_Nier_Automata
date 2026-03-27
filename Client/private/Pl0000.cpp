#include "pch.h"
#include "Pl0000.h"

#include <Game.h>
#include <SpdLogger.h>

#include "Shader.h"
#include "Model.h"
#include "Pl0000Body.h"
#include "Pl0000StateMachine.h"

#include "StateMachine.h"
#include "State2B_Idle.h"
#include "State2B_Jump.h"
#include "State2B_Run.h"
#include "State2B_Sprint.h"
#include "State2B_Walk.h"
#include "State2B_Dash.h"

namespace Client {

Pl0000::Pl0000() : ContainerObject{} {}
Pl0000::Pl0000(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: ContainerObject{ device, context } {}
Pl0000::Pl0000(const Pl0000& rhs)
	: ContainerObject{ rhs } {}

HRESULT Pl0000::Initialize_Prototype()
{
	return ContainerObject::Initialize_Prototype();
}

HRESULT Pl0000::Initialize(void* arg)
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

void Pl0000::On_Destroy()
{
	
	ContainerObject::On_Destroy();
}

void Pl0000::Priority_Update(Float timeDelta)
{
	m_Pl0000Input->Update_P10000_InputState(timeDelta);
}

void Pl0000::Update(Float timeDelta)
{
	m_Pl0000States->Update_State(timeDelta);
}

void Pl0000::Late_Update(Float timeDelta)
{
	


}

void Pl0000::Fixed_Update(Float fixedDelta)
{
	m_Transform->Update_WorldMatrix();
}

HRESULT Pl0000::Render()
{
	return S_OK;
}

void Pl0000::Submit_RenderGroup()
{
	GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::NONBLEND, shared_from_this());
}

HRESULT Pl0000::Ready_PartObjects()
{
	Pl0000Body::Pl0000BODY_DESC desc{};
	desc.parentMatrix = m_Transform->Get_WorldMatrixPtr();

	if (FAILED(Add_PartObject(ETOI(LEVEL::GAMEPLAY), L"Pl0000Body", L"Pl0000Body", &desc)))
		return E_FAIL;
	if (FAILED(Add_PartObject(ETOI(LEVEL::GAMEPLAY), L"WP0070Body", L"WP0070Body", &desc)))
		return E_FAIL;
	if (FAILED(Add_PartObject(ETOI(LEVEL::GAMEPLAY), L"WP0220Body", L"WP0220Body", &desc)))
		return E_FAIL;
	// TODO : POD

	return S_OK;
}

HRESULT Pl0000::Ready_Components()
{
	m_Pl0000Input = Add_Component<Pl0000Input>(ETOI(LEVEL::GAMEPLAY));
	if (nullptr == m_Pl0000Input)
		return E_FAIL;


	// StateMachine은 마지막에 처리
	if ((m_Pl0000States = Add_Component<Pl0000StateMachine>(ETOI(LEVEL::GAMEPLAY))))
	{
		auto pl0000 = static_pointer_cast<Pl0000>(shared_from_this());

		if (FAILED(m_Pl0000States->Add_State(State2B_Idle::Create(
			Helper::To_wString(magic_enum::enum_name(IDLE)), pl0000))))
			return E_FAIL;
		if (FAILED(m_Pl0000States->Add_State(State2B_Run::Create(
			Helper::To_wString(magic_enum::enum_name(RUN)), pl0000))))
			return E_FAIL;
		if (FAILED(m_Pl0000States->Add_State(State2B_Sprint::Create(
			Helper::To_wString(magic_enum::enum_name(SPRINT)), pl0000))))
			return E_FAIL;
		if (FAILED(m_Pl0000States->Add_State(State2B_Jump::Create(
			Helper::To_wString(magic_enum::enum_name(JUMP)), pl0000))))
			return E_FAIL;
		if (FAILED(m_Pl0000States->Add_State(State2B_Dash::Create(
			Helper::To_wString(magic_enum::enum_name(DASH)), pl0000))))
			return E_FAIL;

		m_Pl0000States->Change_State(IDLE);
	}
	else
		return E_FAIL;



	return S_OK;
}

Shared<Pl0000> Pl0000::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
{
	Shared<Pl0000> pInstance = make_shared<Pl0000>(device, context);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		LOG_ERROR(L"Failed to Created : 2B");
		MSG_BOX("Failed to Created : 2B");
	}
	return pInstance;
}

Shared<GameObject> Pl0000::Clone(void* arg)
{
	Shared<Pl0000> pInstance = make_shared<Pl0000>(*this);

	if (FAILED(pInstance->Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Cloned : 2B");
		MSG_BOX("Failed to Cloned : 2B");
	}
	return pInstance;
}

}
