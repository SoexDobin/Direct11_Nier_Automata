#include "pch.h"
#include "EditorView.h"
#include "ImGuizmo.h"
#include <Transform.h>

#include "EditorManager.h"

static ImGuizmo::OPERATION m_CurrentGizmoMode = ImGuizmo::TRANSLATE;

EditorView::EditorView() {}
EditorView::~EditorView() {}

HRESULT EditorView::Initialize()
{
	return EditorObject::Initialize();
}

void EditorView::Update(Bool isResize) {}

void EditorView::Render(Bool isResize) {
	if (!m_IsDirty)
		return;

	EditorView::RenderView(isResize);
}

void EditorView::RenderView(Bool isResize) {
	static ImVec2 prevSceneViewportSize{};
	static ImVec2 prevGameViewportSize{};
	auto srvScene = GAME_INSTANCE->Get_OffScreenSRV(1);
	auto srvGame = GAME_INSTANCE->Get_OffScreenSRV(0);

	ImGui::Begin("Scene View");
	if (srvScene && !isResize)
	{
		ImVec2 currentSize = ImGui::GetContentRegionAvail();
		if (prevSceneViewportSize.x != currentSize.x || prevSceneViewportSize.y != currentSize.y) {
			prevSceneViewportSize = currentSize;
			EDITOR->RequestResize(currentSize.x,currentSize.y,1);
		}
		
		ImGui::Text("FPS : %3f", GAME_INSTANCE->Get_FPS());
		ImGui::Image(reinterpret_cast<ImTextureID>(srvScene.Get()), prevSceneViewportSize);
		prevSceneViewportSize = currentSize;
		EditorView::MousePicking(prevSceneViewportSize);
		Update_ImGuizmo(prevSceneViewportSize);
	}
	ImGui::End();

	ImGui::Begin("Game View");
	Bool loadFinished = GAME_INSTANCE->LevelLoad_Finished();
	Bool canPlay = (EDITOR->Get_State() != EDITOR_STATE::PLAY);
	Bool isPlayDisabled = canPlay && loadFinished;
	if (!isPlayDisabled) ImGui::BeginDisabled();
	if (ImGui::Button("Play")) {
		EDITOR->Set_State(EDITOR_STATE::PLAY);
	}
	if (!isPlayDisabled) ImGui::EndDisabled();
	ImGui::SameLine();
	Bool canPause = (EDITOR->Get_State() == EDITOR_STATE::PLAY);
	if (!canPause) ImGui::BeginDisabled();
	if (ImGui::Button("Pause")) {
		if (EDITOR->Get_State() == EDITOR_STATE::PAUSE)
			EDITOR->Set_State(EDITOR_STATE::PLAY);
		else
			EDITOR->Set_State(EDITOR_STATE::PAUSE);
	}
	if (!canPause) ImGui::EndDisabled();
	ImGui::SameLine();
	Bool canStop = (EDITOR->Get_State() != EDITOR_STATE::STOP);
	if (!canStop) ImGui::BeginDisabled();
	if (ImGui::Button("Stop")) {
		EDITOR->Set_State(EDITOR_STATE::STOP);
	}
	if (!canStop) ImGui::EndDisabled();

	ImGui::SameLine();
	ImGui::Text(" | Gizmo: "); // 구분선
	ImGui::SameLine();
	// Translate 버튼
	{
		Bool isSelected = (m_CurrentGizmoMode == ImGuizmo::TRANSLATE);
		if (isSelected) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.8f, 1.0f));
		if (ImGui::Button("T")) { m_CurrentGizmoMode = ImGuizmo::TRANSLATE; }
		if (isSelected) ImGui::PopStyleColor(); // 버튼 클릭 여부와 상관없이 Push했으면 무조건 Pop
		ImGui::SameLine();
	}
	// Rotate 버튼
	{
		Bool isSelected = (m_CurrentGizmoMode == ImGuizmo::ROTATE);
		if (isSelected) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.8f, 1.0f));
		if (ImGui::Button("R")) { m_CurrentGizmoMode = ImGuizmo::ROTATE; }
		if (isSelected) ImGui::PopStyleColor();
		ImGui::SameLine();
	}
	// Scale 버튼
	{
		Bool isSelected = (m_CurrentGizmoMode == ImGuizmo::SCALE);
		if (isSelected) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.8f, 1.0f));
		if (ImGui::Button("S")) { m_CurrentGizmoMode = ImGuizmo::SCALE; }
		if (isSelected) ImGui::PopStyleColor();
	}

	ImGui::Separator();

	if (srvGame && !isResize) {
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
		
		if (ImGuizmo::IsOver()) return;
		Shared<GameObject> pickedObject = nullptr;
		float minDistance = FLT_MAX;
		// 모든 객체를 순회하며 피킹 검사
		for (auto& pair : GAME_INSTANCE->Get_GameObjects()) {
			auto obj = pair.second;
			Vector3 pos = obj->Get_Transform()->Get_Position();

			// 임시로 구체(Sphere) 반경 0.5f 정도로 피킹 영역 설정
			BoundingSphere sphere(pos, 0.5f);
			float dist = 0.f;

			if (sphere.Intersects(rayOrigin, rayDir, dist)) {
				if (dist < minDistance) {
					minDistance = dist;
					pickedObject = obj;
				}
			}
		}
		// 선택 결과 업데이트
		if (pickedObject) {
			EDITOR->Set_SelectedObject(pickedObject);
		}
		else {
			EDITOR->Clear_SelectedObject();
		}

		if (fabs(rayDir.y) > 0.0001f)
		{
			Float t = -rayOrigin.y / rayDir.y;
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
				ImGui::EndTooltip();
			}
		}
		// 클릭 중인 좌표를 시각적으로 강조 (선택 사항)
		if (ndcX >= -1.f && ndcX <= 1.f && ndcY >= -1.f && ndcY <= 1.f)
			ImGui::TextColored(ImVec4(0, 0.5f, 0.1f, 1), "Inside Viewport");
		else
			ImGui::TextColored(ImVec4(1.f, 0.2f, 0.2f, 1.f), "Outside Viewport");

	}
}

