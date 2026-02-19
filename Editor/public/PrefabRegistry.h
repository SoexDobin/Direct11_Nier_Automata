#pragma once
#include "EditorObject.h"

NS_BEGIN(Editor)

class PrefabRegistry final : public EditorObject 
{
	NO_COPY(PrefabRegistry)
public:
    struct OverrideEntry {
        string name;
        string type;
        string value;
    };

    struct PrefabData {
        string className;
        string nameSpace;
        string baseClass;
        vector<OverrideEntry> overrides;
    };

public:
  PrefabRegistry();
  ~PrefabRegistry() override;

public:
  HRESULT Initialize() override;
  void Update() override {}
  void Render() override {}

public:
  void Set_Override(const string &className, const string &memberName,
                    const string &type, const string &value);
  string Get_Override(const string &className, const string &memberName) const;
  const PrefabData *Get_Prefab(const string &className) const;
  PrefabData &GetOrCreate_Prefab(const string &className, const string &ns,
                                 const string &base);

public:
  void LoadFromDir(const wstring &dirPath);
  void SaveToDir(const wstring &dirPath) const;

public:
  static Shared<PrefabRegistry> Create();

private:
  map<string, PrefabData> m_Prefabs;
};

NS_END
