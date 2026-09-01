#pragma once
#include "EditorObject.h"

NS_BEGIN(Editor)

class MenuBar final : public EditorObject
{
public:
	MenuBar();
	~MenuBar() override = default;

public:
	HRESULT Initialize() override;
	void Update(Bool isResize) override;
	void Render(Bool isResize) override;

private:
	void Update_HotKey();
	void Render_Debug();
	void Render_Prefab();

private:
	Shared<Game> m_Game = { nullptr };
	char m_Title[MAX_PATH] = "client";

	Bool m_ShowProjectSettings = false;
	char m_PrefabName[MAX_PATH]{};

	Bool m_PrevF1 = false;
	Bool m_PrevF2 = false;

public:
	static Shared<MenuBar> Create();
};

NS_END
