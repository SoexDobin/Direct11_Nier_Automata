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
	static ImVec2 prevSceneViewportSize{};
	static ImVec2 prevGameViewportSize{};
	auto srvGame = GAME_INSTANCE->Get_OffScreenSRV(0);
	auto srvScene = GAME_INSTANCE->Get_OffScreenSRV(1);

	ImVec2 sceneViewportSize = ImGui::GetContentRegionAvail();
	if (sceneViewportSize.x != prevSceneViewportSize.x || sceneViewportSize.y != prevSceneViewportSize.y)
	{
		if (sceneViewportSize.x > 0 && sceneViewportSize.y > 0)
		{
			GAME_INSTANCE->OnResize(static_cast<uint32_t>(sceneViewportSize.x), static_cast<uint32_t>(sceneViewportSize.y));
			prevSceneViewportSize = sceneViewportSize;
		}
	}

	ImGui::Begin("Scene View");
	if (srvScene)
	{
		ImGui::Text("FPS : %3f", GAME_INSTANCE->Get_FPS());

		
		ImGui::Image(reinterpret_cast<ImTextureID>(srvScene.Get()), sceneViewportSize);
		
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

