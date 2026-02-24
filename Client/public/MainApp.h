#pragma once
#include "Client_Define.h"
#include "Engine_Define.h"


NS_BEGIN(Engine)
class Game;
NS_END

NS_BEGIN(Client)

class MainApp final {
public:
  explicit MainApp();
  ~MainApp();

public:
  HRESULT Initialize();
  void Update() const;
  HRESULT Render() const;

private:
  Shared<Engine::Game> m_Game = {nullptr};

private:
  HRESULT Ready_StartLevel(LEVEL startLevel);

public:
  static Unique<MainApp> Create();
};

NS_END