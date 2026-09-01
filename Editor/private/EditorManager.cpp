#include "pch.h"
#include "EditorManager.h"

#include "EditorCamera.h"
#include "Game.h"
#include "PathManager.h"

#include "EditorView.h"
#include "Inspector.h"
#include "LogConsole.h"
#include "MenuBar.h"
#include "Hierarchy.h"
#include "ModelViewer.h"
#include "AnimationPresetEditor.h"
#include "AssetBrowser.h"
#include "NavHelper.h"
#include "Transform.h"

IMPLEMENT_SINGLETON(EditorManager)

EditorManager::EditorManager()
    : m_Inspector{nullptr}, m_EditorView{nullptr}, m_MenuBar{nullptr},
      m_AssetBrowser{nullptr}, m_LogConsole{nullptr}, m_Hierarchy{nullptr},
	  m_ModelViewer{nullptr}
{}

EditorManager::~EditorManager() {}

HRESULT EditorManager::Initialize() 
{
    EditorCamera::EDITOR_CAMERA_DESC desc;
    {
        desc.cameraSpeed = 20.f;
        desc.mouseSensitive = 5.f;
        desc.eye = Vector4{ 0.f, 10.f, -10.f, 1.f };
        desc.at = Vector4{ 0.f, 0.f, 0.f, 1.f };
        desc.up = Vector4{ 0.f, 1.f, 0.f, 1.f };
        desc.fovY = XMConvertToRadians(60.f);
        desc.aspect = static_cast<Float>(g_projectSettings.viewportWidth) / static_cast<Float>(g_projectSettings.viewportHeight);
        desc.nearPlane = 0.1f;
        desc.farPlane = 500.f;
    }

    m_EditorCamera = EditorCamera::Create(GAME_INSTANCE->Get_Device(), GAME_INSTANCE->Get_Context(),desc);
    if (FAILED(m_EditorCamera->Bind_EditorMatrix()))
        return E_FAIL;

    /* UI 윈도우 */
    if (nullptr == (m_Inspector = Inspector::Create()))
		return E_FAIL;
    if (nullptr == (m_EditorView = EditorView::Create()))
		return E_FAIL;
    if (nullptr == (m_MenuBar = MenuBar::Create()))
		return E_FAIL;
    if (nullptr == (m_LogConsole = LogConsole::Create()))
		return E_FAIL;
    if (nullptr == (m_Hierarchy = Hierarchy::Create()))
        return E_FAIL;
    if (nullptr == (m_ModelViewer = ModelViewer::Create()))
        return E_FAIL;
	if (nullptr == (m_AnimationPresetEditor = AnimationPresetEditor::Create()))
		return E_FAIL;
    if (nullptr == (m_AssetBrowser = AssetBrowser::Create()))
        return E_FAIL;
    if (nullptr == (m_NavHelper = NavHelper::Create()))
        return E_FAIL;

	return S_OK;
}

void EditorManager::Update(Bool IsResetView) {
    Float timeDelta = GAME_INSTANCE->Compute_UnscaledTimeDelta();
    EDITOR_STATE state = EDITOR->Get_State();

    if (state == EDITOR_STATE::PLAY) {

        GAME_INSTANCE->Update_Engine();
    }
    else {
        if (m_EditorCamera) {
            GAME_INSTANCE->Update_Input();
            m_EditorCamera->Update(timeDelta);
            GAME_INSTANCE->Submit_RenderGroup();
        }
        if (GAME_INSTANCE->Get_CurrentLevel())
			GAME_INSTANCE->Get_CurrentLevel()->Update_LoadLevel(0.016777f);
    }

    m_Inspector->Update(IsResetView);
	m_EditorView->Update(IsResetView);
    m_MenuBar->Update(IsResetView);
    m_LogConsole->Update(IsResetView);
    m_Hierarchy->Update(IsResetView);
    m_ModelViewer->Update(IsResetView);
	m_AnimationPresetEditor->Update(IsResetView);
    m_AssetBrowser->Update(IsResetView);
    m_NavHelper->Update(IsResetView);
}

