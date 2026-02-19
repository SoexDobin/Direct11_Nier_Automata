#pragma once
#include "EditorObject.h"
#include "MetadataManager.h"

NS_BEGIN(Editor)

class PrefabRegistry;

class Inspector final : public EditorObject {
  NO_COPY(Inspector)
public:
  Inspector();
  ~Inspector() override;

public:
  HRESULT Initialize() override;
  void Update() override {}
  void Render() override;

public:
  void Set_SelectedClass(const string &className) {
    m_SelectedClass = className;
  }

private:
    void LayerTagGUI();
  void RenderMemberList(const vector<MetadataManager::MemberInfo> &members,
                        const Shared<PrefabRegistry> &registry,
                        const char *sectionId);

private:
  string m_SelectedClass;

public:
  static Shared<Inspector> Create();
};

NS_END