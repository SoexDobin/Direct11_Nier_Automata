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
class AssetBrowser;
class LogConsole;
class Hierarchy;
class ModelViewer;

class EditorCamera;

class EditorManager {
  DECLARE_SINGLETON(EditorManager)
public:
    typedef struct tagResizeInfo
    {
        Float width{};
        Float height{};
        uint32 screenIndex{};
    } RESIZE_INFO;
public:
  EditorManager();
  ~EditorManager();

public:
  HRESULT Initialize();
  void Update(Bool IsResetView);
  HRESULT Render(Bool IsResetView);

public:
  EDITOR_STATE Get_State() const { return m_State; }
  void Set_State(EDITOR_STATE state) { m_State = state; }

public:
    Shared<EditorCamera> Get_EditorCamera() const { return m_EditorCamera; }
    Shared<Engine::Camera> Get_InGameCamera() const { return m_InGameCamera; }
    void Set_InGameCamera(const Shared<Engine::Camera> &camera) {
		m_InGameCamera = camera;
    }

    Shared<ModelViewer> Get_ModelViewer() const { return m_ModelViewer; }
    Shared<MenuBar> Get_MenuBar() const { return m_MenuBar; }

public:
    Bool Is_ResizeRequest() const { return m_IsResizeView; }
    RESIZE_INFO Get_ResizeInfo() const
    {
        return RESIZE_INFO{ m_ResizeWidth, m_ResizeHeight, m_ScreenIndex };
    }
    void RequestResize(Float width, Float height, uint32 screenIndex)
    {
        if (width <= 0.f) width = 10.f;
        if (height <= 0.f) height = 10.f;
        m_IsResizeView = true;
        m_ResizeWidth = width; 
    	m_ResizeHeight = height; 
    	m_ScreenIndex = screenIndex;
    }
    void Clear_ResizeRequest() { m_IsResizeView = false; }

public: /* Selected Object (Hierarchy <-> Inspector 공유) */
  Shared<GameObject> Get_SelectedObject() const {
    return m_SelectedObject.lock();
  }
  void Set_SelectedObject(const Shared<GameObject>& obj) {
    m_SelectedObject = obj;
  }
  void Clear_SelectedObject() { m_SelectedObject.reset(); }
  
public: /* Reset Request */
    void Request_Reset(uint32 startLevel) {
        m_IsResetRequested = true;
        m_EngineDesc.startLevel = startLevel;
    }
    Bool Is_ResetRequested() const { return m_IsResetRequested; }
    void Clear_ResetRequest() { m_IsResetRequested = false; }

    ENGINE_DESC& Get_EngineDesc() { return m_EngineDesc; }
    void Set_EngineDesc(const ENGINE_DESC& desc)
    {
        m_EngineDesc = desc;}

public: /* Auto Load Settings */
    Bool Is_AutoLoadEnabled() const { return m_IsAutoLoad; }
    void Set_AutoLoadEnabled(Bool isEnabled) { m_IsAutoLoad = isEnabled; }

    Bool Is_AutoLoadRequested() const { return m_IsAutoLoadRequested; }
    void Request_AutoLoad(Bool isRequested) { m_IsAutoLoadRequested = isRequested; }

private:
    Bool m_IsResizeView{ false };
    Float m_ResizeWidth{}, m_ResizeHeight{};
	uint32 m_ScreenIndex{};

    EDITOR_STATE m_State = EDITOR_STATE::STOP;
    Shared<EditorCamera> m_EditorCamera{nullptr};
    Shared<Engine::Camera> m_InGameCamera{nullptr};
    Weak<Engine::GameObject> m_SelectedObject = {};

    Bool m_IsResetRequested{ false };
    ENGINE_DESC m_EngineDesc = {};

    Bool m_IsAutoLoad{ true };
    Bool m_IsAutoLoadRequested{ false };

private:
    Shared<Inspector> m_Inspector = {nullptr};
    Shared<EditorView> m_EditorView = {nullptr};
    Shared<MenuBar> m_MenuBar = {nullptr};
    Shared<AssetBrowser> m_AssetBrowser = {nullptr};
    Shared<LogConsole> m_LogConsole = {nullptr};
    Shared<Hierarchy> m_Hierarchy = {nullptr};
    Shared<ModelViewer> m_ModelViewer = {nullptr};
};

NS_END