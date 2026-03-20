#include "pch.h"
#include "LoadingFade.h"

#include <Game.h>

#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"

#include "SpdLogger.h"

LoadingFade::LoadingFade() : UIObject() {}
LoadingFade::LoadingFade(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
    : UIObject(device, context) {}
LoadingFade::LoadingFade(const LoadingFade& rhs) : UIObject(rhs) {}

HRESULT LoadingFade::Initialize_Prototype() {
    return UIObject::Initialize_Prototype();
}

HRESULT LoadingFade::Initialize(void* arg)
{
    if (nullptr == arg)
    {
        LOG_ERROR(L"There is no desc for Fade UI");
        return E_FAIL;
    }

    LOADING_FADE_UI_DESC& desc = *static_cast<LOADING_FADE_UI_DESC*>(arg);
    desc.anchor = UI_ANCHOR::CENTER;
    desc.x = 0.f;
    desc.y = 0.f;
    desc.sizeX = 1920.f;
    desc.sizeY = 1920.f;

    m_FadeSpeed = desc.fadeSpeed;
    m_FadeFlag = true == desc.isFadeIn ? 1 : 0;

    if (FAILED(UIObject::Initialize(&desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (m_FadeFlag) m_Alpha = 0;
    else m_Alpha = 1.f;
    m_Texture->Set_RGBA(Color{ 1.f, 1.f, 1.f, m_Alpha });
	
	Set_Active(!desc.isHuman);

    return S_OK;
}

void LoadingFade::On_Destroy() { UIObject::On_Destroy(); }

void LoadingFade::Priority_Update(Float timeDelta) {
    
}

void LoadingFade::Update(Float timeDelta) 
{
    Update_UITransform();

    if (!Is_Active()) return;
    if (m_IsFadeEnd) return;
    if (m_FadeFlag)
    {
        m_Texture->Set_RGBA(Color{ 1.f, 1.f, 1.f, m_Alpha + (timeDelta / m_FadeSpeed) });
        if (m_Alpha >= 255.f) m_IsFadeEnd = true;
    }
    else
    {
        m_Texture->Set_RGBA(Color{ 1.f, 1.f, 1.f, m_Alpha - (timeDelta / m_FadeSpeed) });
        if (m_Alpha <= 0.f) m_IsFadeEnd = true;
    }
}

void LoadingFade::Late_Update(Float timeDelta) {
    
}

void LoadingFade::Fixed_Update(Float fixedDelta) {
    
}

HRESULT LoadingFade::Render() {
    if (!Is_Active()) return S_OK;

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

    return S_OK;;
}

void LoadingFade::Submit_RenderGroup()
{
    GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::UI, shared_from_this());
}

void LoadingFade::Reset_FadeUI(Bool IsFadeIn, Float fadeSpeed)
{
    m_IsFadeEnd = false;
    m_FadeFlag = true == IsFadeIn ? 1 : 0;
    m_FadeSpeed = fadeSpeed;

    if (m_FadeFlag) m_Alpha = 0;
    else m_Alpha = 1.f;
    m_Texture->Set_RGBA(Color{ 1.f, 1.f, 1.f, m_Alpha });
}

HRESULT LoadingFade::Ready_Components()
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

Shared<LoadingFade>
LoadingFade::Create(const ComPtr<ID3D11Device>& device,
    const ComPtr<ID3D11DeviceContext>& context) {
    auto logo = make_shared<LoadingFade>(device, context);

    if (FAILED(logo->Initialize_Prototype())) {
        LOG_ERROR(L"Failed To Create LoadingFade");
        return nullptr;
    }

    return logo;
}

Shared<GameObject> LoadingFade::Clone(void* arg) {
    auto logo = make_shared<LoadingFade>(*this);

    if (FAILED(logo->Initialize(arg))) {
        LOG_ERROR(L"Failed To CreateComponent LoadingFade");
        return nullptr;
    }

    return logo;
}
