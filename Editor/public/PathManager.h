#pragma once

NS_BEGIN(Editor)

class PathManager {
  NO_COPY(PathManager)
public:
  static PathManager &GetInstance() {
    static PathManager instance;
    return instance;
  }
  void Initialize(const wstring &projectRoot = L"../../") {
    m_ProjectRoot = projectRoot;
    m_ProjectSettingDir = m_ProjectRoot + L"ProjectSetting/";
    m_PrefabSettingsDir = m_ProjectSettingDir + L"Prefab/";
    m_LayerSettingsPath = m_ProjectSettingDir + L"LayerSettings.json";
    m_TagSettingsPath = m_ProjectSettingDir + L"TagSettings.json";
    m_EngineDescSettingsPath = m_ProjectSettingDir + L"EngineDesc.json";
    m_SceneDataPath = m_ProjectSettingDir + L"SceneData.json";
    m_AssetsDir = m_ProjectRoot + L"Assets/";
    m_ShadersDir = m_AssetsDir + L"Shaders/";
    m_TexturesDir = m_AssetsDir + L"Textures/";
    m_ModelsDir = m_AssetsDir + L"Models/";
    m_ClientProjectDir = m_ProjectRoot + L"Client/";
    m_ClientPublicDir = m_ClientProjectDir + L"public/";
    m_ClientMetadataPath = m_ProjectRoot + L"client_metadata.json";
  }
  const wstring& GetProjectRoot() const { return m_ProjectRoot; }
  const wstring& GetProjectSettingDir() const { return m_ProjectSettingDir; }
  const wstring& GetLayerSettingsPath() const { return m_LayerSettingsPath; }
  const wstring& GetTagSettingsPath() const { return m_TagSettingsPath; }
  const wstring& GetRenderSettingsPath() const { return m_EngineDescSettingsPath; }
  const wstring& GetSceneDataPath() const { return m_SceneDataPath; }

  const wstring& GetAssetsDir() const { return m_AssetsDir; }
  const wstring& GetShadersDir() const { return m_ShadersDir; }
  const wstring& GetTexturesDir() const { return m_TexturesDir; }
  const wstring& GetModelsDir() const { return m_ModelsDir; }
  const wstring& GetPrefabSettingsDir() const { return m_PrefabSettingsDir; }
  const wstring& GetClientProjectDir() const { return m_ClientProjectDir; }
  const wstring& GetClientPublicDir() const { return m_ClientPublicDir; }
  const wstring& GetClientMetadataPath() const { return m_ClientMetadataPath; }
  void SetProjectRoot(const wstring &root) { Initialize(root); }

private:
  PathManager() = default;
  ~PathManager() = default;
  wstring m_ProjectRoot = L"../../";
  wstring m_ProjectSettingDir = L"../../ProjectSetting/";

  wstring m_LayerSettingsPath = L"../../ProjectSetting/LayerSettings.json";
  wstring m_TagSettingsPath = L"../../ProjectSetting/TagSettings.json";
  wstring m_EngineDescSettingsPath = L"../../ProjectSetting/EngineDesc.json";
  wstring m_SceneDataPath = L"../../ProjectSetting/SceneData.json";
  wstring m_AssetsDir = L"../../Assets/";
  wstring m_ShadersDir = L"../../Assets/Shaders/";
  wstring m_TexturesDir = L"../../Assets/Textures/";
  wstring m_ModelsDir = L"../../Assets/Models/";
  wstring m_PrefabSettingsDir = L"../../ProjectSetting/Prefab/";
  wstring m_ClientProjectDir = L"../../Client/";
  wstring m_ClientPublicDir = L"../../Client/public/";
  wstring m_ClientMetadataPath = L"../../ProjectSetting/client_metadata.json";
};
#define PATH Editor::PathManager::GetInstance()

NS_END
