#include "pch.h"
#include "State2B_Dash.h"

#include <Game.h>
#include <SpdLogger.h>

#include "Pl0000.h"
#include "Pl0000Body.h"
#include "WP0070Body.h"
#include "WP0220Body.h"
#include "Pl0000Input.h"
#include "Pl0000StateMachine.h"
#include "Model.h"
#include "Pl0000Movement.h"

State2B_Dash::State2B_Dash(const wstring& tag, const Shared<Pl0000>& owner)
	: State2B{tag, owner}
{
}

HRESULT State2B_Dash::Initialize()
{
	m_EnterAnim.emplace(ETOI(Pl0000::PL0000_STATE::STAND_TO_DASH_F));
	m_EnterAnim.emplace(ETOI(Pl0000::PL0000_STATE::STAND_TO_DASH_B));
	m_EnterAnim.emplace(ETOI(Pl0000::PL0000_STATE::STAND_TO_DASH_R));
	m_EnterAnim.emplace(ETOI(Pl0000::PL0000_STATE::STAND_TO_DASH_L));

	m_DashAnim.emplace(ETOI(Pl0000::PL0000_STATE::DASH_F));
	m_DashAnim.emplace(ETOI(Pl0000::PL0000_STATE::DASH_B));
	m_DashAnim.emplace(ETOI(Pl0000::PL0000_STATE::DASH_R));
	m_DashAnim.emplace(ETOI(Pl0000::PL0000_STATE::DASH_L));

	return State2B::Initialize();
}

Bool State2B_Dash::StateEnterInvoke()
{
	auto pl0000 = m_Body.lock();
	auto input = m_Input.lock();
	
	m_OriginalFov = m_MainCamera.lock()->Get_FovY();

	if (input->Is_KeyPress(UBYTE(DIKEYBOARD_W))) {
		m_DashDir = DASH_DIR::FRONT;
		pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::STAND_TO_DASH_F), 0.15f, false);
	}
	else if (input->Is_KeyPress(UBYTE(DIKEYBOARD_S))) {
		m_DashDir = DASH_DIR::BACK;
		pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::STAND_TO_DASH_B), 0.15f, false);
	}
	else if (input->Is_KeyPress(UBYTE(DIKEYBOARD_D))) {
		m_DashDir = DASH_DIR::RIGHT;
		pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::STAND_TO_DASH_R), 0.15f, false);
	}
	else if (input->Is_KeyPress(UBYTE(DIKEYBOARD_A))) {
		m_DashDir = DASH_DIR::LEFT;
		pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::STAND_TO_DASH_L), 0.15f, false);
	}
	else {
		LOG_ERROR(L"Wrong Input At DashState Enter");
		return false;
	}
	/* (이전 시간에 구성한 MovementData 방향 연산 및 주입은 필요시 Update나 여기서 1회 수행) */

	return true;

}

void State2B_Dash::Update(Float timeDelta)
{
	auto pl0000 = m_Body.lock();
	auto input = m_Input.lock();
	uint32 curIndex = pl0000->Get_CurrentAnimationIndex();
	Bool isAnimFinished = pl0000->Is_AnimationFinished();
	Float dashProgress = pl0000->Get_AnimationProgress();

	Pl0000Movement::PL0000_MOVEMENT_DATA moveData{};
	moveData.direction = Calculate_Direction();
	moveData.isMove = (moveData.direction.LengthSquared() > 0.f);

	if (m_DashDir != DASH_DIR::FRONT)
	{
		Vector3 camLook = m_MainCamera.lock()->Get_Transform()->Get_Look();
		camLook.y = 0.f; // 위아래로 기울지 않도록 XZ평면으로 납작하게 투영
		camLook.Normalize();
		moveData.lookDirection = camLook;
	}
	
	moveData.useRootMotionDir = false;
	m_Movement.lock()->Set_MovementData(moveData);

	if (input->Is_KeyDown(UBYTE(DIKEYBOARD_SPACE)))
	{
		m_States.lock()->Change_State(Pl0000::PL0000_STATE::JUMP);
		return;
	}

	if (input->Is_WASD_NoneOrUp())
	{
		m_States.lock()->Change_State(Pl0000::PL0000_STATE::IDLE);
		return;
	}

	if (m_EnterAnim.contains(curIndex) && dashProgress >= 0.7f)
	{
		switch (m_DashDir) {
		case DASH_DIR::FRONT: 
			pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::DASH_F), 0.5f, false); 
			break;
		case DASH_DIR::BACK:  
			pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::DASH_B), 0.5f, false); 
			break;
		case DASH_DIR::RIGHT: 
			pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::DASH_R), 0.5f, false); 
			break;
		case DASH_DIR::LEFT:  
			pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::DASH_L), 0.5f, false); 
			break;
		case DASH_DIR::NONE:
			LOG_ERROR(L"Wrong Dash Direction");
			break;
		}
	}
	else if (m_DashAnim.contains(curIndex) && isAnimFinished)
	{
		if (!input->Is_WASD_NoneOrUp())
		{
			m_States.lock()->Change_State(Pl0000::PL0000_STATE::SPRINT);
			return;
		}
		m_States.lock()->Change_State(Pl0000::PL0000_STATE::IDLE);
	}
}

void State2B_Dash::Late_Update(Float timeDelta)
{
	Float dashProgress = m_Body.lock()->Get_AnimationProgress();

	Float extraFovRadian = sinf(dashProgress * XM_PI) * XMConvertToRadians(8.f);
	m_MainCamera.lock()->Set_FovY(m_OriginalFov + extraFovRadian);
}

void State2B_Dash::StateExitInvoke()
{
	if (m_OriginalFov > 0.f)
	{
		m_MainCamera.lock()->Set_FovY(m_OriginalFov);
		m_OriginalFov = 0.f;
	}
}

Shared<State2B_Dash> State2B_Dash::Create(const wstring& tag, const Shared<Pl0000>& owner)
{
	auto instance = make_shared<State2B_Dash>(tag, owner);

	if (FAILED(instance->Initialize()))
	{
		MSG_BOX("Failed to Created : State2B_Dash");
		return nullptr;
	}

	return instance;
}
