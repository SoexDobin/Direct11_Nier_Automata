#include "pch.h"
#include "State2B_Jump.h"

#include <SpdLogger.h>

#include "Pl0000.h"
#include "Pl0000Body.h"
#include "Pl0000Movement.h"
#include "Pl0000StateMachine.h"

#include "Random_Helper.h"

State2B_Jump::State2B_Jump(const wstring& tag, const Shared<Pl0000>& owner)
	: State2B{tag, owner}
{
}

HRESULT State2B_Jump::Initialize()
{	
	m_EnterAnim.emplace(ETOI(Pl0000::PL0000_STATE::STAND_TO_JUMP));
	m_EnterAnim.emplace(ETOI(Pl0000::PL0000_STATE::RUN_TO_JUMP));
	m_EnterAnim.emplace(ETOI(Pl0000::PL0000_STATE::SPRINT_TO_JUMP));

	return State2B::Initialize();
}

Bool State2B_Jump::StateEnterInvoke()
{
	auto pl0000 = m_Body.lock();
	
	m_CanDoubleJump = true;
	JumpScalar = 15.f;
	m_Movement.lock()->Add_Force(Vector3(0.f, JumpScalar, 0.f));

	switch (auto prevState = m_States.lock()->Get_CurP10000State())
	{
	case Pl0000::PL0000_STATE::IDLE:
		pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::STAND_TO_JUMP), 0.2f, false);
		return true;
	case Pl0000::PL0000_STATE::RUN:
		pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::RUN_TO_JUMP), 0.2f, false);
		m_PrevMoveState = Pl0000::PL0000_STATE::RUN;
		return true;
	case Pl0000::PL0000_STATE::SPRINT: case Pl0000::PL0000_STATE::DASH:
		pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::SPRINT_TO_JUMP), 0.2f, false);
		m_PrevMoveState = Pl0000::PL0000_STATE::SPRINT;
		return true;
	default: 
		LOG_ERROR(L"[ENTER JUMP] : No Enter state PrevIndex {} ", Helper::To_wString(magic_enum::enum_name(prevState)));
		return false;
	}
}

void State2B_Jump::Update(Float timeDelta)
{
	Pl0000Movement::PL0000_MOVEMENT_DATA moveData{};
	moveData.direction = Calculate_Direction();
	moveData.isMove = (moveData.direction.LengthSquared() > 0.f);
	moveData.isJump = true;
	moveData.canRotation = true;
	moveData.useRootMotionDir = false;


	m_Movement.lock()->Set_MovementData(moveData);

	auto pl0000 = m_Body.lock();
	uint32 curIndex = pl0000->Get_CurrentAnimationIndex();
	Bool isAnimFinished = pl0000->Get_ModelComponent()->Is_AnimationFinished();

	// 점프 상관 없이 무조건 땅 닿으면 끝
	if (m_Movement.lock()->Is_Grounded())
	{
		if (m_Input.lock()->Is_WASD_NoneOrUp())
		{
			m_States.lock()->Change_State(Pl0000::PL0000_STATE::IDLE);
			return;
		}
		if (m_PrevMoveState == Pl0000::PL0000_STATE::SPRINT)
		{
			m_States.lock()->Change_State(Pl0000::PL0000_STATE::SPRINT);
			return;
		}
		if (m_Input.lock()->Is_WASD_SingleClickHold(0.5f) || m_Input.lock()->Is_WASD_Press())
		{
			m_States.lock()->Change_State(Pl0000::PL0000_STATE::RUN);
			return;
		}
	}

	// 더블 점프
	if ((m_CanDoubleJump && m_Input.lock()->Is_KeyDown(UBYTE(DIKEYBOARD_SPACE)))
		|| m_CanDoubleJump && m_Input.lock()->Is_KeyMultiClick(UBYTE(DIKEYBOARD_SPACE)))
	{
		m_Movement.lock()->Add_Force(Vector3(0.f, JumpScalar, 0.f));
		if (false == m_Input.lock()->Is_NoneOrUp(UBYTE(DIKEYBOARD_S)))
		{
			pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::DOUBLE_JUMP1), 0.2f, false);
		}
		else
		{
			uint32 doubleJumpIndex = Helper::Random_Int(ETOI(Pl0000::PL0000_STATE::DOUBLE_JUMP2), ETOI(Pl0000::PL0000_STATE::DOUBLE_JUMP4));
			pl0000->Set_Animation(doubleJumpIndex, 0.2f, false);
		}
		m_CanDoubleJump = false;    // 한 번만 가능하도록
	}

	if (m_EnterAnim.contains(curIndex) && isAnimFinished)
	{
		pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::JUMP_ENTER), 0.2f, false);
	}
	else if (isAnimFinished && !m_Movement.lock()->Is_Grounded())
	{
		pl0000->Set_Animation(ETOI(Pl0000::PL0000_STATE::JUMP_HOLD), 0.2f, true);
	}
}

void State2B_Jump::Late_Update(Float timeDelta)
{
	
}

void State2B_Jump::StateExitInvoke()
{
	
}

Shared<State2B_Jump> State2B_Jump::Create(const wstring& tag, const Shared<Pl0000>& owner)
{
	auto instance = make_shared<State2B_Jump>(tag, owner);

	if (FAILED(instance->Initialize()))
	{
		LOG_ERROR(L"Failed to Create State2B_Jump");
		return nullptr;
	}

	return instance;
}


