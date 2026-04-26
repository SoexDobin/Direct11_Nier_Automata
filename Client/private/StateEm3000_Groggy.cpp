#include "pch.h"
#include "StateEm3000_Groggy.h"

#include "Em3000.h"
#include "Em3000Body.h"
#include "Em3000StateMachine.h"
#include "ExplodeEffect.h"
#include <Game.h>

StateEm3000_Groggy::StateEm3000_Groggy(const wstring& tag, const Shared<Em3000>& owner)
	: StateEm3000{tag, owner} {}
HRESULT StateEm3000_Groggy::Initialize()
{
	return StateEm3000::Initialize();
}

using em3000State = Em3000::EM3000_STATE;

Bool StateEm3000_Groggy::StateEnterInvoke()
{
	m_Body.lock()->Set_Animation(ETOI(em3000State::GROGGY_IN), 0.1f, false);

	return true;
}

void StateEm3000_Groggy::Update(Float timeDelta)
{
	auto em3000Body = m_Body.lock();
	Bool isFinished = m_Body.lock()->Is_AnimationFinished();
	uint32 animIndex = m_Body.lock()->Get_CurrentAnimationIndex();

	if (animIndex == ETOI(em3000State::GROGGY_LOOP_1))
		m_GroggyElapsed += timeDelta;

	if (isFinished && animIndex == ETOI(em3000State::GROGGY_END))
	{
		auto owner = m_Owner.lock();
		uint32 levIndex = GAME_INSTANCE->Get_TargetLevelIndex();

		ExplodeEffect::EXPLODE_EFFECT_DESC effectDesc{};
		effectDesc.position = owner->Get_Transform()->Get_Position();
		effectDesc.scale = Vector3(20.f, 20.f, 20.f);
		effectDesc.textureTag = L"Effect_Explode2";
		effectDesc.threshold = 0.05f;
		GAME_INSTANCE->Instantiate<ExplodeEffect>(L"ExplodeEffect", levIndex, &effectDesc);
		effectDesc.position = effectDesc.position + Vector3{ 0.02f, 0.02f, 0.02f };
		effectDesc.scale = Vector3(15.f, 15.f, 15.f);
		GAME_INSTANCE->Instantiate<ExplodeEffect>(L"ExplodeEffect", levIndex, &effectDesc);
		effectDesc.position = effectDesc.position + Vector3{ 0.01f, 0.01f, 0.01f };
		effectDesc.scale = Vector3(17.5f, 17.5f, 17.5f);
		GAME_INSTANCE->Instantiate<ExplodeEffect>(L"ExplodeEffect", levIndex, &effectDesc);

		GAME_INSTANCE->PlaySoundFXOnce(L"Explode1", SOUNDCHANNEL::CHANNEL_27, 0.4f);

		const wstring partTags[] = { L"Em3001", L"Em3002", L"Em3003" };
		for (const auto& tag : partTags)
		{
			if (auto part = owner->Find_PartObject(tag))
			{
				Object::Destroy(part);
			}
		}

		m_States.lock()->Change_State(em3000State::PHASE2_TRANSFORM);
	}

	if (m_GroggyElapsed >= m_GroggyTime)
	{
		em3000Body->Set_Animation(ETOI(em3000State::GROGGY_END), 0.1f, false);
		return;
	}

	if (isFinished && ETOI(em3000State::GROGGY_IN) == animIndex)
	{
		em3000Body->Set_Animation(ETOI(em3000State::GROGGY_LOOP_1), 0.1f, true);
	}
}

void StateEm3000_Groggy::Late_Update(Float timeDelta)
{

}

void StateEm3000_Groggy::StateExitInvoke()
{

}

Shared<StateEm3000_Groggy> StateEm3000_Groggy::Create(const wstring& tag, const Shared<Em3000>& owner)
{
	auto prototype = make_shared<StateEm3000_Groggy>(tag, owner);

	if (FAILED(prototype->Initialize()))
	{
		MSG_BOX("Failed to Created : StateEm3000_Groggy");
		return nullptr;
	}

	return prototype;
}