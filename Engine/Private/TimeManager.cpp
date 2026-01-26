#include  "TimeManager.h"
#include "Timer.h"

TimeManager::TimeManager()
{
}

HRESULT TimeManager::Initialize()
{

	return S_OK;
}

Float TimeManager::Update_Timer(const uint32& timerID)
{
	for (auto& timer : m_Timers)
	{
		
		if (Get_Timer(timerID) )

		if (timer.second->Is_Active())
			timer.second->Update_Timer();
	}

	// TODO : 프레임 반환
	return 0.0f;
}

Shared<Timer> TimeManager::Get_Timer(const uint32 objectID) const
{
	for (auto& timer : m_Timers)
	{
		if (timer.second->Get_ObjectID() == objectID)
			return timer.second;
	}

	MSG_BOX("There are no such Timer.");
	return nullptr;
}

Shared<Timer> TimeManager::Get_Timer(const wstring& name) const
{
	for (auto& timer : m_Timers)
	{
		if (timer.second)
			return timer.second;
	}

	MSG_BOX("There are no such Timer.");
	return nullptr;
}

HRESULT TimeManager::Add_Timer(const wstring& name)
{
	

	return S_OK;
}

Unique<TimeManager> TimeManager::Create()
{
	auto timeManager = std::make_unique<TimeManager>();

	if (FAILED(timeManager->Initialize()))
	{
		MSG_BOX("Failed To Create TimeManager");
		return nullptr;
	}

	return timeManager;
}
