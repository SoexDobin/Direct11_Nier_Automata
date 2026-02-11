#pragma once

#include "Engine_Define.h"

NS_BEGIN(Engine)
class Game;
NS_END

NS_BEGIN(Editor)

class EditorApp final {
public:
  explicit EditorApp();
  ~EditorApp();

public:
  HRESULT Initialize();
  void Update();
  HRESULT Render();

private:
  void RenderDualView();

  Shared<Engine::Game> m_Game = {nullptr};
  Bool m_PlayMode = {false};

public:
  static Unique<EditorApp> Create();
};

NS_END
