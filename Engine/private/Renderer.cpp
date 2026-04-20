#include "Renderer.h"
#include "Game.h"
#include "GameObject.h"
#include "VIBuffer_Rect.h"
#include "SpdLogger.h"
#include "../../Client/public/Client_Define.h"

Renderer::Renderer(const ComPtr<ID3D11Device> &device,
                   const ComPtr<ID3D11DeviceContext> &context)
    : m_Device(device), m_Context(context) {}

void Renderer::Add_RenderGroup(RENDERGROUP renderGroup, const Shared<GameObject>& gameObject) {
    if (gameObject->Get_Parent()) return;

	m_RenderGroup[ETOI(renderGroup)].push_back(gameObject);
}

void Renderer::Draw() {
	Render_Group(ETOI(RENDERGROUP::PRIORITY));

	Render_Group(ETOI(RENDERGROUP::NONLIGHT));

	if (FAILED(GAME_INSTANCE->Begin_MultiRenderTarget(MRT_GameObject)))
		return;
	Render_Group(ETOI(RENDERGROUP::NONBLEND));
	if (FAILED(GAME_INSTANCE->End_MultiRenderTarget()))
		return;

	Render_Lights();
	Render_Combined();

	Render_Group(ETOI(RENDERGROUP::BLEND));
	Render_Group(ETOI(RENDERGROUP::WORLDUI));
	Render_Group(ETOI(RENDERGROUP::UI));

#ifdef _DEBUG
	Render_Debug();
#endif

	for (uint32 i = 0; i < ETOI(RENDERGROUP::END); ++i) {
        m_RenderGroup[i].clear();
	}
}

void Renderer::Draw_NoClearing() {
	Render_Group(ETOI(RENDERGROUP::PRIORITY));
	Render_Group(ETOI(RENDERGROUP::NONLIGHT));
	
	if (FAILED(GAME_INSTANCE->Begin_MultiRenderTarget(MRT_GameObject)))
		return;
	Render_Group(ETOI(RENDERGROUP::NONBLEND));
	if (FAILED(GAME_INSTANCE->End_MultiRenderTarget()))
		return;

	Render_Lights();
	Render_Combined();
	Render_Group(ETOI(RENDERGROUP::BLEND));
	Render_Group(ETOI(RENDERGROUP::WORLDUI));
	Render_Group(ETOI(RENDERGROUP::UI));
}

// TODO : draw call check clear group all frame
HRESULT Renderer::Clear_RenderGroup()
{
    m_LayerMask = ETOI(LAYER::ALL_LAYER);

    for (auto& group : m_RenderGroup)
        group.shrink_to_fit();

    return S_OK;
}

HRESULT Renderer::OnResize(uint32 width, uint32 height)
{
	if (width == 0 || height == 0) return S_OK;

	m_WorldMatrix = Matrix::CreateScale(static_cast<Float>(width), static_cast<Float>(height), 1.f);
	m_ProjMatrix = Matrix::CreateOrthographic(static_cast<Float>(width), static_cast<Float>(height), 0.f, 1.f);

	return S_OK;
}

