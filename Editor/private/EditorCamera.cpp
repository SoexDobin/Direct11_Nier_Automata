#include "pch.h"
#include "EditorCamera.h"

#include <Transform.h>

EditorCamera::EditorCamera(const ComPtr<ID3D11Device> &device,
                           const ComPtr<ID3D11DeviceContext> &context)
    : Camera{device, context} {}

HRESULT EditorCamera::Initialize(void *arg) {
  if (nullptr == arg) {
    LOG_ERROR(L"Need EDITOR_CAMERA_DESC to Create Camera");
    return E_FAIL;
  }
  if (FAILED(Camera::Initialize_Prototype())) {
    LOG_ERROR(L"Failed to Initialize Prototype");
    return E_FAIL;
  }

  const EDITOR_CAMERA_DESC &desc = *static_cast<EDITOR_CAMERA_DESC *>(arg);
  m_CameraSpeed = desc.cameraSpeed;
  m_MouseSensitive = desc.mouseSensitive;

  if (FAILED(Camera::Initialize(arg))) {
    LOG_ERROR(L"Failed to Initialize");
    return E_FAIL;
  }

  return S_OK;
}

void EditorCamera::Priority_Update(Float timeDelta) {
  Camera::Priority_Update(timeDelta);
}

void EditorCamera::Update(Float timeDelta) {

    Float speed = m_CameraSpeed * timeDelta;
    // 1. WASD (XZ 평면 이동)
    Vector3 look = m_Transform->Get_WorldMatrix().Backward();
    Vector3 right = m_Transform->Get_WorldMatrix().Right();

    // Y축 파괴 및 정규화
    look.y = 0.f;  look.Normalize();
    right.y = 0.f; right.Normalize();
    Vector3 currentPos = m_Transform->Get_Position();
    if (ImGui::IsKeyDown(ImGuiKey_W)) currentPos += look * speed;
    if (ImGui::IsKeyDown(ImGuiKey_S)) currentPos -= look * speed;
    if (ImGui::IsKeyDown(ImGuiKey_A)) currentPos -= right * speed;
    if (ImGui::IsKeyDown(ImGuiKey_D)) currentPos += right * speed;
    m_Transform->Set_Position(currentPos);
    // 2. 마우스 휠 (줌 인/아웃)
    Float wheel = ImGui::GetIO().MouseWheel;
    if (wheel != 0.f)
    {
        Vector3 zoomDir = m_Transform->Get_WorldMatrix().Backward();
        zoomDir.Normalize();
        // 휠 1틱당 이동할 거리 지정 (배율 곱하기)
        m_Transform->Set_Position(m_Transform->Get_Position() + zoomDir * wheel * 5.f);
    }
    // 3. 우클릭 드래그 시 회전 (Turn)
    if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
        ImVec2 mouseDelta = ImGui::GetIO().MouseDelta;

        if (mouseDelta.x != 0.f || mouseDelta.y != 0.f)
        {
            m_Transform->Turn(Vector3{
                mouseDelta.y,
                mouseDelta.x,
                0.f
            },
                timeDelta,
                m_MouseSensitive
            );
        }
    }
}

void EditorCamera::Late_Update(Float timeDelta) {}
void EditorCamera::Fixed_Update(Float fixedDelta) {}
HRESULT EditorCamera::Render() { return S_OK; }

HRESULT EditorCamera::Bind_EditorMatrix() const {
  m_Transform->Update_WorldMatrix();
  const Matrix &viewMatrix = m_Transform->Get_WorldMatrix().Invert();
  Matrix projMatrix = XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_Near, m_Far);

  GAME_INSTANCE->Set_Transform(D3DTS::VIEW, viewMatrix);
  GAME_INSTANCE->Set_Transform(D3DTS::PROJ, projMatrix);

  return S_OK;
}

Vector3
EditorCamera::RaycastEditorView(Vector2 mousePos, Vector2 viewSize,
                                Shared<class GameOject> *outObjectAddress) {
  return Vector3::One;
}

Shared<EditorCamera>
EditorCamera::Create(const ComPtr<ID3D11Device> &device,
                     const ComPtr<ID3D11DeviceContext> &context,
                     EDITOR_CAMERA_DESC &desc) {
  auto editorCamera = make_shared<EditorCamera>(device, context);

  if (FAILED(editorCamera->Initialize(&desc))) {
    LOG_ERROR(L"Failed to Create EditorCamera");
    return nullptr;
  }

  return editorCamera;
}
