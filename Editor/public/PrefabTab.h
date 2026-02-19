#pragma once
#include "EditorObject.h"

NS_BEGIN(Editor)

class Inspector;

class PrefabTab final : public EditorObject {
  NO_COPY(PrefabTab)
public:
  PrefabTab();
  ~PrefabTab() override;

public:
  HRESULT Initialize() override;
  void Update() override {}
  void Render() override;

public:
  void Set_Inspector(const Shared<Inspector> &inspector) {
    m_Inspector = inspector;
  }

private:
  void RenderCategoryFolder(const char *label, const char *category);

private:
  Weak<Inspector> m_Inspector;
  string m_SelectedClass;
  float m_IconSize = 64.f;

public:
  static Shared<PrefabTab> Create();
};

NS_END
