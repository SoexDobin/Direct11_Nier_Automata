#pragma once
#include "ClientApp.h"

NS_BEGIN(Engine)
class Game;
NS_END

NS_BEGIN(Editor)

class EditorManager;

class EditorApp final {
public:
  EditorApp();
  ~EditorApp();

public:
  HRESULT Initialize();
  void Update();
  HRESULT Render();

private:
  HRESULT Initialize_IMGUI(const ENGINE_DESC &desc);
  HRESULT Destruct_IMGUI();

private:
  void Reset_ClientApp();
  void Change_ClientLevel(uint32 levIndex);

private:
	uint32 m_StartLevel{};
	uint32 m_StartLevelIndex{};
	Bool m_IsReset = { false };
	Unique<ClientApp> m_ClientApp = {nullptr};
	uint32 m_Phase5GateStage{};
	HRESULT m_Phase5GateInitialization{ S_OK };
	Bool m_Phase5GateExecuted{};

public:
  static Unique<EditorApp> Create();
};

NS_END
