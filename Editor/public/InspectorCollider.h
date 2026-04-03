#pragma once
#include "EditorObject.h"
NS_BEGIN(Engine)
class Component;
class OBBCollider;
class SphereCollider;
NS_END
NS_BEGIN(Editor)
class InspectorCollider final : public EditorObject
{
public:
	InspectorCollider() = default;
	~InspectorCollider() override = default;
public:
	HRESULT Initialize() override;
	void Update(Bool isResize) override {};
	void Render(Bool isResize) override {}
	void RenderComponent(const std::shared_ptr<Engine::Component>& pCollider);
private:
	void RenderOBB(const std::shared_ptr<Engine::OBBCollider>& pOBB);
	void RenderSphere(const std::shared_ptr<Engine::SphereCollider>& pSphere);
public:
	static std::shared_ptr<InspectorCollider> Create();
};
NS_END