#pragma once
#include "Editor_Define.h"

NS_BEGIN(Editor)

class Inspector;
class EditorView;
class MenuBar;
class PrefabTab;
class LogConsole;
class MetadataManager;
class PrefabRegistry;

class EditorManager {
  DECLARE_SINGLETON(EditorManager)
public:
  EditorManager();
  ~EditorManager();

public:
  HRESULT Initialize();
  void Update();
  void Render();

  Shared<PrefabRegistry> Get_PrefabRegistry() const { return m_PrefabRegistry; }
  Shared<MetadataManager> Get_MetadataManager() const { return m_MetadataManager; }

private:
	Shared<Inspector> m_Inspector = {nullptr};
	Shared<EditorView> m_EditorView = {nullptr};
	Shared<MenuBar> m_MenuBar = {nullptr};
	Shared<PrefabTab> m_PrefabTab = {nullptr};
	Shared<LogConsole> m_LogConsole = {nullptr};
	Shared<MetadataManager> m_MetadataManager = {nullptr};
	Shared<PrefabRegistry> m_PrefabRegistry = {nullptr};
};

NS_END