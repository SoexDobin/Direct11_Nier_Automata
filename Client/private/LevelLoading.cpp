#include "pch.h"
#include "LevelLoading.h"

#include "Game.h"
#include "Terrain.h"
#include "Loader.h"
#include <SpdLogger.h>

LevelLoading::LevelLoading(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
	: Level { device, context }, m_Loader { nullptr }, m_NextLevel { LEVEL::LEVEL_END }
{
}

HRESULT LevelLoading::Initialize(void* arg)
{
	m_NextLevel = *static_cast<LEVEL*>(arg);
	
	m_Loader = Loader::Create(m_Device, m_Context, m_NextLevel);
	if (nullptr == m_Loader)
		return E_FAIL;

	return Level::Initialize(arg);
}

void LevelLoading::On_Destroy()
{

	Level::On_Destroy();
}

void LevelLoading::Update_Level(Float timeDelta)
{
	Bool a = m_Loader->Is_Finished();


	if (true == m_Loader->Is_Finished()
		&& GetKeyState(VK_RETURN) & 0x8000)
	{
		GAME_INSTANCE->Instantiate<Terrain>();

		if (4 >= ETOI(m_NextLevel))
		{
			MSG_BOX("Failed to Created : NextLevel");
			return;
		}
	}
	Level::Update_Level(timeDelta);
}

HRESULT LevelLoading::Render_Level()
{


	return Level::Render_Level();
}

HRESULT LevelLoading::Ready_BackGround()
{
#ifdef _DEBUG
	m_Loader->Print_LoadingText();
#endif

	return S_OK;
}

HRESULT LevelLoading::Ready_UI()
{


	return S_OK;
}

Unique<LevelLoading> LevelLoading::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context, LEVEL nextLevelID)
{
	auto loadingLevel = make_unique<LevelLoading>(device, context);

	if (FAILED(loadingLevel->Initialize(&nextLevelID))) {
		LOG_ERROR(L"Failed To Create LoadingBackground");
		return nullptr;
	}

	return loadingLevel;
}
