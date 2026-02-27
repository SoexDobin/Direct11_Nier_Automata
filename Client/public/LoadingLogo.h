#pragma once
#include "UIObject.h"

NS_BEGIN(Engine)
class Shader;
class Texture;
class VIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class LoadingLogo : public UIObject
{
public:
    LoadingLogo();
    LoadingLogo(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
    LoadingLogo(const Shared<LoadingLogo>& rhs);
    ~LoadingLogo() override = default;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(void* arg) override;
    void On_Destroy() override;

    void Priority_Update(Float timeDelta) override;
    void Update(Float timeDelta) override;
    void Late_Update(Float timeDelta) override;
    void Fixed_Update(Float fixedDelta) override;
    HRESULT Render() override;

public:
    static Shared<LoadingLogo> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
    Shared<GameObject> Clone(void* arg) override;
};

NS_END