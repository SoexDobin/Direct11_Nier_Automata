#include "pch.h"
#include "State2B_Idle.h"
#include "P10000Body.h"
#include <Game.h>
#include <SpdLogger.h>

#include "Model.h"
#include "StateMachine.h"
#include "P10000.h"
#include "WP0070Body.h"
#include "WP0220Body.h"

State2B_Idle::State2B_Idle(const wstring& tag, const Shared<P10000>& owner)
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
	
	auto body = static_pointer_cast<P10000Body>(m_Owner.lock()->Find_PartObject(L"P10000Body"));
	if (body) {
		
		body->Set_Animation(46, 0.25f, true);
	}

	// 2. 무기들도 Idle 상태에 맞는 애니메이션으로 동기화
	//auto lightWeapon = static_pointer_cast<WP0070Body>(m_Owner.lock()->Find_PartObject(L"WP0070Body"));
	//if (lightWeapon) {
	//	lightWeapon->Get_Model()->Set_Animation(0, 0.2f); // 무기 Idle 애니메이션
	//	lightWeapon->Get_Model()->Set_AnimLoop(true);
	//}
	//
	//auto heavyWeapon = static_pointer_cast<WP0220Body>(m_Owner.lock()->Find_PartObject(L"WP0220Body"));
	//if (heavyWeapon) {
	//	heavyWeapon->Get_Model()->Set_Animation(0, 0.2f); // 무기 Idle 애니메이션
	//	heavyWeapon->Get_Model()->Set_AnimLoop(true);
	//}

	return true;
}

void State2B_Idle::Update(Float timeDelta)
{

}

void State2B_Idle::Late_Update(Float timeDelta)
{
}

void State2B_Idle::StateExitInvoke()
{
}

Shared<State2B_Idle> State2B_Idle::Create(const wstring& tag, const Shared<P10000>& owner)
{
	auto instance = make_shared<State2B_Idle>(tag, owner);

	if (FAILED(instance->Initialize()))
	{
		LOG_ERROR(L"Failed to Create State2B_Idle");
		return nullptr;
	}

	return instance;
}
