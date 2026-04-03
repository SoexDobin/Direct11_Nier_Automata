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
	Shared<Game> m_Game = { nullptr };
	char m_Title[MAX_PATH] = "client";

	Bool m_ShowProjectSettings = false;

public:
	static Shared<MenuBar> Create();
};

NS_END