HRESULT Renderer::Initialize(void *arg) {
	m_LayerMask = ETOI(LAYER::ALL_LAYER);

	if (FAILED(EngineManager::Initialize(arg)))
	{
		LOG_ERROR(L"Failed to Initialize Renderer");
		return E_FAIL;
	}

	uint32			numViewports = { 1 };
	D3D11_VIEWPORT	viewportDesc{};

	m_Context->RSGetViewports(&numViewports, &viewportDesc);
	if (FAILED(GAME_INSTANCE->Add_RenderTarget(RT_DIFFUSE, viewportDesc.Width, viewportDesc.Height,
		DXGI_FORMAT_R8G8B8A8_UNORM, Vector4::Zero)))
	return E_FAIL;
	if (FAILED(GAME_INSTANCE->Add_RenderTarget(RT_NORMAL, viewportDesc.Width, viewportDesc.Height,
		DXGI_FORMAT_R16G16B16A16_UNORM, Vector4::Zero)))
	return E_FAIL;
	if (FAILED(GAME_INSTANCE->Add_RenderTarget(RT_DEPTH, viewportDesc.Width, viewportDesc.Height,
		DXGI_FORMAT_R32G32B32A32_FLOAT, Vector4::Zero)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Add_RenderTarget(RT_SPECULAR, viewportDesc.Width, viewportDesc.Height,
		DXGI_FORMAT_R16G16B16A16_UNORM, Vector4::Zero)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Add_RenderTarget(RT_SHADE, viewportDesc.Width, viewportDesc.Height,
		DXGI_FORMAT_R16G16B16A16_UNORM, Vector4::Zero)))
	return E_FAIL;

	if (FAILED(GAME_INSTANCE->Add_MultiRenderTarget(MRT_GameObject, RT_DIFFUSE)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Add_MultiRenderTarget(MRT_GameObject, RT_NORMAL)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Add_MultiRenderTarget(MRT_GameObject, RT_DEPTH)))
		return E_FAIL;

	// Light Acc
	if (FAILED(GAME_INSTANCE->Add_MultiRenderTarget(MRT_LIGHT, RT_SHADE)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Add_MultiRenderTarget(MRT_LIGHT, RT_SPECULAR)))
		return E_FAIL;

	m_Buffer = VIBuffer_Rect::Create(m_Device, m_Context);
	m_Shader = Shader::Create(m_Device, m_Context, L"../../Engine/bin/shaders/DeferredShader.hlsl", VTXTEX::Elements, VTXTEX::numElements);

	m_WorldMatrix = Matrix::CreateScale(viewportDesc.Width, viewportDesc.Height, 1.f);
	m_ViewMatrix = Matrix::Identity;
	m_ProjMatrix = Matrix::CreateOrthographic(viewportDesc.Width, viewportDesc.Height, 0.f, 1.f);

#ifdef _DEBUG
	if (FAILED(GAME_INSTANCE->Ready_RenderTarget_Debug(RT_DIFFUSE, 150.f, 150.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Ready_RenderTarget_Debug(RT_NORMAL, 150.f, 450.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Ready_RenderTarget_Debug(RT_DEPTH, 150.f, 750.f, 300.f, 300.f)))
		return E_FAIL;

	if (FAILED(GAME_INSTANCE->Ready_RenderTarget_Debug(RT_SHADE, 450.f, 150.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(GAME_INSTANCE->Ready_RenderTarget_Debug(RT_SPECULAR, 450.f, 450.f, 300.f, 300.f)))
		return E_FAIL;
#endif

	return S_OK;
}

void Renderer::On_Destroy() {
  for (auto &group : m_RenderGroup)
    group.clear();
}

void Renderer::On_Disable() { EngineManager::On_Disable(); }

void Renderer::On_Enable() { EngineManager::On_Enable(); }

void Renderer::Set_Active(Bool isActive) { EngineManager::Set_Active(isActive); }

void Renderer::Render_Combined() const
{
	if(FAILED(m_Shader->Bind_Matrix(WorldMatrix, &m_WorldMatrix)))
		return;

	if (FAILED(m_Shader->Bind_Matrix(ViewMatrix, &m_ViewMatrix)))
		return;

	if (FAILED(m_Shader->Bind_Matrix(ProjMatrix, &m_ProjMatrix)))
		return;

	if (FAILED(GAME_INSTANCE->Bind_RenderTarget_ShaderResource(m_Shader, DiffuseMap, RT_DIFFUSE)))
		return;
	if (FAILED(GAME_INSTANCE->Bind_RenderTarget_ShaderResource(m_Shader, ShadeMap, RT_SHADE)))
		return;
	if (FAILED(GAME_INSTANCE->Bind_RenderTarget_ShaderResource(m_Shader, SpecularMap, RT_SPECULAR)))
		return;

	m_Shader->Begin(ETOI(DEFERRED::COMBINED));
	m_Buffer->Bind_Resources();
	m_Buffer->Render();

	
	m_Shader->Bind_SRV(DiffuseMap, nullptr);
	m_Shader->Bind_SRV(ShadeMap, nullptr);
	m_Shader->Bind_SRV(SpecularMap, nullptr);
	m_Shader->Begin(ETOI(DEFERRED::COMBINED));
}

void Renderer::Render_Lights() const
{
	if (FAILED(GAME_INSTANCE->Begin_MultiRenderTarget(MRT_LIGHT)))
		return;

	if (FAILED(m_Shader->Bind_Matrix(WorldMatrix, &m_WorldMatrix)))
		return;
	if (FAILED(m_Shader->Bind_Matrix(ViewMatrix, &m_ViewMatrix)))
		return;
	if (FAILED(m_Shader->Bind_Matrix(ProjMatrix, &m_ProjMatrix)))
		return;
	if (FAILED(m_Shader->Bind_Matrix(InverseViewMatrix, GAME_INSTANCE->Get_RawInvTransform(D3DTS::VIEW))))
		return;
	if (FAILED(m_Shader->Bind_Matrix(InverseProjMatrix, GAME_INSTANCE->Get_RawInvTransform(D3DTS::PROJ))))
		return;

	if (FAILED(m_Shader->Bind_RawValue(CameraPosition, GAME_INSTANCE->Get_RawCamTransform(), sizeof(Vector4))))
		return;

	if (FAILED(GAME_INSTANCE->Bind_RenderTarget_ShaderResource(m_Shader, NormalMap, RT_NORMAL)))
		return;
	if (FAILED(GAME_INSTANCE->Bind_RenderTarget_ShaderResource(m_Shader, DepthMap, RT_DEPTH)))
		return;

	if (FAILED(m_Buffer->Bind_Resources()))
		return;

	if (FAILED(GAME_INSTANCE->Render_Lights(m_Shader, m_Buffer)))
		return;

	if (FAILED(GAME_INSTANCE->End_MultiRenderTarget()))
		return;

	m_Shader->Bind_SRV(NormalMap, nullptr);
	m_Shader->Bind_SRV(DepthMap, nullptr);
	m_Shader->Begin(0);
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

Unique<Renderer> Renderer::Create(const ComPtr<ID3D11Device> &device, const ComPtr<ID3D11DeviceContext> &context) {
	auto renderer = make_unique<Renderer>(device, context);

	if (FAILED(renderer->Initialize(nullptr))) {
		MSG_BOX("Failed To Create Renderer");
		return nullptr;
	}

	return renderer;
}


#ifdef _DEBUG
void Renderer::Render_Debug()
{
	if (FAILED(m_Shader->Bind_Matrix(ViewMatrix, &m_ViewMatrix)))
		return;

	if (FAILED(m_Shader->Bind_Matrix(ProjMatrix, &m_ProjMatrix)))
		return;

	if (FAILED(GAME_INSTANCE->Render_RenderTarget_Debug(m_Buffer, m_Shader, MRT_GameObject)))
		return;

	if (FAILED(GAME_INSTANCE->Render_RenderTarget_Debug(m_Buffer, m_Shader, MRT_LIGHT)))
		return;
}
#endif
