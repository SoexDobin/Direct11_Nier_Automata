#include "pch.h"
#include "Em3000State_Groogy2.h"

#include <Game.h>

#include "Em3000.h"
#include "Em3000Body.h"
#include "Em3000StateMachine.h"

Em3000State_Groogy2::Em3000State_Groogy2(const wstring& tag, const Shared<Em3000>& owner)
	: StateEm3000{ tag, owner }
{
}

HRESULT Em3000State_Groogy2::Initialize()
{
	return StateEm3000::Initialize();
}

Bool Em3000State_Groogy2::StateEnterInvoke()
{

	if (m_States.lock()->Get_CurEm3000State() == Em3000::EM3000_STATE::PHASE2_GROGGY) return false;
 
	m_Body.lock()->Set_Animation(ETOI(Em3000::EM3000_STATE::PHASE2_IDLE), 0.1f, true);
	m_GroggyDelta = 0.f;

	m_Owner.lock()->Get_IntroLight()->Set_Diffuse(Vector4{ 4.f, 4.f, 102.f, 255.f });
	GAME_INSTANCE->PlaySoundFXOnce(L"SpotLight", SOUNDCHANNEL::CHANNEL_32, 0.5f);

	return true;
}

void Em3000State_Groogy2::Update(Float timeDelta)
{
	if (m_GroggyDelta >= m_Phase2GroggyDeltaTime)
	{
		m_States.lock()->Change_State(Em3000::EM3000_STATE::PHASE2_PUPPET);
	}
}

void Em3000State_Groogy2::Late_Update(Float timeDelta)
{
	m_GroggyDelta += timeDelta;
}

void Em3000State_Groogy2::StateExitInvoke()
{
	m_GroggyDelta = 0.f;
	m_Owner.lock()->Get_IntroLight()->Set_Diffuse(Vector4{ 102.f, 4.f, 4.f, 255.f });
	GAME_INSTANCE->PlaySoundFXOnce(L"SpotLight", SOUNDCHANNEL::CHANNEL_32, 0.5f);
}

Shared<Em3000State_Groogy2> Em3000State_Groogy2::Create(const wstring& tag, const Shared<Em3000>& owner)
{
	auto prototype = make_shared<Em3000State_Groogy2>(tag, owner);

	if (FAILED(prototype->Initialize()))
	{
		MSG_BOX("Failed to Created : Em3000State_Groogy2");
		return nullptr;
	}

	return prototype;
}
