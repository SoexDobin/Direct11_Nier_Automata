#pragma once
#include "EditorObject.h"

NS_BEGIN(Engine)
class Model;
NS_END

NS_BEGIN(Editor)

class ModelViewer final : public EditorObject
{
public:
	ModelViewer();
	~ModelViewer() override = default;

public:
	HRESULT Initialize() override;
	void Update(Bool isResize) override;
	void Render(Bool isResize) override;

private:
	void View_ModelList();
	void View_AnimationList();
	void Handle_KeyInput();

private:
	wstring m_SelectedModelTag = L"";
	int32 m_SelectedAnimIndex = -1;
	int32 m_CurrentLevelIndex = 0;

	Shared<Engine::Model> m_pSelectedModel = nullptr;

public:
	static Shared<ModelViewer> Create();
};

NS_END
