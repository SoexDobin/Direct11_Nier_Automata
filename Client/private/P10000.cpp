#include "pch.h"
#include "2B.h"
#include "StateMachine.h"
#include "PlayerStates.h"

C2B::C2B() : Playable{} {}

C2B::C2B(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Playable{device, context} {}

C2B::C2B(const C2B& rhs)
	: Playable{rhs} {}

HRESULT C2B::Initialize_Prototype()
{
	if (FAILED(Playable::Initialize_Prototype()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT C2B::Initialize(void* arg)
{
	if (FAILED(Playable::Initialize(arg)))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	// 상태 등록 예시
	auto pOwner = static_pointer_cast<C2B>(shared_from_this());
	m_pStateMachine->Add_State(L"IDLE", make_shared<C2B_Idle>(pOwner));
	m_pStateMachine->Add_State(L"MOVE", make_shared<C2B_Move>(pOwner));

	return S_OK;
}

void C2B::Priority_Update(Float timeDelta)
{
	Playable::Priority_Update(timeDelta);
}

void C2B::Update(Float timeDelta)
{
	Playable::Update(timeDelta);
}

void C2B::Late_Update(Float timeDelta)
{
	Playable::Late_Update(timeDelta);
}

void C2B::Fixed_Update(Float fixedDelta)
{
	Playable::Fixed_Update(fixedDelta);
}

HRESULT C2B::Render()
{
	if (FAILED(Playable::Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

Shared<GameObject> C2B::Clone(void* arg)
{
	auto pInstance = make_shared<C2B>(*this);
	if (FAILED(pInstance->Initialize(arg)))
	{
		return nullptr;
	}
	return pInstance;
}

HRESULT C2B::Add_Components()
{
	m_pStateMachine = Add_Component<StateMachine>();
	if (nullptr == m_pStateMachine)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT C2B::Bind_ShaderResources()
{
	return S_OK;
}

RTTR_REGISTRATION
{
	rttr::registration::class_<C2B>("C2B")
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr);
}
