#include "pch.h"
#include "EditorView.h"
#include "EditorManager.h"


EditorView::EditorView() {}
EditorView::~EditorView() {}

HRESULT EditorView::Initialize()
{
	return EditorObject::Initialize();
}

void EditorView::Update() {}

void EditorView::Render() {
	if (!m_IsDirty)
		return;

	EditorView::RenderView();
}

void EditorView::RenderView() {
	auto srvGame = GAME_INSTANCE->Get_OffScreenSRV(0);
	auto srvScene = GAME_INSTANCE->Get_OffScreenSRV(1);

	ImGui::Begin("Scene View");
	if (srvScene)
	{
		ImGui::Text("FPS : %3f", GAME_INSTANCE->Get_FPS());

		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		ImGui::Image(reinterpret_cast<ImTextureID>(srvScene.Get()), viewportSize);

	}
	ImGui::End();

	ImGui::Begin("Game View");
	if (ImGui::Button("Play")) {
		if (EDITOR->Get_State() == EDITOR_STATE::PLAY) return;
		EDITOR->Set_State(EDITOR_STATE::PLAY);
	}
	ImGui::SameLine();
	if (ImGui::Button("Pause")) {
		EDITOR->Set_State(EDITOR_STATE::PAUSE);
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop")) {
		if (EDITOR->Get_State() == EDITOR_STATE::STOP) return;
		EDITOR->Set_State(EDITOR_STATE::STOP);
	}
	ImGui::Separator();

	if (srvGame) {
		ImVec2 viewportSize = ImGui::GetContentRegionAvail();
		ImGui::Image(reinterpret_cast<ImTextureID>(srvGame.Get()), viewportSize);
		
	}

	ImGui::End();
}

Shared<EditorView> EditorView::Create()
{
	auto editorView = make_shared<EditorView>();

	if (FAILED(editorView->Initialize()))
	{
		MSG_BOX("Failed to Create EditorView");
		return nullptr;
	}

	return editorView;
}

