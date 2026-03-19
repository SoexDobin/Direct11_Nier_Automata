#include "pch.h"
#include "EditorView.h"
#include "ImGuizmo.h"
#include <Transform.h>
#include "UIObject.h"
#include "Game.h"
#include "ID_Helper.h"
#include "EditorManager.h"
#include "PathManager.h"

static ImGuizmo::OPERATION m_CurrentGizmoMode = ImGuizmo::TRANSLATE;

EditorView::EditorView() {}
EditorView::~EditorView() {}

HRESULT EditorView::Initialize()
{
	return EditorObject::Initialize();
}

void EditorView::Update(Bool isResize) {}

void EditorView::Render(Bool isResize) {
	// m_IsDirty 체크 제거하여 항상 렌더링되게 함 (기즈모/피킹 상태 반영 보장)
	EditorView::RenderView(isResize);
}

void EditorView::RenderView(Bool isResize) {
	static ImVec2 prevSceneViewportSize{};
	static ImVec2 prevGameViewportSize{};
	auto srvScene = GAME_INSTANCE->Get_OffScreenSRV(1);
	auto srvGame = GAME_INSTANCE->Get_OffScreenSRV(0);

	ImGui::Begin("Scene View");
	{
		auto selected = EDITOR->Get_SelectedObject();
		if (selected)
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Selected: %S", selected->Get_Name().c_str());
		else
			ImGui::Text("Selected: None");

		if (srvScene && !isResize)
		{
			ImVec2 currentSize = ImGui::GetContentRegionAvail();
			if (prevSceneViewportSize.x != currentSize.x || prevSceneViewportSize.y != currentSize.y) {
				prevSceneViewportSize = currentSize;
				EDITOR->RequestResize(currentSize.x, currentSize.y, 1);
			}

			ImGui::Image(reinterpret_cast<ImTextureID>(srvScene.Get()), prevSceneViewportSize);
			
			// ── Scene View 드래그 앤 드롭 수신 ──────────────────────────────────
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(Drag_PayLoadKey.c_str()))
				{
					const wchar_t *rawTag = static_cast<const wchar_t *>(payload->Data);
					wstring prototypeTag(rawTag);

					// 드롭된 마우스 위치 계산 (Viewport NDC 변환)
					ImVec2 mousePos = ImGui::GetMousePos();
					ImVec2 imageRectMin = ImGui::GetItemRectMin();
					ImVec2 imageRectSize = ImGui::GetItemRectSize();

					Float localX = mousePos.x - imageRectMin.x;
					Float localY = mousePos.y - imageRectMin.y;
					Float ndcX = (localX / imageRectSize.x) * 2.f - 1.f;
					Float ndcY = 1.f - (localY / imageRectSize.y) * 2.f;

					// 레이 생성
					Matrix invView = GAME_INSTANCE->Get_InvTransform(D3DTS::VIEW);
					Matrix invProj = GAME_INSTANCE->Get_InvTransform(D3DTS::PROJ);

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

					// 스폰 위치 결정: 지면(Y=0)과의 교점 또는 특정 거리 앞
					Vector3 spawnPos{};
					if (abs(rayDir.y) > 0.0001f)
					{
						Float t = -rayOrigin.y / rayDir.y;
						if (t > 0)
						{
							spawnPos = rayOrigin + rayDir * t;
						}
						else
						{
							spawnPos = rayOrigin + rayDir * 5.f;
						}
					}
					else
					{
						spawnPos = rayOrigin + rayDir * 5.f;
					}

					// 객체 생성 및 배치
					Shared<GameObject> cloned = GAME_INSTANCE->Instantiate<GameObject>(prototypeTag, UINT_MAX);
					if (cloned)
					{
						auto allObjs = GAME_INSTANCE->Get_GameObjects();
						int suffix = 0;
						wstring baseName = cloned->Get_Name();
						wstring uniqueName = baseName;
						while (true) {
							uniqueName = (suffix == 0) ? baseName : baseName + L"_" + std::to_wstring(suffix);
							bool overlap = false;
							for (auto& [id, obj] : allObjs) {
								if (obj != cloned && obj->Get_Name() == uniqueName) { overlap = true; break; }
							}
							if (!overlap) break;
							suffix++;
						}
						cloned->Set_Name(uniqueName);
						cloned->Set_ObjectID(Helper::Create_FixedObjectID(prototypeTag, uniqueName));

						cloned->Get_Transform()->Set_LocalPositionByValue(spawnPos);
						LOG_INFO(L"[SceneView] Dropped {} at ({}, {}, {})", prototypeTag, spawnPos.x, spawnPos.y, spawnPos.z);
						EDITOR->Set_SelectedObject(cloned);
					}
				}
				ImGui::EndDragDropTarget();
			}

			ImVec2 imageRectMin = ImGui::GetItemRectMin();
			ImVec2 imageRectSize = ImGui::GetItemRectSize();

			EditorView::MousePicking(imageRectSize, imageRectMin);
			Update_ImGuizmo(imageRectSize, imageRectMin);
		}
	}
	ImGui::End();

	ImGui::Begin("Game View");

	// 마우스가 GameView 컨텐츠 영역 내에 있거나, 카메라 등으로 마우스가 락(Lock)되어 있으면 입력 허용
	Bool isGameViewHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
	GAME_INSTANCE->Set_InputEnabled(isGameViewHovered || GAME_INSTANCE->Get_MouseLock());

	Bool loadFinished = GAME_INSTANCE->LevelLoad_Finished();
	Bool canPlay = (EDITOR->Get_State() != EDITOR_STATE::PLAY);
	Bool isPlayDisabled = canPlay && loadFinished;
	if (!isPlayDisabled) ImGui::BeginDisabled();
	if (ImGui::Button("Play")) {
		if (SUCCEEDED(GAME_INSTANCE->SerializeLevel(PATH.GetSceneDataPath())))
			EDITOR->Set_State(EDITOR_STATE::PLAY);
		else
			LOG_CRITICAL("Failed to Save Level Data");
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

void EditorView::MousePicking(ImVec2 viewport, ImVec2 imageStartPos)
{
	ImVec2 mousePos = ImGui::GetMousePos();
	
	// 마우스가 이미지 영역 안에 있는지 수동 체크
	Bool isHovered = (mousePos.x >= imageStartPos.x && mousePos.x <= imageStartPos.x + viewport.x &&
					  mousePos.y >= imageStartPos.y && mousePos.y <= imageStartPos.y + viewport.y);

	if (!isHovered) return;

	// 좌클릭 시에만 연산 수행
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		if (ImGuizmo::IsOver()) return;

		Float localX = mousePos.x - imageStartPos.x;
		Float localY = mousePos.y - imageStartPos.y;
		
		Float ndcX = (localX / viewport.x) * 2.f - 1.f;
		Float ndcY = 1.f - (localY / viewport.y) * 2.f;

		Matrix invView = GAME_INSTANCE->Get_InvTransform(D3DTS::VIEW);
		Matrix invProj = GAME_INSTANCE->Get_InvTransform(D3DTS::PROJ);

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
		
		Shared<GameObject> pickedObject = nullptr;
		Float minDistance = FLT_MAX;
		
		// 모든 객체를 순회하며 피킹 검사
		for (auto& pair : GAME_INSTANCE->Get_GameObjects()) {
			auto obj = pair.second;

			if (obj == EDITOR->Get_EditorCamera()) continue;

			auto uiObj = std::dynamic_pointer_cast<UIObject>(obj);
			if (uiObj)
			{
				uiObj->Update_UITransform(viewport.x, viewport.y);

				Matrix uiWorld = uiObj->Get_Transform()->Get_WorldMatrix();
				Matrix invUIWorld = uiWorld.Invert();

				Vector3 mousePos_Center;
				mousePos_Center.x = localX - viewport.x * 0.5f;
				mousePos_Center.y = -localY + viewport.y * 0.5f;
				mousePos_Center.z = 0.f;

				Vector3 localMouse = Vector3::Transform(mousePos_Center, invUIWorld);

				if (localMouse.x >= -0.5f && localMouse.x <= 0.5f &&
					localMouse.y >= -0.5f && localMouse.y <= 0.5f)
				{
					pickedObject = obj;
					minDistance = -1.f; 
					break;
				}
				continue;
			}

			// 3D 객체 피킹 (충돌체 없으면 간단한 Sphere 체크)
			Vector3 pos = obj->Get_Transform()->Get_Position();
			BoundingSphere sphere(pos, 1.0f); // 범위를 약간 넓힘 (0.5f -> 1.0f)
			float dist = 0.f;

			if (sphere.Intersects(rayOrigin, rayDir, dist)) {
				if (dist < minDistance) {
					minDistance = dist;
					pickedObject = obj;
				}
			}
		}
		
		if (pickedObject) {
			EDITOR->Set_SelectedObject(pickedObject);
		}
		else {
			EDITOR->Clear_SelectedObject();
		}
	}
}

