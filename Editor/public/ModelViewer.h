#pragma once
#include "EditorObject.h"
#include "Model.h"
#include <filesystem>

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
	Shared<Engine::Model> Get_SelectedModel() const { return m_pSelectedModel; }
	HRESULT Preview_Model(const Shared<Engine::Model>& model, ComPtr<ID3D11ShaderResourceView>& image,
		int32 meshIndex = -1);
	static HRESULT Save_MaterialSettings(const Shared<Engine::Model>& model,
		const Engine::MODEL_MATERIAL_SETTINGS& draft, const filesystem::path& target, string& status);

private:
	void View_ModelList();
	void View_AnimationList();
	void View_MaterialSettings();
	void Handle_KeyInput();
	void Load_MaterialDraft();
	void Generate_MaterialDraft();
	void Save_MaterialDraft();
	filesystem::path Get_MaterialSettingsPath() const;

private:
	wstring m_SelectedModelTag = L"";
	int32 m_SelectedAnimIndex = -1;
	int32 m_PreviewMeshIndex = -1;
	int32 m_CurrentLevelIndex = 0;

	Shared<Engine::Model> m_pSelectedModel = nullptr;
	Engine::MODEL_MATERIAL_SETTINGS m_MaterialDraft{};
	Bool m_HasMaterialDraft{ false };
	string m_MaterialStatus{};
	Weak<Engine::Model> m_PreviewModel;
	Shared<Engine::Shader> m_PreviewShader;
	ComPtr<ID3D11RenderTargetView> m_PreviewTarget;
	ComPtr<ID3D11RenderTargetView> m_PreviewNormal;
	ComPtr<ID3D11ShaderResourceView> m_PreviewImage;
	ComPtr<ID3D11DepthStencilView> m_PreviewDepth;
	Engine::Vector3 m_PreviewCenter{};
	Float m_PreviewRadius{ 1.f };

public:
	static Shared<ModelViewer> Create();
};

NS_END
