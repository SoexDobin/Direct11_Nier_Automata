#include "pch.h"
#include "Em3000State_Intro.h"

#include <Game.h>

#include "AmusementParkLight.h"
#include "CinematicCamera.h"
#include "Em3000.h"
#include "Em3000Body.h"
#include "Em3000StateMachine.h"

StateEm3000_Intro::StateEm3000_Intro(const wstring& tag, const Shared<Em3000>& owner)
	: StateEm3000{ tag, owner } {}

HRESULT StateEm3000_Intro::Initialize()
{
	CinematicCamera::CINEMATIC_CAMERA_DESC cineDesc{};
	cineDesc.startPosition = Vector3(396.f, 28.f, 52.f);
	cineDesc.targetPosition = Vector3(386.f, 28.f, 52.f);
	cineDesc.startRotation = Vector3(0.f, 1.5f, 0.f);
	cineDesc.targetRotation = Vector3(0.f, 1.5f, 0.f);
	cineDesc.startFov = 60.f;
	cineDesc.targetFov = 80.f;
	
	cineDesc.startWaitTime = 3.5f;
	cineDesc.travelTime = 3.75f; 
	cineDesc.returnTime = 0.75f;  

	cineDesc.nearPlane = 0.1f;
	cineDesc.farPlane = 250.f;
	cineDesc.aspect = GAME_INSTANCE->Get_ViewportDesc().Width / GAME_INSTANCE->Get_ViewportDesc().Height;
	
	m_CinematicCamera = GAME_INSTANCE->Instantiate<CinematicCamera>(L"CinematicCamera", ETOI(LEVEL::STATIC), &cineDesc);

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
		m_CinematicCamera->InvokeCinematic();
	}

	uint32 animIndex = m_Body.lock()->Get_CurrentAnimationIndex();
	Bool isFinished = m_Body.lock()->Is_AnimationFinished();
	

	if (animIndex == ETOI(Em3000::EM3000_STATE::INTRO) && isFinished)
	{
		m_States.lock()->Change_State(Em3000::EM3000_STATE::IDLE);
		m_Body.lock()->Set_AnimationSpeed(1.f);

		uint32 levIndex = GAME_INSTANCE->Get_TargetLevelIndex();

		AmusementParkLight::AMUSEMENT_LIGHT_DESC lightDesc{};
		lightDesc.target = m_Owner.lock();         
		lightDesc.offset = Vector3(0.f, 7.f, 0.f); 

		lightDesc.lightDesc.type = LIGHT::POINT;
		lightDesc.lightDesc.range = 9.f;
		lightDesc.lightDesc.diffuse = Vector4(1.0f, 0.8f, 0.6f, 1.0f); // 따뜻한 빛
		lightDesc.lightDesc.ambient = Vector4(0.2f, 0.2f, 0.2f, 1.0f);
		lightDesc.lightDesc.specular = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

		auto light = GAME_INSTANCE->Instantiate<AmusementParkLight>(
			L"AmusementParkLight",
			GAME_INSTANCE->Get_CurrentLevelIndex(),
			&lightDesc);

		m_Owner.lock()->Set_IntroLight(light);


		lightDesc.target = m_Owner.lock()->Get_Target();
		lightDesc.offset = lightDesc.offset = Vector3(0.f, 2.5f, 0.f); 
		lightDesc.lightDesc.range = 5.f;

		GAME_INSTANCE->Instantiate<AmusementParkLight>(
			L"AmusementParkLight",
			GAME_INSTANCE->Get_CurrentLevelIndex(),
			&lightDesc);

		GAME_INSTANCE->PlaySoundFXOnce(L"SpotLight", SOUNDCHANNEL::CHANNEL_32, 0.6f);
		GAME_INSTANCE->PlaySoundFXOnce(L"Alert", SOUNDCHANNEL::CHANNEL_31, 0.75f);
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

