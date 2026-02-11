#pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)
class Game;
NS_END

NS_BEGIN(Editor)

class Editor_Function {
public:
  static void Render_DualView(Shared<Engine::Game> pGame, bool &bPlayMode);
  static void Render_ProjectSettings(Shared<Engine::Game> pGame);
};

NS_END
