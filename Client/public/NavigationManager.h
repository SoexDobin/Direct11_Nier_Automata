#pragma once
#include <Object.h>

NS_BEGIN(Engine)
	class Navigation;
NS_END

NS_BEGIN(Client)

class NavigationManager 
{
	DECLARE_SINGLETON(NavigationManager) 
public:
	explicit NavigationManager() = default;
	~NavigationManager();

public:
	void Reset_NavigationSettingFlag();
	HRESULT Setting_NavigationSector(LEVEL level);
	HRESULT UpdateNextSceneTrigger();

private:
	HRESULT Setting_GamePlay();
	HRESULT Setting_GamePlay2();

	Bool m_InitialFinished{ false };

};

NS_END