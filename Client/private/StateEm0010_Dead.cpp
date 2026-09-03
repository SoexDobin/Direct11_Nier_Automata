#include "pch.h"
#include "StateEm0010_Dead.h"

#include <Game.h>

#include "Em0010.h"
#include "Random_Helper.h"

StateEm0010_Dead::StateEm0010_Dead(const wstring& tag, const Shared<Em0010>& owner)
	: StateEm0010{ tag, owner } {}
StateEm0010_Dead::~StateEm0010_Dead()
{
}

HRESULT StateEm0010_Dead::Initialize()
{
	using em0010Anim = Em0010::EM0010_STATE;

	m_DeadAnim.emplace(ETOI(em0010Anim::DEAD_1));
	m_DeadAnim.emplace(ETOI(em0010Anim::DEAD_2));
	m_DeadAnim.emplace(ETOI(em0010Anim::DEAD_3));
	m_DeadAnim.emplace(ETOI(em0010Anim::BACK_DROP_3));

	return StateEm0010::Initialize();
}

Bool StateEm0010_Dead::StateEnterInvoke()
{
	using em0010Anim = Em0010::EM0010_STATE;
	auto& dmgInfo = m_Owner.lock()->Get_LastDamageInfo();

	uint32 animIndex{ 0 };
	if (dmgInfo.attackType == ATK_TYPE::HEAVY)
	{
		animIndex = ETOI(em0010Anim::BACK_DROP_1);
	}
	else
	{
		int32 rand = Helper::Random_Int(0, 2);
		uint32 arr[3] = {
			ETOI(em0010Anim::DEAD_1),
			ETOI(em0010Anim::DEAD_2),
			ETOI(em0010Anim::DEAD_3),
		};

		animIndex = arr[rand];
	}

	m_Body.lock()->Set_Animation(animIndex, 0.2f, false);

	return true;
}

void StateEm0010_Dead::Update(Float timeDelta)
{
	using em0010Anim = Em0010::EM0010_STATE;
	auto em0010 = m_Body.lock();
	uint32 animIndex = em0010->Get_CurrentAnimationIndex();
	Bool isFinished = em0010->Is_AnimationFinished();
	Float progress = em0010->Get_AnimationProgress();

	if (m_DeadAnim.contains(animIndex) && isFinished)
	{
		if (const Shared<Em0010> owner = m_Owner.lock())
			GAME_INSTANCE->Destroy(owner->Get_ObjectGuid());
	}
	else if (animIndex == ETOI(em0010Anim::BACK_DROP_1) && progress >= 0.75f)
	{
		em0010->Set_Animation(ETOI(em0010Anim::BACK_DROP_3), 0.2f, false);
	}
}

void StateEm0010_Dead::Late_Update(Float timeDelta)
{

}

void StateEm0010_Dead::StateExitInvoke()
{

}

Shared<StateEm0010_Dead> StateEm0010_Dead::Create(const wstring& tag, const Shared<Em0010>& owner)
{
	auto instance = make_shared<StateEm0010_Dead>(tag, owner);

	if (FAILED(instance->Initialize()))
	{
		MSG_BOX("Failed to Clone : StateEm0010_Dead");
		return nullptr;
	}

	return instance;
}
