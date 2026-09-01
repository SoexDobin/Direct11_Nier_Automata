#pragma once
#include "Projectile.h"
#include "Entity.h"

NS_BEGIN(Engine)
class SphereCollider;
class Model;
class Shader;
NS_END

NS_BEGIN(Client)

class Entity;

class CLIENT_DLL Bullet final : public Projectile
{
public:
    typedef struct tagBulletDesc : public PROJECTILE_DESC
    {
        Entity::DAMAGE_INFO damageInfo{};
        wstring resourceTag{};
        wstring targetLayer{};
        Bool  isPermanent{ false };
        Float maxDistance{};       
        Float damage{};           
    } BULLET_DESC;

public:
    explicit Bullet();
    explicit Bullet(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
    explicit Bullet(const Bullet& rhs);
    ~Bullet() override = default;

public:
    Bool Is_Permanent() const { return m_Desc.isPermanent; }
    wstring Get_TargetLayerName() const { return m_Desc.targetLayer; }

public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(void* arg) override;
    void Update(Float timeDelta) override;
    void Late_Update(Float timeDelta) override;
    HRESULT Render() override;
    void Submit_RenderGroup() override;

    // 충돌 처리 (상쇄 로직 핵심)
    void OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;

private:
    HRESULT Ready_Components();
    HRESULT Bind_ShaderResources();

private:
    BULLET_DESC                 m_Desc{};
    Entity::DAMAGE_INFO	        m_DamageInfo{};
    Float                       m_TravelDistance{};

    Shared<SphereCollider>      m_Collider{ nullptr };
    Shared<Model>               m_Model{ nullptr };
    Shared<Shader>              m_Shader{ nullptr };
    
public:
    static Shared<Bullet> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
    Shared<GameObject> Clone(void* arg = nullptr) override;
};
NS_END
