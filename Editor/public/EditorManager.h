#pragma once

NS_BEGIN(Editor)

class Inspector;
class EditorView;
class MenuBar;

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
	Bool m_ShowInspector = { true };

private:
	Shared<Inspector> m_Inspector = { nullptr };
	Shared<EditorView> m_EditorView = { nullptr };
	Shared<MenuBar> m_MenuBar = { nullptr };
	
};

NS_END