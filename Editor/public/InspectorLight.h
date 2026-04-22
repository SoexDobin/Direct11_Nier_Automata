#pragma once
#include "EditorObject.h"
NS_BEGIN(Engine)
class GameObject;
NS_END
NS_BEGIN(Editor)
class InspectorLight : public EditorObject
{
public:
    InspectorLight() = default;
    ~InspectorLight() override = default;
public:
    HRESULT Initialize();
    void Render(Bool isResize) override {}

    void RenderLight(const std::shared_ptr<Engine::GameObject>& pObj);
public:
    static std::shared_ptr<InspectorLight> Create();
};
NS_END

