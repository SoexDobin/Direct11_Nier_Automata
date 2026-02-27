#include "MenuBar.h"
#include "PathManager.h"
#include "pch.h"


MenuBar::MenuBar() {}

HRESULT MenuBar::Initialize() {
  m_Enable = false;
  if (FAILED(Load_EngineDesc()))
    return E_FAIL;
  return EditorObject::Initialize();
}
void MenuBar::Update() { EditorObject::Update(); }
void MenuBar::Render() {
  EditorObject::Render();
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("WindowSetting")) {
      if (ImGui::MenuItem("EngineDesc", nullptr, m_Enable))
        m_Enable = !m_Enable;
      ImGui::Separator();
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
  if (m_Enable) {
    ViewEngineDesc();
  }
}
void MenuBar::ViewEngineDesc() {
  if (ImGui::Begin("Viewport Configuration", &m_Enable,
                   ImGuiWindowFlags_AlwaysAutoResize)) {
    ImGui::Text("Client Settings");
    ImGui::Separator();

    ImGui::InputText("Window Title", m_Title, MAXCHAR);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    // Use InputScalar for unsigned int types
    ImGui::InputScalar("Viewport Width", ImGuiDataType_U32,
                       &m_EngineDesc.viewportWidth, nullptr, nullptr, "%u");
    ImGui::InputScalar("Viewport Height", ImGuiDataType_U32,
                       &m_EngineDesc.viewportHeight, nullptr, nullptr, "%u");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::InputScalar("Start Level", ImGuiDataType_U32, &m_EngineDesc.levCount,
                       nullptr, nullptr, "%u");
    ImGui::Spacing();
    // Save Button
    if (ImGui::Button("Save Settings", ImVec2(-FLT_MIN, 0))) {
      if (SUCCEEDED(Save_EngineDesc())) {
          LOG_INFO("Save Engine Desc file");
      }
    }
  }
  ImGui::End();
}
HRESULT MenuBar::Load_EngineDesc() {
  try {
    string path = Helper::To_String(PATH.GetRenderSettingsPath());
    ifstream file(path);
    if (!file.is_open()) {
      // Set defaults if file is missing
      m_EngineDesc.windowTitle = L"client";
      m_EngineDesc.viewportWidth = 1280;
      m_EngineDesc.viewportHeight = 720;
      m_EngineDesc.levCount = 0;
      strcpy_s(m_Title, "client");
      return Save_EngineDesc();
    }
    nlohmann::json json;
    file >> json;
    string title = json.value("windowTitle", "client");
    m_EngineDesc.windowTitle = Helper::To_wString(title);
    m_EngineDesc.viewportWidth = json.value("viewportWidth", 1280u);
    m_EngineDesc.viewportHeight = json.value("viewportHeight", 720u);
    m_EngineDesc.levCount = json.value("levCount", 0u);

    // Sync m_Title buffer from loaded windowTitle
    strcpy_s(m_Title, title.c_str());
  } catch (const std::exception &e) {
    LOG_WARN(L"Load Failed: {}", Helper::To_wString(e.what()));
    m_EngineDesc.windowTitle = L"client";
    m_EngineDesc.viewportWidth = 1280;
    m_EngineDesc.viewportHeight = 720;
    m_EngineDesc.levCount = 0;
    strcpy_s(m_Title, "client");
    return Save_EngineDesc();
  }
  return S_OK;
}
HRESULT MenuBar::Save_EngineDesc() {
  try {
    string dirPath = Helper::To_String(PATH.GetProjectSettingDir());
    string filePath = Helper::To_String(PATH.GetRenderSettingsPath());
    if (!filesystem::exists(dirPath)) {
      filesystem::create_directories(dirPath);
    }
    std::ofstream file(filePath);
    if (!file.is_open()) {
      LOG_CRITICAL(L"Failed To Open EngineDesc");
      return E_FAIL;
    }
    // Sync m_Title to m_EngineDesc before saving
    m_EngineDesc.windowTitle = Helper::To_wString(m_Title);

    nlohmann::json json;
    json["windowTitle"] = string(m_Title);
    json["viewportWidth"] = m_EngineDesc.viewportWidth;
    json["viewportHeight"] = m_EngineDesc.viewportHeight;
    json["levCount"] = m_EngineDesc.levCount;
    file << json.dump(4);
  } catch (const std::exception &e) {
    LOG_CRITICAL(L"Save Failed: {}", Helper::To_wString(e.what()));
    return E_FAIL;
  }
  return S_OK;
}
Shared<MenuBar> MenuBar::Create() {
  auto menuBar = make_shared<MenuBar>();

  if (FAILED(menuBar->Initialize())) {
    MSG_BOX("Failed To Create MenuBar");
    return nullptr;
  }

  return menuBar;
}