#include "Renderer.h"
#include "Game.h"
#include "GameObject.h"

Renderer::Renderer(const ComPtr<ID3D11Device> &device,
                   const ComPtr<ID3D11DeviceContext> &context)
    : m_Device(device), m_Context(context) {}

void Renderer::Add_RenderGroup(RENDERGROUP renderGroup, const Shared<GameObject>& gameObject) {
    if (gameObject->Get_Parent()) return;

	m_RenderGroup[ETOI(renderGroup)].push_back(gameObject);
}

void Renderer::Draw() {
	for (uint32 i = 0; i < ETOI(RENDERGROUP::END); ++i) {
		Render_Group(i);
        m_RenderGroup[i].clear();
	}
}

void Renderer::Draw_NoClearing() {
    for (uint32 i = 0; i < ETOI(RENDERGROUP::END); ++i) {
        Render_Group(i);
    }
}

// TODO : draw call check clear group all frame
HRESULT Renderer::Clear_RenderGroup()
{
    m_LayerMask = ETOI(LAYER::ALL_LAYER);

    for (auto& group : m_RenderGroup)
        group.shrink_to_fit();

    return S_OK;
}

HRESULT Renderer::Initialize(void *arg) {
  m_LayerMask = ETOI(LAYER::ALL_LAYER);

  return EngineManager::Initialize(arg);
}

void Renderer::On_Destroy() {
  for (auto &group : m_RenderGroup)
    group.clear();
}

void Renderer::On_Disable() { EngineManager::On_Disable(); }

void Renderer::On_Enable() { EngineManager::On_Enable(); }

void Renderer::Set_Active(Bool isActive) {
  EngineManager::Set_Active(isActive);
}

void Renderer::Render_Group(uint32 groupIndex) const
{
	for (auto& object : m_RenderGroup[groupIndex])
	{
		uint32 objLayer = object->Get_LayerMask().Get_Layer();
		if (objLayer != 0 && !(m_LayerMask & objLayer))
			continue;
		if (!object->Is_Active() || object->Is_Destroy())
			continue;

		Render_Recursive(object);
	}

	if (ETOI(RENDERGROUP::UI) == groupIndex)
		GAME_INSTANCE->Set_DepthStencilState(nullptr, 0);
}
void Renderer::Render_Recursive(const Shared<GameObject>& object) const
{
	object->Render();

	for (auto& child : object->Get_Children())
	{
		if (!child->Is_Active() || child->Is_Destroy())
			continue;
		Render_Recursive(child);
	}
}

Unique<Renderer> Renderer::Create(const ComPtr<ID3D11Device> &device,
                                  const ComPtr<ID3D11DeviceContext> &context) {
  auto renderer = make_unique<Renderer>(device, context);

  if (FAILED(renderer->Initialize(nullptr))) {
    MSG_BOX("Failed To Create Renderer");
    return nullptr;
  }

  return renderer;
}
