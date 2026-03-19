#include "pch.h"
#include "LoadingBackground.h"

#include <Transform.h>

#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "Game.h"

#include "SpdLogger.h"

LoadingBackground::LoadingBackground() : UIObject() {}

LoadingBackground::LoadingBackground(const ComPtr<ID3D11Device> &device,
                                     const ComPtr<ID3D11DeviceContext> &context)
    : UIObject{device, context} {}

LoadingBackground::LoadingBackground(const LoadingBackground& rhs)
    : UIObject{rhs} {}

HRESULT LoadingBackground::Initialize_Prototype() {

  return UIObject::Initialize_Prototype();
}

HRESULT LoadingBackground::Initialize(void *arg) {
    UI_DESC desc{};
    desc.anchor = UI_ANCHOR::CENTER;
    desc.x = 0.f;
    desc.y = 0.f;
    desc.sizeX = 1980.f;
    desc.sizeY = 1080.f;

    if (FAILED(UIObject::Initialize(&desc)))
        return E_FAIL;
    
    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void LoadingBackground::On_Destroy() { UIObject::On_Destroy(); }

void LoadingBackground::Priority_Update(Float timeDelta) {
	UIObject::Priority_Update(timeDelta);
}

void LoadingBackground::Update(Float timeDelta) {
    Update_UITransform();
    
	UIObject::Update(timeDelta);
}

void LoadingBackground::Late_Update(Float timeDelta) {
	UIObject::Late_Update(timeDelta);
}

void LoadingBackground::Fixed_Update(Float fixedDelta) {
    UIObject::Fixed_Update(fixedDelta);
    GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::UI, shared_from_this());
}

HRESULT LoadingBackground::Render() {
    if (FAILED(m_Transform->Bind_ShaderResource(m_Shader, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(Bind_ShaderResource(m_Shader, "g_ViewMatrix", D3DTS::VIEW)))
        return E_FAIL;
    if (FAILED(Bind_ShaderResource(m_Shader, "g_ProjMatrix", D3DTS::PROJ)))
        return E_FAIL;
    if (FAILED(m_Texture->Bind_ShaderResourceView(m_Shader, "g_Texture", 0)))
        return E_FAIL;
    if (FAILED(m_Shader->Bind_RawValue("g_RGBA", m_Texture->Get_RGBA_Absolute(), sizeof(Color))))
        return E_FAIL;
    
    if (FAILED(m_Shader->Begin(0)))
        return E_FAIL;
    if (FAILED(m_BufferRect->Bind_Resources()))
        return E_FAIL;
    if (FAILED(m_BufferRect->Render()))
        return E_FAIL;
	
    return S_OK;
}

void LoadingBackground::Submit_RenderGroup()
{
    GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::UI, shared_from_this());
}

HRESULT LoadingBackground::Ready_Components()
{
    auto shaderDesc = Shader::SHADER_DESC{ VTXTEX::Tag, VTXTEX::Elements, VTXTEX::numElements };
    m_Shader = Add_Component<Shader>(&shaderDesc);
    if (nullptr == m_Shader)
        return E_FAIL;

    auto textureDesc = Texture::TEXTURE_DESC{ ETOI(LEVEL::LOADING), L"UI_Loading_BackGround" };
    m_Texture = Add_Component<Texture>(&textureDesc);
    if (nullptr == m_Texture)
        return E_FAIL;

    m_BufferRect = Add_Component<VIBuffer_Rect>();
    if (nullptr == m_BufferRect)
        return E_FAIL;

    return S_OK;
}

Shared<LoadingBackground> LoadingBackground::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) {
  auto bg = make_shared<LoadingBackground>(device, context);

  if (FAILED(bg->Initialize_Prototype())) {
    LOG_ERROR(L"Failed To Create LoadingBackground");
    return nullptr;
  }

  return bg;
}

Shared<GameObject> LoadingBackground::Clone(void *arg) {
  auto bg = make_shared<LoadingBackground>(*this);

  if (FAILED(bg->Initialize(arg))) {
    LOG_ERROR(L"Failed To CreateComponent LoadingBackground");
    return nullptr;
  }

  return bg;
}
