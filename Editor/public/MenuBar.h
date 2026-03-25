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

	// Client 해상도 설정 메뉴 Json으로 저장해 두고 클라에서 ProjectSetting 파일 구성
private:
	Shared<Game> m_Game = { nullptr };
	char m_Title[MAX_PATH] = "client";
	
	
public:
	static Shared<MenuBar> Create();
};

NS_END