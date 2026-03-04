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

private:
	Bool m_IsReset = { false };
	Shared<Game> m_Game = { nullptr };
	Unique<ClientApp> m_ClientApp = {nullptr};
	ENGINE_DESC m_EngineDesc = {};

public:
  static Unique<EditorApp> Create();
};

NS_END
