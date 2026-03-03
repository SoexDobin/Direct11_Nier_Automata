#pragma once
#include "EditorCamera.h"

NS_BEGIN(Engine)
class Camera;
class GameObject;
NS_END

NS_BEGIN(Editor)

class Inspector;
class EditorView;
class MenuBar;
class PrefabTab;
class LogConsole;
class Hierarchy;

class EditorCamera;

class EditorManager {
  DECLARE_SINGLETON(EditorManager)
public:
  EditorManager();
  ~EditorManager();

public:
  HRESULT Initialize();
  void Update();
  HRESULT Render();

public:
  EDITOR_STATE Get_State() const { return m_State; }
  void Set_State(EDITOR_STATE state) { m_State = state; }

public:
  Shared<EditorCamera> Get_EditorCamera() const { return m_EditorCamera; }
  Shared<Engine::Camera> Get_InGameCamera() const { return m_InGameCamera; }
  void Set_InGameCamera(const Shared<Engine::Camera> &camera) {
    m_InGameCamera = camera;
  }

public: /* Selected Object (Hierarchy <-> Inspector 공유) */
  Shared<Engine::GameObject> Get_SelectedObject() const {
    return m_SelectedObject.lock();
  }
  void Set_SelectedObject(const Shared<Engine::GameObject> &obj) {
    m_SelectedObject = obj;
  }
  void Clear_SelectedObject() { m_SelectedObject.reset(); }

private:
  EDITOR_STATE m_State = EDITOR_STATE::STOP;
  Shared<EditorCamera> m_EditorCamera{nullptr};
  Shared<Engine::Camera> m_InGameCamera{nullptr};
  Weak<Engine::GameObject> m_SelectedObject = {};

private:
  Shared<Inspector> m_Inspector = {nullptr};
  Shared<EditorView> m_EditorView = {nullptr};
  Shared<MenuBar> m_MenuBar = {nullptr};
  Shared<PrefabTab> m_PrefabTab = {nullptr};
  Shared<LogConsole> m_LogConsole = {nullptr};
  Shared<Hierarchy> m_Hierarchy = {nullptr};
};

NS_END