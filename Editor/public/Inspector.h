#pragma once
#include "EditorObject.h"

NS_BEGIN(Engine)
class GameObject;
class Component;
NS_END

NS_BEGIN(Editor)

class PrefabRegistry;

class Inspector final : public EditorObject {
  NO_COPY(Inspector)
public:
  Inspector();
  ~Inspector() override;

public:
  HRESULT Initialize() override;
  void Update() override {}
  void Render() override;

private:
  void LayerTagGUI();
  void GameObjectGUI(const Shared<Engine::GameObject> &pObj);
  void ComponentGUI(const string &label,
                    const Shared<Engine::Component> &pComp);

public:
  static Shared<Inspector> Create();
};

NS_END