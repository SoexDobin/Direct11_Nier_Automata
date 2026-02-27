#pragma once

NS_BEGIN(Editor)

class Inspector;
class EditorView;
class MenuBar;
class PrefabTab;
class LogConsole;

class EditorManager 
{
	DECLARE_SINGLETON(EditorManager)
public:
	EditorManager();
	~EditorManager();

public:
	HRESULT Initialize();
	void Update();
	void Render();

private:
	Shared<Inspector> m_Inspector = {nullptr};
	Shared<EditorView> m_EditorView = {nullptr};
	Shared<MenuBar> m_MenuBar = {nullptr};
	Shared<PrefabTab> m_PrefabTab = {nullptr};
	Shared<LogConsole> m_LogConsole = {nullptr};

};

NS_END