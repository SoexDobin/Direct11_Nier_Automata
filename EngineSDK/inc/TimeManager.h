#pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)

class Timer;

class TimeManager final
{
	NO_COPY(TimeManager)
public:
	explicit TimeManager() = default;
	~TimeManager() = default;

public:
	HRESULT Initialize();
	Float Update_Timers();

	Shared<Timer> Get_MainTimer() const;
	Shared<Timer> Get_Timer(const wstring& key) const;
	HRESULT Add_Timer(const wstring& key);
	HRESULT Remove_Timer(const wstring& key);

private:
	Shared<Timer> m_MainTimer = { nullptr };
	unordered_map<wstring, Shared<Timer>> m_Timers;

public:
	static Unique<TimeManager> Create();
};

NS_END