#pragma once
#include <Object.h>

NS_BEGIN(Engine)
class Navigation;
NS_END

NS_BEGIN(Client)

class WorldTriggerManager
{
	DECLARE_SINGLETON(WorldTriggerManager)
public:
	explicit WorldTriggerManager() = default;
	~WorldTriggerManager();

public:
	void Reset_NavigationSettingFlag();
	HRESULT Setting_Trigger(LEVEL level);

private:
	HRESULT Setting_GamePlay();
	HRESULT Setting_GamePlay2();

	Bool m_InitialFinished{ false };

};

NS_END