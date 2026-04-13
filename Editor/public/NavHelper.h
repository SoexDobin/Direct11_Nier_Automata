#pragma once
#include "EditorObject.h"

NS_BEGIN(Editor)

class NavHelper final : public EditorObject 
{
public:
    NavHelper();
    ~NavHelper() override;
    HRESULT Initialize() override;

public:
    void Update(Bool isResize) override;
    void Render(Bool isResize) override;

private:
    rcConfig m_Config {};
    Shared<Model> m_TargetModel{ nullptr };
    vector<NavCellData> m_PreviewData;
    string m_SaveName{ "New_Navigation" };

    void BakePreview();
    void SaveBinary();
};

NS_END