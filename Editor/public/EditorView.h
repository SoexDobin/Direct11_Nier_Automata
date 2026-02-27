#pragma once
#include "EditorObject.h"

NS_BEGIN(Editor)

class EditorView final : public EditorObject
{
public:
	EditorView();
	~EditorView() override;

public:
	HRESULT Initialize() override;
	void Update() override;
	void Render() override;

private:
	void RenderView();

private:
	Bool m_PlayMode = { false };

public:
	static Shared<EditorView> Create();

};

NS_END