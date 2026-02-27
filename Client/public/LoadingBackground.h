#pragma once
#include "UIObject.h"

NS_BEGIN(Engine)
class VIBuffer;
class Texture;
class Shader;
NS_END

NS_BEGIN(Client)

class LoadingBackground final : public UIObject, public enable_shared_from_this<LoadingBackground> 
{
public:
    LoadingBackground();
    LoadingBackground(const ComPtr<ID3D11Device> &device, const ComPtr<ID3D11DeviceContext> &context);
    LoadingBackground(const Shared<LoadingBackground> &rhs);
    ~LoadingBackground() override = default;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(void *arg) override;
    void On_Destroy() override;

    void Priority_Update(Float timeDelta) override;
    void Update(Float timeDelta) override;
    void Late_Update(Float timeDelta) override;
    void Fixed_Update(Float fixedDelta) override;
    HRESULT Render() override;

private:
    HRESULT Ready_Components();

public:
    static Shared<LoadingBackground> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
    Shared<GameObject> Clone(void *arg) override;

    RTTR_ENABLE()
};

NS_END