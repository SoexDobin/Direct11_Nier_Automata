#pragma once
#include "Bullet.h"
#include "Entity.h"

NS_BEGIN(Engine)
class SphereCollider;
class Model;
class Shader;
NS_END

NS_BEGIN(Client)

class Entity;

class CLIENT_DLL HowitzerBullet final : public Bullet
{
    RTTR_ENABLE(Bullet)
public:
    typedef struct tagHowitzerBulletDesc : public BULLET_DESC
    {
        Bool  useCurvedFlight{ false };
        Float gravityStrength{ 0.f };
        Float targetY{ 0.f };
    } HOWITZER_BULLET_DESC;

public:
    explicit HowitzerBullet() = default;
    explicit HowitzerBullet(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
    explicit HowitzerBullet(const HowitzerBullet& rhs);
    ~HowitzerBullet() override = default;

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(void* arg) override;
    void Update(Float timeDelta) override;
    void Late_Update(Float timeDelta) override;
    HRESULT Render() override;
    void Submit_RenderGroup() override;

    void OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;

private:
    void InstanceExplodeEffect();

private:
    Float m_TargetY{ 0.f };
    Float m_GravityStrength{ 20.f };
    Vector3 m_CurvedVelocity{};

public:
    static Shared<HowitzerBullet> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
    Shared<GameObject> Clone(void* arg = nullptr) override;
};
NS_END
