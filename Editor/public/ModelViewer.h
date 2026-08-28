#pragma once
#include "EditorObject.h"
#include <filesystem>

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
	void View_AnimationPreset();
	void Handle_KeyInput();
	void New_AnimationPreset();
	void Save_AnimationPreset();
	void Load_AnimationPreset(const filesystem::path& presetPath);
	void Apply_AnimationPreset();

private:
	wstring m_SelectedModelTag = L"";
	int32 m_SelectedAnimIndex = -1;
	int32 m_CurrentLevelIndex = 0;

	Shared<Engine::Model> m_pSelectedModel = nullptr;
	Char m_PresetName[128]{};
	Char m_PresetGuid[33]{};
	vector<wstring> m_PresetAnimationPaths;
	string m_PresetStatus;

public:
	static Shared<ModelViewer> Create();
};

NS_END
