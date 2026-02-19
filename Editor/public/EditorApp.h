#pragma once

NS_BEGIN(Engine)
class Game;
NS_END

NS_BEGIN(Editor)

class EditorManager;

class EditorApp final {
public:
	EditorApp();
	~EditorApp();

public:
	HRESULT Initialize();
	void Update();
	HRESULT Render();

private:
	HRESULT Initialize_IMGUI();
	HRESULT Destruct_IMGUI();

private:
	Shared<Game> m_Game = { nullptr };
	Shared<EditorManager> m_Editor = { nullptr };

public:
	static Unique<EditorApp> Create();

};

NS_END
