#pragma once
#include <PartObject.h>

NS_BEGIN(Engine)
class Model;
class Shader;
NS_END

NS_BEGIN(Client)

class Pl0000Parts;

class CLIENT_DLL WeaponHalo final : public PartObject
{
    RTTR_ENABLE(PartObject)
public:
    typedef struct tagWeaponHaloDesc :public PARTOBJECT_DESC
    {
        Shared<Pl0000Parts> targetSheath; // 쫓아다닐 타겟 검집
    } WEAPON_HALO_DESC;
public:
    explicit WeaponHalo() = default;
    explicit WeaponHalo(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
    explicit WeaponHalo(const WeaponHalo& rhs);
    ~WeaponHalo() override = default;
    
public:
    HRESULT Initialize_Prototype() override;
    HRESULT Initialize(void* arg) override;
    void Update(Float timeDelta) override;
    void Late_Update(Float timeDelta) override;
    HRESULT Render() override;
    void Submit_RenderGroup() override;

    void OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;

private:
    HRESULT Ready_Components();
    HRESULT Bind_ShaderResources();

private:
    Shared<Model>               m_Model{ nullptr };
    Shared<Shader>              m_Shader{ nullptr };
    Weak<Pl0000Parts>           m_TargetSheath{};
    int32                       m_BoneIndex{-1};


public:
    static Shared<WeaponHalo> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
    Shared<GameObject> Clone(void* arg = nullptr) override;
};

NS_END