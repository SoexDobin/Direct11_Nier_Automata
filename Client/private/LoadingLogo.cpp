#include "pch.h"
#include "LoadingLogo.h"

#include <Game.h>

#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"

#include "SpdLogger.h"

LoadingLogo::LoadingLogo() : UIObject() {}

LoadingLogo::LoadingLogo(const ComPtr<ID3D11Device> &device,
                         const ComPtr<ID3D11DeviceContext> &context)
    : UIObject(device, context) {}

LoadingLogo::LoadingLogo(const LoadingLogo& rhs) : UIObject(rhs) {}

HRESULT LoadingLogo::Initialize_Prototype() {
  return UIObject::Initialize_Prototype();
}

HRESULT LoadingLogo::Initialize(void *arg)
{
    UI_DESC desc{};
    desc.x = 100.f;
    desc.y = 100.f;
    desc.sizeX = 1600;
    desc.sizeY = 512.f;

    if (FAILED(UIObject::Initialize(&desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void LoadingLogo::On_Destroy() { UIObject::On_Destroy(); }

void LoadingLogo::Priority_Update(Float timeDelta) {
  UIObject::Priority_Update(timeDelta);
}

void LoadingLogo::Update(Float timeDelta) {
    Update_UITransform();
	UIObject::Update(timeDelta);
}

void LoadingLogo::Late_Update(Float timeDelta) {
  UIObject::Late_Update(timeDelta);
}

void LoadingLogo::Fixed_Update(Float fixedDelta) {
  UIObject::Fixed_Update(fixedDelta);
}

HRESULT LoadingLogo::Render() {
    if (FAILED(m_Transform->Bind_ShaderResource(m_Shader, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(Bind_ShaderResource(m_Shader, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;
    if (FAILED(Bind_ShaderResource(m_Shader, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;
    if (FAILED(m_Texture->Bind_ShaderResourceView(m_Shader, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_Shader->Begin(0)))
        return E_FAIL;
    if (FAILED(m_BufferRect->Bind_Resources()))
        return E_FAIL;
    if (FAILED(m_BufferRect->Render()))
        return E_FAIL;

    return S_OK;;
}

void LoadingLogo::Submit_RenderGroup()
{
    GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::UI, shared_from_this());
}

HRESULT LoadingLogo::Ready_Components()
{
    auto shaderDesc = Shader::SHADER_DESC{ VTXTEX::Tag, VTXTEX::Elements, VTXTEX::numElements };
    m_Shader = Add_Component<Shader>(&shaderDesc);
    if (nullptr == m_Shader)
        return E_FAIL;

    auto textureDesc = Texture::TEXTURE_DESC{ ETOI(LEVEL::STATIC), L"UI_Loading_Logo" };
    m_Texture = Add_Component<Texture>(&textureDesc);
    if (nullptr == m_Texture)
        return E_FAIL;

    m_BufferRect = Add_Component<VIBuffer_Rect>();
    if (nullptr == m_BufferRect)
        return E_FAIL;

    return S_OK;
}

Shared<LoadingLogo>
LoadingLogo::Create(const ComPtr<ID3D11Device> &device,
                    const ComPtr<ID3D11DeviceContext> &context) {
  auto logo = make_shared<LoadingLogo>(device, context);

  if (FAILED(logo->Initialize_Prototype())) {
    LOG_ERROR(L"Failed To Create LoadingLogo");
    return nullptr;
  }

  return logo;
}

Shared<GameObject> LoadingLogo::Clone(void *arg) {
  auto logo = make_shared<LoadingLogo>(*this);

  if (FAILED(logo->Initialize(arg))) {
    LOG_ERROR(L"Failed To CreateComponent LoadingLogo");
    return nullptr;
  }

  return logo;
}
