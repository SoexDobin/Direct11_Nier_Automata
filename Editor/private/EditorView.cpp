#include "pch.h"
#include "EditorView.h"
#include "EditorManager.h"


EditorView::EditorView() {}
EditorView::~EditorView() {}

HRESULT EditorView::Initialize() { return EditorObject::Initialize(); }

void EditorView::Update() {}

void EditorView::Render() {
  if (!m_IsDirty)
    return;

  // HRESULT hr = {};
  // hr = GAME_INSTANCE->Begin_RenderOffScreen(0);
  // hr = GAME_INSTANCE->Draw();
  // hr = GAME_INSTANCE->End_RenderOffScreen();
  // if (FAILED(hr)) {
  //	LOG_CRITICAL(L"Failed {} Rendering ", editorViewTag);
  //	return;
  // }
  //
  // if (m_PlayMode)
  //{
  //	hr = GAME_INSTANCE->Begin_RenderOffScreen(gameViewTag);
  //	hr = GAME_INSTANCE->Draw();
  //	hr = GAME_INSTANCE->End_RenderOffScreen();
  // }
  // if (FAILED(hr)) {
  //	LOG_CRITICAL(L"Failed {} Rendering ", gameViewTag);
  //	return;
  // }

  EditorView::RenderView();
}

void EditorView::RenderView() {
  ImGui::Begin("Scene View");
  {
    auto srv = GAME_INSTANCE->Get_OffScreenSRV(0);
    if (srv) {
      ImVec2 viewportSize = ImGui::GetContentRegionAvail();
      ImGui::Image(reinterpret_cast<ImTextureID>(srv.Get()), viewportSize);
    } else
      ImGui::Text("Scene View: RenderTarget not available");
  }
  ImGui::End();

  ImGui::Begin("Game View");
  {
    if (ImGui::Button("Play")) {
      EDITOR->Set_PlayMode(true);
    }
    ImGui::SameLine();
    if (ImGui::Button("Stop")) {
      EDITOR->Set_PlayMode(false);
    }
    ImGui::SameLine();
    if (ImGui::Button("Quit")) {
      PostQuitMessage(0);
    }

    ImGui::Separator();

    auto srv = GAME_INSTANCE->Get_OffScreenSRV(0);
    if (srv) {
      ImVec2 viewportSize = ImGui::GetContentRegionAvail();
      ImGui::Image(reinterpret_cast<ImTextureID>(srv.Get()), viewportSize);
    } else
      ImGui::Text("Game View: RenderTarget not available");
  }
  ImGui::End();
}

Shared<EditorView> EditorView::Create() {
  auto view = make_shared<EditorView>();

  if (FAILED(view->Initialize())) {
    MSG_BOX("Failed To Create EditorView");
  }

  return view;
}
