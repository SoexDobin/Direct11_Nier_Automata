#include "pch.h"
#include "EditorView.h"

#include <Transform.h>

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
	auto srvScene = GAME_INSTANCE->Get_OffScreenSRV(1);
	auto srvGame = GAME_INSTANCE->Get_OffScreenSRV(0);

	ImGui::Begin("Scene View");
	if (srvScene)
	{
		ImVec2 currentSize = ImGui::GetContentRegionAvail();
		if (prevSceneViewportSize.x != currentSize.x || prevSceneViewportSize.y != currentSize.y) {
			prevSceneViewportSize = currentSize;
			EDITOR->RequestResize(currentSize.x,currentSize.y,1);
		}
		
		ImGui::Text("FPS : %3f", GAME_INSTANCE->Get_FPS());
		ImGui::Image(reinterpret_cast<ImTextureID>(srvScene.Get()), prevSceneViewportSize);
		prevSceneViewportSize = currentSize;
		MousePicking(prevSceneViewportSize);
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
		ImVec2 currentSize = ImGui::GetContentRegionAvail();
		if (prevGameViewportSize.x != currentSize.x || prevGameViewportSize.y != currentSize.y) {
			prevGameViewportSize = currentSize;
			EDITOR->RequestResize(currentSize.x,currentSize.y,0);
		}
		
		ImGui::Image(reinterpret_cast<ImTextureID>(srvGame.Get()), prevGameViewportSize);
		prevGameViewportSize = currentSize;
	}

	ImGui::End();
}

void EditorView::MousePicking(ImVec2 viewport)
{
	Matrix invView = GAME_INSTANCE->Get_InvTransform(D3DTS::VIEW);
	Matrix invProj = GAME_INSTANCE->Get_InvTransform(D3DTS::PROJ);

	if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
	{
		ImVec2 mousePos = ImGui::GetMousePos();

		// [핵심] GetCursorScreenPos는 이미지가 그려지기 시작한 '진짜' 절대 좌표를 줍니다.
		// 이미지를 그리기 직전에 이 좌표를 가져오거나 별도로 저장해 두어야 합니다.
		// 임시로 GetWindowPos() + GetCursorPos() 조합을 사용하는 것이 안전합니다.
		ImVec2 imageStartPos = ImGui::GetItemRectMin(); // 현재 그려진 아이템(이미지)의 최소 좌표
		Float localX = mousePos.x - imageStartPos.x;
		Float localY = mousePos.y - imageStartPos.y;
		
		Float ndcX = (localX / viewport.x) * 2.f - 1.f;
		Float ndcY = 1.f - (localY / viewport.y) * 2.f;
		
		Vector4 vNear = Vector4(ndcX, ndcY, 0.f, 1.f);
		Vector4 vFar = Vector4(ndcX, ndcY, 1.f, 1.f);
		
		vNear = Vector4::Transform(vNear, invProj);
		vNear /= vNear.w;
		vNear = Vector4::Transform(vNear, invView);
		vFar = Vector4::Transform(vFar, invProj);
		vFar /= vFar.w;   
		vFar = Vector4::Transform(vFar, invView);
		
		Vector3 rayOrigin = Vector3(vNear.x, vNear.y, vNear.z);
		Vector3 rayTarget = Vector3(vFar.x, vFar.y, vFar.z);
		Vector3 rayDir = rayTarget - rayOrigin;
		rayDir.Normalize();
		// 6. Y = 0 평면(바닥)과 충돌 계산
		if (fabs(rayDir.y) > 0.0001f)
		{
			float t = -rayOrigin.y / rayDir.y;
			if (t > 0.f)
			{
				Vector3 worldPickPos = rayOrigin + rayDir * t;
				// [디버깅 출력]
				ImGui::BeginTooltip();
				ImGui::Text("Picking Info (Fixed)");
				ImGui::Separator();
				ImGui::Text("NDC      X:%.3f, Y:%.3f", ndcX, ndcY);
				ImGui::TextColored(ImVec4(0, 0.8f, 0.f, 1),
					"World Pos X:%.2f, Y:%.2f, Z:%.2f", worldPickPos.x, worldPickPos.y, worldPickPos.z);

			}
		}
		// 클릭 중인 좌표를 시각적으로 강조 (선택 사항)
		if (ndcX >= -1.f && ndcX <= 1.f && ndcY >= -1.f && ndcY <= 1.f)
			ImGui::TextColored(ImVec4(0, 0.5f, 0.1f, 1), "Inside Viewport");
		else
			ImGui::TextColored(ImVec4(1.f, 0.2f, 0.2f, 1.f), "Outside Viewport");

		ImGui::EndTooltip();
	}
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