void EditorView::Update_ImGuizmo(ImVec2 viewport)
{
	if (EDITOR->Get_State() == EDITOR_STATE::PLAY) return;

	auto selectedObj = EDITOR->Get_SelectedObject();
	if (!selectedObj) return;
	auto transform = selectedObj->Get_Transform();

	// 1. 기즈모 드로잉 설정
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(ImGui::GetItemRectMin().x, ImGui::GetItemRectMin().y, viewport.x, viewport.y);
	// 2. 행렬 준비 (SimpleMath::Matrix 사용)
	Matrix view = GAME_INSTANCE->Get_Transform(D3DTS::VIEW);
	Matrix proj = GAME_INSTANCE->Get_Transform(D3DTS::PROJ);
	Matrix world = transform->Get_WorldMatrix();
	// 3. 조작 처리
	// ImGuizmo는 내부적으로 float[16]을 수정하므로 world 행렬이 직접 바뀝니다.
	if (ImGuizmo::Manipulate(
		reinterpret_cast<Float*>(&view), 
		reinterpret_cast<Float*>(&proj),
		m_CurrentGizmoMode, // 현재 조작 모드
		ImGuizmo::WORLD,
		reinterpret_cast<Float*>(&world)))
	{
		// 4. [핵심] 조작 결과 반영
		// 부모가 있을 경우 World 행렬에서 Local 행렬로 변환이 필요할 수 있으나,
		// 단순 구현을 위해 World 값을 분해해 바로 넣어줍니다.
		Vector3 scale, pos;
		Quaternion rot;
		if (world.Decompose(scale, rot, pos)) {
			transform->Set_LocalPositionByValue(pos);
			transform->Set_LocalRotation(rot);
			transform->Set_LocalScaleByValue(scale);
			transform->Update_WorldMatrix();
		}
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

