#pragma once
#include "EditorObject.h"
#include <nlohmann/json.hpp>

NS_BEGIN(Editor)

class MetadataManager final : public EditorObject 
{
	NO_COPY(MetadataManager)
public:
  struct MemberInfo {
    string name;
    string type;
    string defaultValue;
    string access;
  };

  struct ClassInfo {
    string nameSpace;
    string baseClass;
    string category; /* "gameobject" | "level" | "component" | "other" */
    bool hasCreate = false;
    bool hasClone = false;
    vector<MemberInfo> members;          /* own */
    vector<MemberInfo> inheritedMembers; /* from parents */
    vector<MemberInfo> transformMembers; /* for gameobject category */
  };

public:
  MetadataManager();
  ~MetadataManager() override;

public:
  HRESULT Initialize() override;
  void Update() override {}
  void Render() override {}

public:
  HRESULT LoadMetadata(const wstring &jsonPath);
  const map<string, ClassInfo> &GetClasses() const { return m_Classes; }
  const ClassInfo* FindClass(const string& className) const;

  /* 카테고리별 필터 */
  vector<pair<string, const ClassInfo *>>
  GetByCategory(const string &category) const;

public:
  static Shared<MetadataManager> Create();

private:
  static void ParseMemberArray(const nlohmann::json& arr, vector<MemberInfo>& out);
  map<string, ClassInfo> m_Classes;
};

NS_END
