#include "pch.h"
#include "LoadingFadeOut.h"

#include <Game.h>
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "SpdLogger.h"

LoadingFadeOut::LoadingFadeOut() : UIObject() {}
LoadingFadeOut::LoadingFadeOut(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context)
    : UIObject(device, context) {}
LoadingFadeOut::LoadingFadeOut(const LoadingFadeOut& rhs) : UIObject(rhs) {}

HRESULT LoadingFadeOut::Initialize_Prototype() {
    return UIObject::Initialize_Prototype();
}

HRESULT LoadingFadeOut::Initialize(void* arg)
{
    if (nullptr == arg)
    {
        m_FadeSpeed = 0.25f;
    }
    else
    {
        FADE_OUT_DESC& desc = *static_cast<FADE_OUT_DESC*>(arg);
        m_FadeSpeed = desc.fadeSpeed;
    }

    UI_DESC desc{};
    desc.anchor = UI_ANCHOR::CENTER;
    desc.x = 0.f;
    desc.y = 0.f;
    desc.sizeX = 1920.f;
    desc.sizeY = 1920.f;

    m_Alpha = 1.f; // Alpha 1 (Black) to 0 (Clear)

    if (FAILED(UIObject::Initialize(&desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_Texture->Set_RGBA(Color{ 1.f, 1.f, 1.f, m_Alpha });
    
    // FadeOut usually starts active at the beginning of the level
    Set_Active(true);

    return S_OK;
}

void LoadingFadeOut::On_Destroy() { UIObject::On_Destroy(); }

void LoadingFadeOut::Update(Float timeDelta) 
{
    Update_UITransform();

    if (!Is_Active() || m_IsFadeEnd) return;

    m_Alpha = m_Alpha - (timeDelta * m_FadeSpeed);
    if (m_Alpha <= 0.f) 
    {
        m_Alpha = 0.f;
        m_IsFadeEnd = true;
    }
    m_Texture->Set_RGBA(Color{ 1.f, 1.f, 1.f, m_Alpha });
}

HRESULT LoadingFadeOut::Render() {
    if (!Is_Active()) return S_OK;

    if (FAILED(m_Transform->Bind_ShaderResource(m_Shader, WorldMatrix)))
        return E_FAIL;
    if (FAILED(Bind_ShaderResource(m_Shader, ViewMatrix, D3DTS::VIEW)))
        return E_FAIL;
    if (FAILED(Bind_ShaderResource(m_Shader, ProjMatrix, D3DTS::PROJ)))
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

void LoadingFadeOut::Submit_RenderGroup()
{
    GAME_INSTANCE->Add_RenderGroup(RENDERGROUP::UI, shared_from_this());
}

void LoadingFadeOut::Reset_Fade(Float fadeSpeed)
{
    m_IsFadeEnd = false;
    m_FadeSpeed = fadeSpeed;
    m_Alpha = 1.f;
    m_Texture->Set_RGBA(Color{ 1.f, 1.f, 1.f, m_Alpha });
}

HRESULT LoadingFadeOut::Ready_Components()
{
    auto shaderDesc = Shader::SHADER_DESC{ VTXTEX::Tag, VTXTEX::Elements, VTXTEX::numElements };
    m_Shader = Add_Component<Shader>(&shaderDesc);
    if (nullptr == m_Shader)
        return E_FAIL;

    auto textureDesc = Texture::TEXTURE_DESC{ ETOI(LEVEL::STATIC), L"UI_Loading_Fade" };
    m_Texture = Add_Component<Texture>(&textureDesc);
    if (nullptr == m_Texture)
        return E_FAIL;

    m_BufferRect = Add_Component<VIBuffer_Rect>();
    if (nullptr == m_BufferRect)
        return E_FAIL;

    return S_OK;
}

Shared<LoadingFadeOut> LoadingFadeOut::Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) {
    auto logo = make_shared<LoadingFadeOut>(device, context);

    if (FAILED(logo->Initialize_Prototype())) {
        LOG_ERROR(L"Failed To Create LoadingFadeOut");
        return nullptr;
    }

    return logo;
}

Shared<GameObject> LoadingFadeOut::Clone(void* arg) {
    auto logo = make_shared<LoadingFadeOut>(*this);

    if (FAILED(logo->Initialize(arg))) {
        LOG_ERROR(L"Failed To Create LoadingFadeOut Clone");
        return nullptr;
    }

    return logo;
}
