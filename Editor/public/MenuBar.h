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
	void ViewEngineDesc();
	HRESULT Load_EngineDesc();
	HRESULT Save_EngineDesc();

	// Client 해상도 설정 메뉴 Json으로 저장해 두고 클라에서 ProjectSetting 파일 구성
private:
	ENGINE_DESC m_EngineDesc {};
	Char m_Title[MAXCHAR];
	
	
public:
	static Shared<MenuBar> Create();
};

NS_END