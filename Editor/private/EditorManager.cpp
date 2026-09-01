#include "pch.h"
#include "EditorManager.h"

#include "EditorCamera.h"
#include "Game.h"
#include "GameObject.h"
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

namespace
{
Bool Belongs_To_Level(const Shared<GameObject>& object, uint32 levelIndex)
{
    if (!object)
        return false;

    const auto& objects = GAME_INSTANCE->Get_GameObjects(levelIndex);
    const auto it = objects.find(object->Get_InstanceID());
    return it != objects.end() && it->second == object;
}

Bool Has_CodeDefinedStructure(const Shared<GameObject>& object)
{
    if (!object || !object->Get_StableChildKey().empty())
        return true;

    for (const auto& child : object->Get_Children())
        if (child && !child->Get_StableChildKey().empty())
            return true;

    return false;
}

Bool Is_In_Subtree(const Shared<GameObject>& object, ObjectGuid rootGuid)
{
    for (Shared<GameObject> current = object; current; current = current->Get_Parent())
        if (current->Get_ObjectGuid() == rootGuid)
            return true;

    return false;
}

wstring Make_UniqueName(const Shared<GameObject>& object, uint32 levelIndex)
{
    const wstring baseName = object ? object->Get_Name() : L"GameObject";
    const auto& objects = GAME_INSTANCE->Get_GameObjects(levelIndex);

    for (uint32 suffix = 0; ; ++suffix)
    {
        const wstring candidate = suffix == 0
            ? baseName
            : baseName + L"_" + to_wstring(suffix);
        const Bool overlaps = ranges::any_of(objects, [&](const auto& entry)
        {
            return entry.second && entry.second != object && entry.second->Get_Name() == candidate;
        });
        if (!overlaps)
            return candidate;
    }
}
}

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

void EditorManager::Set_State(EDITOR_STATE state)
{
    m_State = state;
    if (state != EDITOR_STATE::PAUSE)
        m_SingleStepRequested = false;
}

void EditorManager::Request_SingleStep()
{
    if (m_State == EDITOR_STATE::PAUSE)
        m_SingleStepRequested = true;
}

void EditorManager::Queue_Destroy(ObjectGuid targetGuid, uint32 levelIndex)
{
    if (targetGuid.Is_Valid())
        m_PendingMutations.push_back({ MUTATION_TYPE::DESTROY, targetGuid, {}, levelIndex, {} });
}

void EditorManager::Queue_MoveToRoot(ObjectGuid targetGuid, uint32 levelIndex)
{
    if (targetGuid.Is_Valid())
        m_PendingMutations.push_back({ MUTATION_TYPE::MOVE_TO_ROOT, targetGuid, {}, levelIndex, {} });
}

void EditorManager::Queue_Reparent(ObjectGuid targetGuid, ObjectGuid parentGuid, uint32 levelIndex)
{
    if (targetGuid.Is_Valid() && parentGuid.Is_Valid())
        m_PendingMutations.push_back({ MUTATION_TYPE::REPARENT, targetGuid, parentGuid, levelIndex, {} });
}

void EditorManager::Queue_Spawn(const wstring& prototypeTag, uint32 levelIndex, ObjectGuid parentGuid)
{
    if (!prototypeTag.empty())
        m_PendingMutations.push_back({ MUTATION_TYPE::SPAWN, {}, parentGuid, levelIndex, prototypeTag });
}

