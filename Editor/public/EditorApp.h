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
	HRESULT Initialize_IMGUI(const ENGINE_DESC& desc);
	HRESULT Destruct_IMGUI();

public:
	static Unique<EditorApp> Create();

};

NS_END
