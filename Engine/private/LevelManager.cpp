#include "LevelManager.h"
#include "Level.h"
#include "Game.h"

HRESULT LevelManager::Initialize(const Shared<void>& arg)
{
	return EngineManager::Initialize(arg);
}

void LevelManager::On_Destroy()
{
	m_CurrentLevel.reset();

}

HRESULT LevelManager::Change_Level(uint32 levIndex, const Shared<Level>& level)
{
	if (nullptr != m_CurrentLevel)
		Game::GetInstance()->Clear_Resource(m_CurrentLevelIndex);

	m_CurrentLevel = level;
	m_CurrentLevelIndex = levIndex;

	return S_OK;
}

void LevelManager::Update(Float timeDelta)
{
	if (m_CurrentLevel == nullptr) return;

	m_CurrentLevel->Update_Level(timeDelta);
}

HRESULT LevelManager::Render()
{
	if (m_CurrentLevel == nullptr) return E_FAIL;

	return m_CurrentLevel->Render_Level();
}

Unique<LevelManager> LevelManager::Create()
{
	return make_unique<LevelManager>();
}
