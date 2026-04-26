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

class CLIENT_DLL Bullet : public Projectile
{
    RTTR_ENABLE(Projectile)
public:
    typedef struct tagBulletDesc : public PROJECTILE_DESC
    {
        Vector3 scale{};
        Entity::DAMAGE_INFO damageInfo{};
        wstring resourceTag{};
        wstring targetLayer{};
        Bool  isPermanent{ false };
        Float maxDistance{};       
        Float damage{};           

        Bool useUpperSin{ false };
    } BULLET_DESC;

public:
    explicit Bullet();
    explicit Bullet(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
    explicit Bullet(const Bullet& rhs);
    virtual ~Bullet() override = default;

public:
    Bool Is_Permanent() const { return m_Desc.isPermanent; }
    wstring Get_TargetLayerName() const { return m_Desc.targetLayer; }

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* arg) override;
    virtual void Update(Float timeDelta) override;
    virtual void Late_Update(Float timeDelta) override;
    virtual HRESULT Render() override;
    virtual void Submit_RenderGroup() override;
    
    virtual void OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;

private:
    HRESULT Ready_Components();
    HRESULT Bind_ShaderResources();

protected:
    uint32                      m_TargetLayerIndex{};
    uint32                      m_MonsterLayerIndex{};
    uint32                      m_PlayerLayerIndex{};

    BULLET_DESC                 m_Desc{};
    Entity::DAMAGE_INFO	        m_DamageInfo{};
    Float                       m_TravelDistance{};

    Shared<SphereCollider>      m_Collider{ nullptr };
    Shared<Model>               m_Model{ nullptr };
    Shared<Shader>              m_Shader{ nullptr };

    Bool                        m_IsUpperSin{ false };
    Float                       m_AccTime{0.f};
    
public:
    static Shared<Bullet> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
    Shared<GameObject> Clone(void* arg = nullptr) override;
};
NS_END
