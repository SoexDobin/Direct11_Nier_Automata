#pragma once
#include "EditorObject.h"

NS_BEGIN(Editor)

class Inspector final : public EditorObject
{
public:
	Inspector();
	~Inspector() override;

public:
	HRESULT Initialize() override;
	void Update() override;
	void Render() override;

private:
	void LayerTagGUI();

private:
	wstring m_layerJsonPath = {};
	wstring m_tagJsonPath = {};

public:
	static Shared<Inspector> Create();
	
};

NS_END