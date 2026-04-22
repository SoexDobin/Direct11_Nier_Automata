#pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)

class Timer;

class TimeManager final
{
	NO_COPY(TimeManager)
public:
	explicit TimeManager() = default;
	~TimeManager();

public:
	HRESULT Initialize();
	Float Update_Timers() const;

	Shared<Timer> Get_MainTimer() const;
	Shared<Timer> Get_Timer(const wstring& key) const;
	HRESULT Add_Timer(const wstring& key);
	HRESULT Remove_Timer(const wstring& key);

	Bool Has_FixedUpdate();
	Bool Is_FixedUpdate() const { return m_HasFixedUpdate; }

	void Set_TargetFPS(uint32 targetFPS) const;
	void Set_MainTimer_MaxDelta(Float maxDelta) const;
	void Set_MainTimer_MaxDeltaByFPS(uint32 minFPS) const;

private:
	Shared<Timer> m_MainTimer = { nullptr };
	unordered_map<wstring, Shared<Timer>> m_Timers;
	Bool m_HasFixedUpdate = { false };

public:
	static Unique<TimeManager> Create();
};

NS_END