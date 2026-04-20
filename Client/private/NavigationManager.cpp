#include "pch.h"
#include "NavigationManager.h"

#include <Game.h>
#include <Navigation.h>
#include <SpdLogger.h>

#include "GameObject.h"
#include "LevelGamePlay2.h"
#include "LoadingFadeIn.h"
#include "NavigationSector.h"
#include "TriggerObject.h"

IMPLEMENT_SINGLETON(NavigationManager)

NavigationManager::~NavigationManager()
{

}

void NavigationManager::Reset_NavigationSettingFlag()
{
	m_InitialFinished = false;
}

HRESULT NavigationManager::Setting_NavigationSector(LEVEL level)
{
	if (m_InitialFinished) return S_OK;

	if (level == LEVEL::GAMEPLAY)
	{
		m_InitialFinished = true;
		return 	Setting_GamePlay();
	}
	else if (level == LEVEL::GAMEPLAY2)
	{
		m_InitialFinished = true;
		return 	Setting_GamePlay2();
	}


	return E_FAIL;
}

HRESULT NavigationManager::UpdateNextSceneTrigger()
{
	Client::TriggerObject::TRIGGER_DESC triggerDesc;
	triggerDesc.position = Engine::Vector3(100.f, 0.f, 150.f);	// 목표 월드 포지션 좌표
	triggerDesc.radius = 15.f;									// 충돌(인식) 범위 반경
	triggerDesc.levelIndex = ETOI(LEVEL::GAMEPLAY2);
	
	triggerDesc.callbacks =
	{
		[levelIndex = triggerDesc.levelIndex]()
		{
			LoadingFadeIn::FADE_IN_DESC fadeDesc;
			fadeDesc.fadeSpeed = 0.5f;
			
			auto fadeUI = GAME_INSTANCE->Instantiate<LoadingFadeIn>(L"LoadingFadeIn", levelIndex, &fadeDesc);

			// 페이드 연출이 끝날 때까지 틱마다가 감시해야 하므로 Add_Permanent_Event로 체크하고 해제합니다.
			std::wstring observerTag = L"Event_WaitFadeAndChangeScene";
			GAME_INSTANCE->Add_Permanent_Event(levelIndex, observerTag, [fadeUI, levelIndex, observerTag]()
			{
					// FADEIN 모션이 전부 끝났는지 여부 확인
					if (fadeUI->Is_FadeFinished())
					{
						// 현재 루프를 돌고 있는 자신(영구 이벤트)을 제거하여 더 이상 감시되지 않도록 함
						if (SUCCEEDED(GAME_INSTANCE->Remove_Event(levelIndex, observerTag)))
						{
							Shared<Level> amusementParkLevel = LevelGamePlay2::Create(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context());
							GAME_INSTANCE->Change_Level(ETOI(LEVEL::LOADING), amusementParkLevel);
						}
						else
						{
							LOG_CRITICAL(L"Failed to Change Level By Trigger!");
						}
					}
				});
			}
	};
	

	Shared<TriggerObject> triggerObject = GAME_INSTANCE->Instantiate<Client::TriggerObject>(L"Prototype_TriggerObject", ETOI(LEVEL::GAMEPLAY2), &triggerDesc);

	if (triggerObject == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT NavigationManager::Setting_GamePlay()
{
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

	NavigationSector::NAVIGATION_COLLISION_DESC centralSectorDesc{};
	centralSectorDesc.navTag = L"CityOfRuinCentral";
	centralSectorDesc.worldPosition = Vector3{ 360.f, 0.f, 98.5f };
	centralSectorDesc.collisionExtends = Vector3{ 100.f, 50.f, 90.f };
	auto centralSector = GAME_INSTANCE->Instantiate<NavigationSector>(L"NavigationSector", ETOI(LEVEL::GAMEPLAY), &centralSectorDesc);

	NavigationSector::NAVIGATION_COLLISION_DESC manHoleDesc{};
	manHoleDesc.navTag = L"CityOfRuinManHole";
	manHoleDesc.worldPosition = Vector3{ 510.f, 0.f, 183.f };
	manHoleDesc.collisionExtends = Vector3{ 65.f, 50.f, 65.f };
	auto manHoleSector = GAME_INSTANCE->Instantiate<NavigationSector>(L"NavigationSector", ETOI(LEVEL::GAMEPLAY), &manHoleDesc);

	return S_OK;
}

HRESULT NavigationManager::Setting_GamePlay2()
{
	NavigationSector::NAVIGATION_COLLISION_DESC parkEntrySectorDesc{};
	parkEntrySectorDesc.navTag = L"AmusementParkEntry";
	parkEntrySectorDesc.worldPosition = Vector3{ 0.f, 0.f, 86.5f };
	parkEntrySectorDesc.collisionExtends = Vector3{ 80.f, 50.f, 120.f };
	auto parkEntrySector = GAME_INSTANCE->Instantiate<NavigationSector>(L"NavigationSector", ETOI(LEVEL::GAMEPLAY2), &parkEntrySectorDesc);

	NavigationSector::NAVIGATION_COLLISION_DESC parkMiddleSectorDesc{};
	parkMiddleSectorDesc.navTag = L"AmusementParkMiddle";
	parkMiddleSectorDesc.worldPosition = Vector3{ 151.f, 0.f, 0.f };
	parkMiddleSectorDesc.collisionExtends = Vector3{ 74.f, 50.f, 80.f };
	auto parkMiddleSector = GAME_INSTANCE->Instantiate<NavigationSector>(L"NavigationSector", ETOI(LEVEL::GAMEPLAY2), &parkMiddleSectorDesc);

	NavigationSector::NAVIGATION_COLLISION_DESC parkDomeSectorDesc{};
	parkDomeSectorDesc.navTag = L"AmusementParkDome";
	parkDomeSectorDesc.worldPosition = Vector3{ 151.f, 0.f, 0.f };
	parkDomeSectorDesc.collisionExtends = Vector3{ 74.f, 50.f, 80.f };
	auto parkDomeSector = GAME_INSTANCE->Instantiate<NavigationSector>(L"NavigationSector", ETOI(LEVEL::GAMEPLAY2), &parkDomeSectorDesc);

	return S_OK;
}
