#include "pch.h"
#include "WorldTriggerManager.h"

#include <Game.h>
#include <Level.h>
#include <SpdLogger.h>

#include "LevelGamePlay2.h"
#include "LevelLoading.h"
#include "LoadingFadeIn.h"
#include "TriggerObject.h"

IMPLEMENT_SINGLETON(WorldTriggerManager)

WorldTriggerManager::~WorldTriggerManager()
{
}

void WorldTriggerManager::Reset_NavigationSettingFlag()
{
	m_InitialFinished = false;
}

HRESULT WorldTriggerManager::Setting_Trigger(LEVEL level)
{
	if (m_InitialFinished) return S_OK;

	if (level == LEVEL::GAMEPLAY)
	{
		m_InitialFinished = true;
		return 	Setting_GamePlay();
	}
	if (level == LEVEL::GAMEPLAY2)
	{
		m_InitialFinished = true;
		return 	Setting_GamePlay2();
	}

	return E_FAIL;
}



HRESULT WorldTriggerManager::Setting_GamePlay()
{
	TriggerObject::TRIGGER_DESC triggerDesc;
	triggerDesc.targetLayerName = L"PlayerPhysical";
	triggerDesc.position = Vector3(497.f, -16.f, 183.f);
	triggerDesc.radius = 15.f;
	triggerDesc.levelIndex = ETOI(LEVEL::GAMEPLAY);

	triggerDesc.callbacks =
	{
		[levelIndex = triggerDesc.levelIndex]()
		{
			LoadingFadeIn::FADE_IN_DESC fadeDesc;
			fadeDesc.fadeSpeed = 1.5f;

			auto fadeUI = GAME_INSTANCE->Instantiate<LoadingFadeIn>(L"LoadingFadeIn", levelIndex, &fadeDesc);
			fadeUI->Set_Active(true);

			auto skyBox = GAME_INSTANCE->Find_ObjectByObjectTag(levelIndex, L"SkyBox");
			if (skyBox)
				Object::Destroy(skyBox);

			wstring observerTag = L"Event_WaitFadeAndChangeScene";
			GAME_INSTANCE->Add_Permanent_Event(levelIndex, observerTag, [fadeUI, levelIndex, observerTag]()
				{
					if (fadeUI->Is_FadeFinished())
					{
						if (SUCCEEDED(GAME_INSTANCE->Remove_Event(levelIndex, observerTag)))
						{
							LevelLoading::LEVEL_LOADING_DESC  loadingDesc{};
							loadingDesc.loadStatic = false;
							loadingDesc.nextLevelID = LEVEL::GAMEPLAY2;
							GAME_INSTANCE->Change_Level(ETOI(LEVEL::LOADING), LevelLoading::Create(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context(), 
								loadingDesc.nextLevelID, loadingDesc.loadStatic));
						}
						else
						{
							LOG_CRITICAL(L"Failed to Change Level By Trigger!");
						}
					}
				});
			}
	};

	Shared<TriggerObject> triggerObject = GAME_INSTANCE->Instantiate<TriggerObject>(L"TriggerObject", ETOI(LEVEL::GAMEPLAY), &triggerDesc);

	if (triggerObject == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT WorldTriggerManager::Setting_GamePlay2()
{
	return S_OK;
}
