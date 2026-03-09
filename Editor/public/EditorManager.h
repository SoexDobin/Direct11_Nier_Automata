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
public:
    Bool Is_ResizeView() const { return m_IsResizeView; }
    Vector3 Get_ResizeInfo() { return Vector3{ (Float)m_NewWidth, (Float)m_NewHeight, (Float)m_TargetIdx }; }
    void RequestResize(uint32 width, uint32 height, uint32 screenIndex)
    {
        m_IsResizeView = true;
        m_NewWidth = width; m_NewHeight = height; m_TargetIdx = screenIndex;
    }
    void Clear_ResizeRequest() { m_IsResizeView = false; }

public: /* Selected Object (Hierarchy <-> Inspector 공유) */
  Shared<Engine::GameObject> Get_SelectedObject() const {
    return m_SelectedObject.lock();
  }
  void Set_SelectedObject(const Shared<Engine::GameObject> &obj) {
    m_SelectedObject = obj;
  }
  void Clear_SelectedObject() { m_SelectedObject.reset(); }

private:
    Bool m_IsResizeView{ false };
    uint32 m_NewWidth, m_NewHeight, m_TargetIdx;

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