HRESULT EditorManager::Render(Bool IsResetView) {

	Shared<Float4> vClearColor = make_shared<Float4>(0.f, 0.f, 1.f, 1.f);

    if (FAILED(GAME_INSTANCE->Begin_RenderOffScreen(0)))
        return E_FAIL;


    Shared<Camera> pCurrentMain = GAME_INSTANCE->Get_MainCamera();
    
    // 만약 엔진의 메인 카메라가 있고, 그게 에디터 카메라가 아니라면 우선적으로 채택
    if (pCurrentMain && pCurrentMain->Get_InstanceID() != m_EditorCamera->Get_InstanceID())
    {
        m_InGameCamera = pCurrentMain;
    }
    else
    {
        // 그렇지 않다면 엔진에 등록된 카메라들 중 에디터 카메라가 아닌 첫 번째 실제 게임 카메라를 찾음
        m_InGameCamera = nullptr;
        for (auto& pCam : GAME_INSTANCE->Get_Cameras(GAME_INSTANCE->Get_CurrentLevelIndex()))
        {
            if (pCam && pCam->Get_InstanceID() != m_EditorCamera->Get_InstanceID())
            {
                m_InGameCamera = pCam;
                break;
            }
        }
    }

    // 1. In-Game 뷰포트 (OffScreen 0) 바인딩 및 렌더링 준비
    if (m_InGameCamera)
    {
        GAME_INSTANCE->Set_MainCamera(m_InGameCamera);
        m_InGameCamera->Bind_CameraTransform();
    }
    else
    {
        // 인게임 카메라가 전혀 없는 경우: 검은 화면 출력을 위해 뷰포트 클리어 및 바인딩 건너뜀
        Shared<Float4> vBlack = make_shared<Float4>(0.f, 0.f, 0.f, 1.f);
        GAME_INSTANCE->Clear_BackBufferView(vBlack);
        // Bind_CameraTransform을 호출하지 않아 렌더링 결과가 나타나지 않음 (검은 화면)
    }

    GAME_INSTANCE->Update_Pipeline();

    if (FAILED(GAME_INSTANCE->Draw_NoClearing()))
        return E_FAIL;

    // 1. 인게임(Game Scene) 렌더링 직후 디버그 박스 렌더
#ifdef _DEBUG
    GAME_INSTANCE->Render_CollisionDebug();
#endif

    if (FAILED(GAME_INSTANCE->Begin_RenderOffScreen(1)))
        return E_FAIL;

    if (FAILED(GAME_INSTANCE->Set_MainCamera(m_EditorCamera)))
        return E_FAIL;
	if (FAILED(m_EditorCamera->Bind_EditorMatrix()))
        return E_FAIL;
    GAME_INSTANCE->Update_Pipeline();

    // 2. 에디터 화면(Editor Scene) 구조체 그리기
    if (FAILED(GAME_INSTANCE->Draw())) 
        return E_FAIL;

    // 에디터 화면 렌더링 직후 디버그 박스 렌더
#ifdef _DEBUG
    GAME_INSTANCE->Render_CollisionDebug();
#endif

    if (FAILED(GAME_INSTANCE->End_RenderOffScreen()))
        return E_FAIL;

    if (m_InGameCamera)
        GAME_INSTANCE->Set_MainCamera(m_InGameCamera);
    else
        GAME_INSTANCE->Set_MainCamera(m_EditorCamera);

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGuiWindowFlags window_flags = 
        ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | 
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground; // 배경 투명화
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Main Panel", nullptr, window_flags);
    {
        ImGui::PopStyleVar(3);
        // 7. 이 패널 공간 전체를 "도킹 가능한 구역(DockSpace)"으로 만듦
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
        }
    }
    ImGui::End();

    m_Inspector->Render(IsResetView);
	m_EditorView->Render(IsResetView);
    m_MenuBar->Render(IsResetView);
    m_LogConsole->Render(IsResetView);
    m_Hierarchy->Render(IsResetView);
    m_ModelViewer->Render(IsResetView);
	m_AnimationPresetEditor->Render(IsResetView);
    m_AssetBrowser->Render(IsResetView);
    m_NavHelper->Render(IsResetView);

    return S_OK;
}
