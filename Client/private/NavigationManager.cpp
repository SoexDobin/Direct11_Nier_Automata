#include "pch.h"
#include "NavigationManager.h"

#include <Game.h>
#include <Navigation.h>

#include "GameObject.h"
#include "NavigationSector.h"

IMPLEMENT_SINGLETON(NavigationManager)

NavigationManager::~NavigationManager()
{

}

HRESULT NavigationManager::Setting_NavigationSector(LEVEL level)
{
	if (m_InitialFinished) return S_OK;

	if (level == LEVEL::GAMEPLAY)
	{
		m_InitialFinished = true;
		return 	Setting_GamePlay();
	}


	return E_FAIL;
}

HRESULT NavigationManager::Setting_GamePlay()
{
	Navigation::NAVIGATION_DESC desc{ };

	NavigationSector::NAVIGATION_COLLISION_DESC bridgeSectorDesc{};
	bridgeSectorDesc.navTag = L"CityOfRuinBridge";
	bridgeSectorDesc.worldPosition = Vector3{ 235.f, 20.f, 120.f};
	bridgeSectorDesc.collisionExtends = Vector3{ 40.f, 50.f, 50.f};
	auto bridgeSector = GAME_INSTANCE->Instantiate<NavigationSector>(L"NavigationSector", ETOI(LEVEL::GAMEPLAY), &bridgeSectorDesc);
	
	NavigationSector::NAVIGATION_COLLISION_DESC entrySectorDesc{};
	entrySectorDesc.navTag = L"CityOfRuinEntry";
	entrySectorDesc.worldPosition = Vector3{ 90.f, 20.f, 60.f };
	entrySectorDesc.collisionExtends = Vector3{ 100.f, 50.f, 70.f };
	auto entrySector = GAME_INSTANCE->Instantiate<NavigationSector>(L"NavigationSector", ETOI(LEVEL::GAMEPLAY), &entrySectorDesc);

	return S_OK;
}
