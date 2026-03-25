#pragma once
#include "EditorObject.h"

NS_BEGIN(Engine)
class Component;
NS_END

NS_BEGIN(Editor)

class InspectorTexture : public EditorObject
{
public:
	InspectorTexture() = default;
	~InspectorTexture() override = default;

public:
	HRESULT Initialize();
	void Render(Bool isResize) override {} 
	void RenderComponent(const std::shared_ptr<Engine::Component>& pComp);

public:
	static std::shared_ptr<InspectorTexture> Create();
};

NS_END
