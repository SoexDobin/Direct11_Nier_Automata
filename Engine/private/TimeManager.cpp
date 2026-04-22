#include "TimeManager.h"
#include "Timer.h"


TimeManager::~TimeManager()
{
	m_Timers.clear();
}


HRESULT TimeManager::Initialize()
{
	m_MainTimer = Timer::Create();
	m_HasFixedUpdate = true;

	return S_OK;
}

Float TimeManager::Update_Timers() const
{
	for (const auto& timer : m_Timers)
	{
		if (timer.second->IsActive())
			timer.second->Update_Timer();
	}

	return m_MainTimer->Update_Timer();
}

Shared<Timer> TimeManager::Get_MainTimer() const
{
	if (m_MainTimer == nullptr)
	{
		MSG_BOX("Failed To Get MainTimer");
		return nullptr;
	}

	return m_MainTimer;
}

Shared<Timer> TimeManager::Get_Timer(const wstring& key) const
{
	if (!m_Timers.contains(key))
	{
		MSG_BOX("There are no such Timer.");
		return nullptr;
	}
	
	return m_Timers.at(key);
}

HRESULT TimeManager::Add_Timer(const wstring& key)
{
	if (m_Timers.contains(key))
	{
		MSG_BOX("Already Exists Timer Key");
		return E_FAIL;
	}

	m_Timers.emplace(key, Timer::Create());

	return S_OK;
}

HRESULT TimeManager::Remove_Timer(const wstring& key)
{
	if (!m_Timers.contains(key))
	{
		MSG_BOX("There are no such Timer.");
		return E_FAIL;
	}

	m_Timers.erase(key);
	return S_OK;
}

Bool TimeManager::Has_FixedUpdate()
{
	Bool hasFixedUpdate = m_MainTimer->IsFixedUpdate();
	m_HasFixedUpdate = hasFixedUpdate;
	return hasFixedUpdate;
}

void TimeManager::Set_TargetFPS(uint32 targetFPS) const
{
	if (m_MainTimer)
	{
		m_MainTimer->Set_TargetFPS(targetFPS);
	}
}

void TimeManager::Set_MainTimer_MaxDelta(Float maxDelta) const
{
	if (m_MainTimer)
	{
		m_MainTimer->Set_MaxDeltaTime(maxDelta);
	}
}

void TimeManager::Set_MainTimer_MaxDeltaByFPS(uint32 minFPS) const
{
	if (m_MainTimer)
	{
		m_MainTimer->Set_MaxDeltaTimeByFPS(minFPS);
	}
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
