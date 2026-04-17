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
	if (level == LEVEL::GAMEPLAY)
	{
		return 	Setting_GamePlay();
	}


	return E_FAIL;
}

HRESULT NavigationManager::Setting_GamePlay()
{
	Navigation::NAVIGATION_DESC desc{ };

	auto bridge = GAME_INSTANCE->Instantiate<Navigation>(L"CityOfRuinBridge", ETOI(LEVEL::STATIC), &desc);
	NavigationSector::NAVIGATION_COLLISION_DESC bridgeSectorDesc{};
	bridgeSectorDesc.TargetCollider = bridge;
	bridgeSectorDesc.worldPosition = Vector3::Zero;
	bridgeSectorDesc.collisionExtends = Vector3::Zero;
	auto bridgeSector = GAME_INSTANCE->Instantiate<NavigationSector>(L"NavigationSector", ETOI(LEVEL::GAMEPLAY), &bridgeSectorDesc);
	
	NavigationSector::NAVIGATION_COLLISION_DESC entrySectorDesc{};
	auto entry = GAME_INSTANCE->Instantiate<Navigation>(L"CityOfRuinEntry", ETOI(LEVEL::STATIC), &desc);
	entrySectorDesc.TargetCollider = entry;
	entrySectorDesc.worldPosition = Vector3::Zero;
	entrySectorDesc.collisionExtends = Vector3::Zero;
	auto entrySector = GAME_INSTANCE->Instantiate<NavigationSector>(L"NavigationSector", ETOI(LEVEL::GAMEPLAY), &entrySectorDesc);

	return S_OK;
}
