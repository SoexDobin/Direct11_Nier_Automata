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
  void Update(Bool isResize) override {}
  void Render(Bool isResize) override;

private:
	void Render_Properties(rttr::property prop, rttr::instance instance);

private:
	void GameObjectGUI(const Shared<Engine::GameObject> &pObj);
	void GameObjectPropertiesGUI(const Shared<GameObject> &pObj);
	void ComponentGUI(const string &label,
                    const Shared<Engine::Component> &pComp);

public:
  static Shared<Inspector> Create();
};

NS_END