#pragma once
#include "EngineManager.h"
#include "Level.h"

NS_BEGIN(Engine)



class LevelManager final : public EngineManager
{
	NO_COPY(LevelManager)
public:
	LevelManager() = default;
	~LevelManager() override = default;

public:
	uint32 Get_CurrentLevelIndex() const { return m_CurrentLevelIndex; }

public:
	HRESULT Initialize(const Shared<void>& arg) override;
	void On_Destroy() override;

public:
	HRESULT		Change_Level(uint32 levIndex, const Shared<Level>& level);
	void		Update(Float timeDelta);
	HRESULT		Render();

private:
	Shared<Level>	m_CurrentLevel = { nullptr };
	uint32			m_CurrentLevelIndex = {};

public:
	static Unique<LevelManager> Create();
};

NS_END