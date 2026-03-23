#include "pch.h"
#include "State2B_Idle.h"
#include "P10000Body.h"
#include <Game.h>
#include <SpdLogger.h>

#include "Model.h"
#include "StateMachine.h"

State2B_Idle::State2B_Idle(const wstring& tag, const Shared<P10000Body>& owner)
	: State2B{tag, owner}
{
}

HRESULT State2B_Idle::Initialize()
{
	return S_OK;
}

Bool State2B_Idle::StateEnterInvoke()
{
	if (m_Owner.expired())
		return false;

	//Shared<Model> model = m_Owner.lock()->Get_Model();
	//if (model)
	//{
	//	int32 animIndex = model->Get_AnimationIndexByName(L"pl0000|pl0000_0000");
	//	if (animIndex != -1)
	//	{
	//		model->Set_AnimationIndex(animIndex);
	//		model->Set_AnimLoop(true);
	//	}
	//}

	return true;
}

void State2B_Idle::Update(Float timeDelta)
{
	if (GAME_INSTANCE->Get_DIKeyState(DIK_W) & 0x80 ||
		GAME_INSTANCE->Get_DIKeyState(DIK_A) & 0x80 ||
		GAME_INSTANCE->Get_DIKeyState(DIK_S) & 0x80 ||
		GAME_INSTANCE->Get_DIKeyState(DIK_D) & 0x80)
	{
		//m_Owner.lock()->Get_StateMachine()->Change_State(L"State2B_Walk");
		return;
	}
}

void State2B_Idle::Late_Update(Float timeDelta)
{
}

void State2B_Idle::StateExitInvoke()
{
}

Shared<State2B_Idle> State2B_Idle::Create(const wstring& tag, const Shared<P10000Body>& owner)
{
	auto instance = make_shared<State2B_Idle>(tag, owner);

	if (FAILED(instance->Initialize()))
	{
		LOG_ERROR(L"Failed to Create State2B_Idle");
		return nullptr;
	}

	return instance;
}
