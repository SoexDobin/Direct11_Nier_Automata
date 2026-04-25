#pragma once
#include "Em3000.h"
#include "StateEm3000.h"

NS_BEGIN(Client)

class CLIENT_DLL StateEm3000_Range final : public StateEm3000
{
public:
	explicit StateEm3000_Range(const wstring& tag, const Shared<Em3000>& owner);
	~StateEm3000_Range() override = default;

private:
	HRESULT Initialize() override;

public:
	Bool StateEnterInvoke() override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void StateExitInvoke() override;

private:
	uint32 m_EntryAnimIndex{};

	uint32 m_RangeEnter[3] = {
		ETOI(Em3000::EM3000_STATE::DANMAK_START_1),
		ETOI(Em3000::EM3000_STATE::DANMAK_START_2),
		ETOI(Em3000::EM3000_STATE::DANMAK_START_3)
	};
	uint32 m_RangeAttack[3] = {
		ETOI(Em3000::EM3000_STATE::DANMAK_LOOP_1),
		ETOI(Em3000::EM3000_STATE::DANMAK_LOOP_2),
		ETOI(Em3000::EM3000_STATE::DANMAK_LOOP_3)
	};
	uint32 m_RangeEnd[3] = {
		ETOI(Em3000::EM3000_STATE::DANMAK_END_1),
		ETOI(Em3000::EM3000_STATE::DANMAK_END_2),
		ETOI(Em3000::EM3000_STATE::DANMAK_END_3)
	};

	unordered_set<uint32> m_EnterAnim;
	unordered_set<uint32> m_AttackAnim;
	unordered_set<uint32> m_EndAnim;

public:
	static Shared<StateEm3000_Range> Create(const wstring& tag, const Shared<Em3000>& owner);
};



NS_END
