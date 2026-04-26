#pragma once
#include "LightGameObject.h"

NS_BEGIN(Client)

class CLIENT_DLL AmusementParkLight final : public LightGameObject
{
    RTTR_ENABLE(LightGameObject)
public:
    typedef struct tagAmusementParkLightDesc : public LIGHT_OBJECT_DESC {
        Shared<GameObject> target{ nullptr };
        Vector3 offset{ 0.f, 0.f, 0.f }; 
    } AMUSEMENT_LIGHT_DESC;

public:
    explicit AmusementParkLight() = default;
    explicit AmusementParkLight(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
    explicit AmusementParkLight(const AmusementParkLight& rhs);
    ~AmusementParkLight() override = default;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(void* arg) override;
    void On_Destroy() override;
    void On_Enable() override;
    void On_Disable() override;

public:
    void Priority_Update(Float timeDelta) override;
    void Update(Float timeDelta) override;
    void Late_Update(Float timeDelta) override;

private:
    Weak<GameObject> m_Target{};
    Vector3 m_Offset{};

public:
    static Shared<AmusementParkLight> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
    Shared<GameObject> Clone(void* arg) override ;
};

NS_END

