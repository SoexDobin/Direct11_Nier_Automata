#include "pch.h"
#include "StateEm3000_Melee.h"

#include "Em3000.h"
#include "Em3000Body.h"
#include "Em3000StateMachine.h"

StateEm3000_Melee::StateEm3000_Melee(const wstring& tag, const Shared<Em3000>& owner)
	: StateEm3000(tag, owner) {}

HRESULT StateEm3000_Melee::Initialize()
{
	return StateEm3000::Initialize();
}

using em3000state = Em3000::EM3000_STATE;

Bool StateEm3000_Melee::StateEnterInvoke()
{
	auto em3000Body = m_Body.lock();
	auto state = m_States.lock();

	if (Has_FrontMeleeTarget())
	{
		em3000Body->Set_Animation(ETOI(em3000state::FRONT_STOMP), 0.1f, false);
		return true;
	}
	else
	{
		em3000Body->Set_Animation(ETOI(em3000state::MIXER_START), 0.1f, false);
		return true;
	}
}

void StateEm3000_Melee::Update(Float timeDelta)
{
	auto em3000 = m_Owner.lock();
	auto em3000Body = m_Body.lock();
	auto state = m_States.lock();
	auto movement = m_Movement.lock();

	uint32 animIndex = em3000Body->Get_CurrentAnimationIndex();
	Bool IsFinished = em3000Body->Is_AnimationFinished();

	if (IsFinished && animIndex == ETOI(em3000state::FRONT_STOMP))
	{
		state->Change_State(em3000state::IDLE);
		return;
	}

	if (IsFinished && animIndex == ETOI(em3000state::MIXER_START))
	{
		em3000Body->Set_Animation(ETOI(em3000state::MIXING), 0.1f, true); 
		m_OriginalPosition = em3000->Get_Transform()->Get_Position();
		m_MixerTime = 0.f;
		
		// TODO : 애니메이션 state가 아닌 실제 객체가 맵을 한바퀴돌게
	}
	else if (animIndex == ETOI(em3000state::MIXING))
	{
		m_MixerTime += timeDelta;
		Float rotationSpeed = 10.f;
		Float radius = 5.f;

		Vector3 nextPos = m_OriginalPosition;
		nextPos.x += cosf(m_MixerTime * rotationSpeed) * radius;
		nextPos.z += sinf(m_MixerTime * rotationSpeed) * radius;

		em3000->Get_Transform()->Set_Position(nextPos);

		if (m_MixerTime >= m_MixerDuration)
		{
			m_MixerTime = 0.f;
			em3000Body->Set_Animation(ETOI(em3000state::MIXER_END), 0.1f, false);
			return;
		}
		// TODO : else if 가 MIXING 이면서 한바퀴 돌고 나서 원래 위치로 돌아왔을때
	}
	else if (animIndex == ETOI(em3000state::MIXER_END))
	{
		m_MixerTime += timeDelta;
		// END 애니메이션 재생 시간 대비 진행도 (0.0 ~ 1.0)
		// (만약 애니메이션 길이를 모르면 대략 1.0f 초 등으로 고정)
		Float progress = m_MixerTime / 1.f;
		if (progress > 1.f) progress = 1.f;
		
		Vector3 currentPos = em3000->Get_Transform()->Get_Position();
		Vector3 lerpPos = Vector3::Lerp(currentPos, m_OriginalPosition, progress * timeDelta * 5.f);

		em3000->Get_Transform()->Set_Position(lerpPos);
		
		if (IsFinished)
		{
			em3000->Get_Transform()->Set_Position(m_OriginalPosition);
			state->Change_State(em3000state::IDLE);
		}
	}

}

void StateEm3000_Melee::Late_Update(Float timeDelta)
{
	
}

void StateEm3000_Melee::StateExitInvoke()
{
	
}

Shared<StateEm3000_Melee> StateEm3000_Melee::Create(const wstring& tag, const Shared<Em3000>& owner)
{
	auto prototype = make_shared<StateEm3000_Melee>(tag, owner);

	if (FAILED(prototype->Initialize()))
	{
		MSG_BOX("Failed to Created : StateEm3000_Melee");
		return nullptr;
	}

	return prototype;
}

