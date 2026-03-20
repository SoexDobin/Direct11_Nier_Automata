#include "pch.h"
#include "LevelTitle.h"
#include <SpdLogger.h>
#include <Game.h>

#include "ClientSettingManager.h"
#include "LevelGamePlay.h"
#include "Loader.h"
#include "LoadingFade.h"

LevelTitle::LevelTitle(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Level { device, context } {}

HRESULT LevelTitle::Initialize(void* arg)
{
	if (FAILED(ClientSettingManager::GetInstance()->Load_LevelData(LEVEL::TITLE)))
		return E_FAIL;

	return Level::Initialize(arg);
}

void LevelTitle::On_Destroy()
{
	Level::On_Destroy();
}

void LevelTitle::Update_Level(Float timeDelta)
{
	if (GAME_INSTANCE->Get_DIKeyState(DIK_SPACE) & 0x80)
	{
		GAME_INSTANCE->Change_Level(ETOI(LEVEL::LOADING), LevelGamePlay::Create(m_Device, m_Context, LEVEL::GAMEPLAY));
	}

	Level::Update_Level(timeDelta);
}

HRESULT LevelTitle::Render_Level()
{
	return Level::Render_Level();
}

Shared<LevelTitle> LevelTitle::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, LEVEL nextLevelID)
{
	auto titleLevel = make_shared<LevelTitle>(device, context);

	if (FAILED(titleLevel->Initialize(&nextLevelID))) {
		LOG_ERROR(L"Failed To Create LoadingBackground");
		return nullptr;
	}

	return titleLevel;
}
