#include "MetadataManager.h"
#include "SpdLogger.h"
#include "String_Helper.h"
#include "pch.h"
#include <fstream>


MetadataManager::MetadataManager() {}
MetadataManager::~MetadataManager() {}

HRESULT MetadataManager::Initialize() { return EditorObject::Initialize(); }

void MetadataManager::ParseMemberArray(const nlohmann::json &arr,
                                       vector<MemberInfo> &out) {
  for (auto& memberJson : arr) {
    MemberInfo member;
    member.name = memberJson.value("name", "");
    member.type = memberJson.value("type", "");
    member.defaultValue = memberJson.value("default", "");
    member.access = memberJson.value("access", "private");
    out.push_back(member);
  }
}

HRESULT MetadataManager::LoadMetadata(const wstring &jsonPath) {
  m_Classes.clear();

  std::ifstream file(jsonPath);
  if (!file.is_open()) {
    LOG_WARN(L"[MetadataManager] client_metadata.json not found");
    return S_OK;
  }

  try {
    nlohmann::json root;
    file >> root;
    file.close();

    if (!root.contains("version") || root["version"] != "1.0") {
      LOG_WARN(L"[MetadataManager] Unsupported metadata version");
      return E_FAIL;
    }

    if (!root.contains("classes"))
      return S_OK;

    for (auto &[className, classJson] : root["classes"].items()) {
      ClassInfo info;
      info.nameSpace = classJson.value("namespace", "");
      info.baseClass = classJson.value("base_class", "");
      info.category = classJson.value("category", "other");
      info.hasCreate = classJson.value("has_create", false);
      info.hasClone = classJson.value("has_clone", false);

      if (classJson.contains("members"))
        ParseMemberArray(classJson["members"], info.members);

      if (classJson.contains("inherited_members"))
        ParseMemberArray(classJson["inherited_members"], info.inheritedMembers);

      if (classJson.contains("transform_members"))
        ParseMemberArray(classJson["transform_members"], info.transformMembers);

      m_Classes[className] = std::move(info);
    }

    LOG_INFO(L"[MetadataManager] Loaded {} classes",
             Helper::To_wString(std::to_string(m_Classes.size())));
  } catch (const std::exception &e) {
    LOG_ERROR(L"[MetadataManager] JSON parse error: {}",
              Helper::To_wString(e.what()));
    return E_FAIL;
  }

  return S_OK;
}

const MetadataManager::ClassInfo *
MetadataManager::FindClass(const string &className) const {
  auto it = m_Classes.find(className);
  if (it != m_Classes.end())
    return &it->second;
  return nullptr;
}

vector<pair<string, const MetadataManager::ClassInfo *>>
MetadataManager::GetByCategory(const string &category) const {
  vector<pair<string, const ClassInfo *>> result;
  for (auto &[name, info] : m_Classes) {
    if (info.category == category)
      result.push_back({name, &info});
  }
  return result;
}

Shared<MetadataManager> MetadataManager::Create() {
  auto instance = make_shared<MetadataManager>();

  if (FAILED(instance->Initialize())) {
    MSG_BOX("Failed To Create MetadataManager");
    return nullptr;
  }

  return instance;
}