Bool EditorManager::Apply_Mutation(const MUTATION_COMMAND& command)
{
    const uint32 currentLevelIndex = GAME_INSTANCE->Get_CurrentLevelIndex();
    if (command.levelIndex != 0 && command.levelIndex != currentLevelIndex)
    {
        LOG_WARN(L"[EditorMutation] Rejected stale level command for level {}", command.levelIndex);
        return false;
    }

    const Shared<GameObject> target = GAME_INSTANCE->Find(command.targetGuid);

    switch (command.type)
    {
    case MUTATION_TYPE::DESTROY:
    {
        if (!target || target->Is_Destroy() ||
            !Belongs_To_Level(target, command.levelIndex) ||
            !target->Get_StableChildKey().empty())
            return false;

        const Shared<GameObject> selected = Get_SelectedObject();
        if (selected && Is_In_Subtree(selected, command.targetGuid))
            Clear_SelectedObject();

        return SUCCEEDED(GAME_INSTANCE->Destroy(command.targetGuid));
    }

    case MUTATION_TYPE::MOVE_TO_ROOT:
        if (!target || target->Is_Destroy() ||
            !Belongs_To_Level(target, command.levelIndex) ||
            !target->Get_StableChildKey().empty())
            return false;
        return SUCCEEDED(target->Remove_Parent());

    case MUTATION_TYPE::REPARENT:
    {
        const Shared<GameObject> parent = GAME_INSTANCE->Find(command.parentGuid);
        if (!target || !parent || target->Is_Destroy() || parent->Is_Destroy() ||
            target == parent || !Belongs_To_Level(target, command.levelIndex) ||
            !Belongs_To_Level(parent, command.levelIndex) ||
            !target->Get_StableChildKey().empty() || Has_CodeDefinedStructure(parent))
            return false;

        return SUCCEEDED(target->Set_Parent(parent));
    }

    case MUTATION_TYPE::SPAWN:
    {
        Shared<GameObject> parent;
        if (command.parentGuid.Is_Valid())
        {
            parent = GAME_INSTANCE->Find(command.parentGuid);
            if (!parent || parent->Is_Destroy() ||
                !Belongs_To_Level(parent, command.levelIndex) || Has_CodeDefinedStructure(parent))
                return false;
        }

        Shared<GameObject> cloned = GAME_INSTANCE->Instantiate<GameObject>(
            command.prototypeTag, command.levelIndex);
        if (!cloned)
            return false;

        cloned->Set_Name(Make_UniqueName(cloned, command.levelIndex));
        if (parent && FAILED(cloned->Set_Parent(parent)))
        {
            GAME_INSTANCE->Destroy(cloned->Get_ObjectGuid());
            return false;
        }

        Set_SelectedObject(cloned);
        return true;
    }
    }

    return false;
}

void EditorManager::Flush_PendingMutations()
{
    if (m_PendingMutations.empty())
        return;

    vector<MUTATION_COMMAND> currentBatch;
    currentBatch.swap(m_PendingMutations);

    for (const MUTATION_COMMAND& command : currentBatch)
        if (!Apply_Mutation(command))
            LOG_WARN(L"[EditorMutation] Command rejected or failed");

    GAME_INSTANCE->Flush_DestroyedGameObjects();
}

void EditorManager::Update(Bool IsResetView) {
    const EDITOR_STATE state = Get_State();
    const Bool isLoading = GAME_INSTANCE->Get_CurrentLevelIndex() == ETOI(LEVEL::LOADING);
    const Bool singleStep = state == EDITOR_STATE::PAUSE && m_SingleStepRequested;
    m_SingleStepRequested = false;

    GAME_INSTANCE->Begin_Frame(state == EDITOR_STATE::PLAY || isLoading);
    const Float editorDelta = GAME_INSTANCE->Compute_UnscaledTimeDelta();

    Flush_PendingMutations();

    if (m_EditorCamera && state != EDITOR_STATE::PLAY)
        m_EditorCamera->Update(editorDelta);

    m_Inspector->Update(IsResetView);
	m_EditorView->Update(IsResetView);
    m_MenuBar->Update(IsResetView);
    m_LogConsole->Update(IsResetView);
    m_Hierarchy->Update(IsResetView);
    m_ModelViewer->Update(IsResetView);
	m_AnimationPresetEditor->Update(IsResetView);
    m_AssetBrowser->Update(IsResetView);
    m_NavHelper->Update(IsResetView);

    if (state == EDITOR_STATE::PLAY || isLoading || singleStep)
    {
        const Float runtimeDelta = singleStep
            ? GAME_INSTANCE->Get_FixedDeltaTime()
            : GAME_INSTANCE->Compute_TimeDelta();
        GAME_INSTANCE->Update_Engine(runtimeDelta, singleStep);
    }
    else
    {
        GAME_INSTANCE->Submit_RenderGroup();
    }
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
