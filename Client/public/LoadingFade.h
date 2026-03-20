#pragma once
#include "UIObject.h"

NS_BEGIN(Engine)
class Shader;
class Texture;
class VIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CLIENT_DLL LoadingFade final : public UIObject
{
    RTTR_ENABLE(UIObject)
public:
    typedef struct tagLoadingFade : public UI_DESC
    {
        Bool isHuman;
        Bool isFadeIn;
        Bool isFadeOut;
        Float fadeSpeed;
    } LOADING_FADE_UI_DESC;
public:
    explicit LoadingFade();
    explicit LoadingFade(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
    explicit LoadingFade(const LoadingFade& rhs);
    ~LoadingFade() override = default;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(void* arg) override;
    void On_Destroy() override;

    void Priority_Update(Float timeDelta) override;
    void Update(Float timeDelta) override;
    void Late_Update(Float timeDelta) override;
    void Fixed_Update(Float fixedDelta) override;
    HRESULT Render() override;
    void Submit_RenderGroup() override;

public:
    Bool Fade_End() const { return m_IsFadeEnd; }
    void Reset_FadeUI(Bool IsFadeIn, Float fadeSpeed);

protected:
    HRESULT Ready_Components();

private:
    Bool m_IsFadeEnd = { false };
    int32 m_FadeFlag{};
    Float m_FadeSpeed{};
    Float m_Alpha{};

    Shared<Texture> m_Texture{ nullptr };
    Shared<Shader> m_Shader{ nullptr };
    Shared<VIBuffer_Rect> m_BufferRect{ nullptr };

public:
    static Shared<LoadingFade> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
    Shared<GameObject> Clone(void* arg) override;

};

NS_END

