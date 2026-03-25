#pragma once
#include "EditorObject.h"

NS_BEGIN(Engine)
class Component;
NS_END

NS_BEGIN(Editor)

class InspectorCamera : public EditorObject
{
public:
	InspectorCamera() = default;
	~InspectorCamera() override = default;

public:
	HRESULT Initialize();
	void Render(Bool isResize) override {} 
	void RenderCamera(const std::shared_ptr<Engine::GameObject>& pObj);

public:
	static std::shared_ptr<InspectorCamera> Create();
};

NS_END
