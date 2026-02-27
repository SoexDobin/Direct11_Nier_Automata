#include "PrefabRegistry.h"
#include "SpdLogger.h"
#include "String_Helper.h"
#include "pch.h"
#include <filesystem>
#include <fstream>


PrefabRegistry::PrefabRegistry() {}
PrefabRegistry::~PrefabRegistry() {}

HRESULT PrefabRegistry::Initialize() { return EditorObject::Initialize(); }

void PrefabRegistry::Set_Override(const string &className,
                                  const string &memberName, const string &type,
                                  const string &value) {
  auto it = m_Prefabs.find(className);
  if (it == m_Prefabs.end())
    return;

  auto &overrides = it->second.overrides;

  for (auto &entry : overrides) {
    if (entry.name == memberName) {
      entry.value = value;
      return;
    }
  }

  overrides.push_back({memberName, type, value});
}

string PrefabRegistry::Get_Override(const string &className,
                                    const string &memberName) const {
  auto it = m_Prefabs.find(className);
  if (it == m_Prefabs.end())
    return "";

  for (auto &entry : it->second.overrides) {
    if (entry.name == memberName)
      return entry.value;
  }

  return "";
}

const PrefabRegistry::PrefabData *
PrefabRegistry::Get_Prefab(const string &className) const {
  auto it = m_Prefabs.find(className);
  if (it != m_Prefabs.end())
    return &it->second;
  return nullptr;
}

PrefabRegistry::PrefabData &
PrefabRegistry::GetOrCreate_Prefab(const string &className, const string &ns,
                                   const string &base) {
  auto it = m_Prefabs.find(className);
  if (it != m_Prefabs.end())
    return it->second;

  PrefabData data;
  data.className = className;
  data.nameSpace = ns;
  data.baseClass = base;
  m_Prefabs[className] = std::move(data);
  return m_Prefabs[className];
}

void PrefabRegistry::LoadFromDir(const wstring &dirPath) {
  m_Prefabs.clear();

  if (!std::filesystem::exists(dirPath))
    return;

  for (auto &entry : std::filesystem::directory_iterator(dirPath)) {
    if (entry.path().extension() != ".json")
      continue;

    std::ifstream file(entry.path());
    if (!file.is_open())
      continue;

    try {
      nlohmann::json root;
      file >> root;
      file.close();

      if (!root.contains("version") || root["version"] != "1.0")
        continue;

      PrefabData data;
      data.className = root.value("class", "");
      data.nameSpace = root.value("namespace", "");
      data.baseClass = root.value("base_class", "");

      if (root.contains("overrides")) {
        for (auto &ov : root["overrides"]) {
          OverrideEntry e;
          e.name = ov.value("name", "");
          e.type = ov.value("type", "");
          e.value = ov.value("value", "");
          data.overrides.push_back(e);
        }
      }

      if (!data.className.empty())
        m_Prefabs[data.className] = std::move(data);
    } catch (const std::exception &) {
    }
  }

  LOG_INFO(L"[PrefabRegistry] Loaded {} prefabs",
           Helper::To_wString(std::to_string(m_Prefabs.size())));
}

void PrefabRegistry::SaveToDir(const wstring &dirPath) const {
  std::filesystem::create_directories(dirPath);

  for (auto &[className, data] : m_Prefabs) {
    if (data.overrides.empty())
      continue;

    nlohmann::json root;
    root["version"] = "1.0";
    root["class"] = data.className;
    root["namespace"] = data.nameSpace;
    root["base_class"] = data.baseClass;

    auto &overridesArr = root["overrides"];
    for (auto &ov : data.overrides) {
      nlohmann::json ovJson;
      ovJson["name"] = ov.name;
      ovJson["type"] = ov.type;
      ovJson["value"] = ov.value;
      overridesArr.push_back(ovJson);
    }

    std::filesystem::path filePath =
        std::filesystem::path(dirPath) / (className + ".json");
    std::ofstream file(filePath);
    if (!file.is_open()) {
      LOG_ERROR(L"[PrefabRegistry] Failed to save: {}",
                Helper::To_wString(className));
      continue;
    }

    file << root.dump(2);
    file.close();
  }

  LOG_INFO(L"[PrefabRegistry] Saved {} prefabs",
           Helper::To_wString(std::to_string(m_Prefabs.size())));
}

Shared<PrefabRegistry> PrefabRegistry::Create() {
  auto instance = make_shared<PrefabRegistry>();

  if (FAILED(instance->Initialize())) {
    MSG_BOX("Failed To Create PrefabRegistry");
    return nullptr;
  }

  return instance;
}
