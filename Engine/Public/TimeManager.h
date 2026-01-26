#pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)

class Timer;

class ENGINE_DLL TimeManager final 
{
public:
	TimeManager();
	~TimeManager() = default;

public:
	HRESULT Initialize();
	Float Update_Timer(const uint32 &timerID);

	Shared<Timer> Get_Timer(uint32 objectID) const;
	Shared<Timer> Get_Timer(const wstring& name) const;
	HRESULT Add_Timer(const wstring& name);

private:
	Shared<Timer> m_MainTimer = { nullptr };
	unordered_map<uint32, Shared<Timer>> m_Timers;

public:
	static Unique<TimeManager> Create();
};

NS_END
