#include "pch.h"
#include "Em3000State_Intro.h"

#include <Game.h>

#include "Em3000.h"
#include "Em3000Body.h"
#include "Em3000StateMachine.h"

StateEm3000_Intro::StateEm3000_Intro(const wstring& tag, const Shared<Em3000>& owner)
	: StateEm3000{ tag, owner } {}

HRESULT StateEm3000_Intro::Initialize()
{
	return StateEm3000::Initialize();
}

Bool StateEm3000_Intro::StateEnterInvoke()
{
	return StateEm3000::StateEnterInvoke();
}

void StateEm3000_Intro::Update(Float timeDelta)
{
	if (false == m_Owner.lock()->Has_Target()) return;

	if (m_Owner.lock()->Has_Target() && m_IsIntroStart ==  false)
	{
		m_Body.lock()->Set_AnimationSpeed(2.f);
		m_IsIntroStart = true;

		m_Body.lock()->Set_Animation(ETOI(Em3000::EM3000_STATE::INTRO), 0.1f, false);
		GAME_INSTANCE->StopSound(SOUNDCHANNEL::CHANNEL_1);
		GAME_INSTANCE->PlaySoundLoopSection(L"ABeautifulSong", SOUNDCHANNEL::CHANNEL_1, 0.3f, 18000, 137000,true);
	}

	uint32 animIndex = m_Body.lock()->Get_CurrentAnimationIndex();
	Bool isFinished = m_Body.lock()->Is_AnimationFinished();
	Float progress = m_Body.lock()->Get_AnimationProgress();

	if (animIndex == ETOI(Em3000::EM3000_STATE::INTRO) && isFinished)
	{
		m_States.lock()->Change_State(Em3000::EM3000_STATE::IDLE);
		m_Body.lock()->Set_AnimationSpeed(1.f);
	}
}

void StateEm3000_Intro::Late_Update(Float timeDelta)
{
	
}

void StateEm3000_Intro::StateExitInvoke()
{
	
}

Shared<StateEm3000_Intro> StateEm3000_Intro::Create(const wstring& tag, const Shared<Em3000>& owner)
{
	auto prototype = make_shared<StateEm3000_Intro>(tag, owner);

	if (FAILED(prototype->Initialize()))
	{
		MSG_BOX("Failed to Created : StateEm3000_Intro");
		return nullptr;
	}

	return prototype;
}

