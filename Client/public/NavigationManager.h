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
	HRESULT Setting_NavigationSector(LEVEL level);

private:
	HRESULT Setting_GamePlay();

};

NS_END