void EditorView::Update_ImGuizmo(ImVec2 viewport, ImVec2 imageStartPos)
{
	if (EDITOR->Get_State() == EDITOR_STATE::PLAY) return;

	auto selectedObj = EDITOR->Get_SelectedObject();
	if (!selectedObj) return;
	auto transform = selectedObj->Get_Transform();

	// 1. 기즈모 드로잉 설정
	ImGuizmo::SetDrawlist(); // 기본값 사용 (현재 윈도우)
	ImGuizmo::SetRect(imageStartPos.x, imageStartPos.y, viewport.x, viewport.y);
	ImGuizmo::Enable(true);

	// 2. 행렬 준비 (SimpleMath::Matrix 사용)
	Matrix view = GAME_INSTANCE->Get_Transform(D3DTS::VIEW);
	Matrix proj = GAME_INSTANCE->Get_Transform(D3DTS::PROJ);
	
	// 3. 조작 처리
	auto uiObj = std::dynamic_pointer_cast<UIObject>(selectedObj);
	if (uiObj)
	{
		ImGuizmo::SetOrthographic(true);
		Matrix uiView = Matrix::Identity;
		Matrix uiProj = XMMatrixOrthographicOffCenterLH(0, viewport.x, viewport.y, 0, 0.f, 1.f);
		
		Vector2 anchorPos = uiObj->Get_AnchorPos(viewport.x, viewport.y);
		uiObj->Update_UITransform(viewport.x, viewport.y);

		Matrix uiScreenWorld = transform->Get_LocalMatrix() * Matrix::CreateTranslation(anchorPos.x, anchorPos.y, 0);

		if (ImGuizmo::Manipulate(
			reinterpret_cast<Float*>(&uiView),
			reinterpret_cast<Float*>(&uiProj),
			m_CurrentGizmoMode,
			ImGuizmo::LOCAL,
			reinterpret_cast<Float*>(&uiScreenWorld)))
		{
			Vector3 scale, pos;
			Quaternion rot;
			if (uiScreenWorld.Decompose(scale, rot, pos)) {
				transform->Set_LocalPositionByValue(Vector3(pos.x - anchorPos.x, pos.y - anchorPos.y, 0.f));
				transform->Set_LocalRotation(rot);
				transform->Set_LocalScaleByValue(scale);
				uiObj->Update_UITransform(viewport.x, viewport.y);
			}
		}
	}
	else
	{
		ImGuizmo::SetOrthographic(false);
		transform->Update_WorldMatrix();
		Matrix world = transform->Get_WorldMatrix();

		if (ImGuizmo::Manipulate(
			reinterpret_cast<Float*>(&view),
			reinterpret_cast<Float*>(&proj),
			m_CurrentGizmoMode,
			ImGuizmo::WORLD,
			reinterpret_cast<Float*>(&world)))
		{
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

