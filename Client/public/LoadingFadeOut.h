#pragma once
#include "UIObject.h"

NS_BEGIN(Engine)
class Shader;
class Texture;
class VIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CLIENT_DLL LoadingFadeOut final : public UIObject
{
public:
    typedef struct tagLoadingFadeOut : public UI_DESC
    {
        Float fadeSpeed = { 0.25f };
    } FADE_OUT_DESC;

public:
    explicit LoadingFadeOut();
    explicit LoadingFadeOut(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
    explicit LoadingFadeOut(const LoadingFadeOut& rhs);
    ~LoadingFadeOut() override = default;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(void* arg) override;
    void On_Destroy() override;

    void Update(Float timeDelta) override;
    HRESULT Render() override;
    void Submit_RenderGroup() override;

public:
    Bool Is_FadeFinished() const { return m_IsFadeEnd; }
    void Reset_Fade(Float fadeSpeed);

protected:
    HRESULT Ready_Components();

private:
    Bool m_IsFadeEnd = { false };
    Float m_FadeSpeed = { 0.25f };
    Float m_Alpha = { 1.f }; // Starts Black

    Shared<Texture> m_Texture{ nullptr };
    Shared<Shader> m_Shader{ nullptr };
    Shared<VIBuffer_Rect> m_BufferRect{ nullptr };

public:
    static Shared<LoadingFadeOut> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
    Shared<GameObject> Clone(void* arg) override;
};

NS_END
