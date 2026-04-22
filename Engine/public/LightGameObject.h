#pragma once
#include "GameObject.h"
#include "Light.h"

NS_BEGIN(Engine)

class ENGINE_DLL LightGameObject abstract : public GameObject
{
    RTTR_ENABLE(GameObject)
public:
    typedef struct tagLightObjectDesc : public GAMEOBJECT_DESC {
        LIGHT_DESC lightDesc;
    } LIGHT_OBJECT_DESC;

public:
    explicit LightGameObject() = default;
    explicit LightGameObject(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
    explicit LightGameObject(const LightGameObject& rhs);
    virtual ~LightGameObject() override = default;

public:
    GAMEOBJECTTYPE Get_GameObjectType() final { return GAMEOBJECTTYPE::LIGHT; }
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(void* arg) override;
    void On_Destroy() override;
    void On_Enable() override;
    void On_Disable() override;

public:
    void Priority_Update(Float timeDelta) override;
    void Update(Float timeDelta) override;
    void Late_Update(Float timeDelta) override;
    HRESULT Render() override { return S_OK; }
    const LIGHT_DESC& Get_LightDesc() const;
    LIGHT_DESC& Get_LightDesc_Ref();

private:
    Shared<Light> m_Light{ nullptr };

public:
    Shared<GameObject> Clone(void* arg) override PURE;
    
};

NS_END
