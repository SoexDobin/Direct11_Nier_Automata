#pragma once
#include "Em3000Parts.h"

NS_BEGIN(Engine)
class SphereCollider;
NS_END

NS_BEGIN(Client)

class CLIENT_DLL Em3000Body final : public PartObject
{
	RTTR_ENABLE(PartObject)
public:
	typedef struct tagEm3000BodyDesc : public PARTOBJECT_DESC
	{

	} EM3000BODY_DESC;

public:
	typedef struct tagProjectilePortDesc
	{
		Vector3 localPosition{};
		Vector3 direction{};
		Vector3 muzzlePosition{};
	} EM3000_PROJECTILE_PORT;

public:
	explicit Em3000Body();
	explicit Em3000Body(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Em3000Body(const Em3000Body& rhs);
	~Em3000Body() override = default;

public:
	const TRANSFORM_FRAME& Get_ModelTransform() const;
	Shared<Model> Get_ModelComponent() { return m_Model; }
	virtual void Set_Animation(uint32 animIndex, Float blendDuration, Bool isLoop);
	uint32 Get_CurrentAnimationIndex() const;
	uint32 Get_NextAnimationIndex() const;
	Float Get_AnimationProgress() const;
	Bool Is_AnimationFinished() const;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	HRESULT Begin() override;
	void On_Destroy() override;

public:
	void Priority_Update(Float timeDelta) override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void Fixed_Update(Float fixedDelta) override;
	HRESULT Render() override;
	void Submit_RenderGroup() override;

public:
	void OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;
	void OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;
	void OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;

private:
	HRESULT Bind_ShaderResources();
	HRESULT Ready_Components();
	HRESULT Ready_AnimationNotify();
	HRESULT Ready_Ports(Float radius, Float muzzleOffset);

private:
	void Set_IsFiring(Bool isFiring) { m_IsFiring = isFiring; }
	void Fire_Projectile(Float timeDelta);

private:
	Weak<Em3000>			m_Em3000{};
	int32					m_RootBoneIndex{ -1 };
	Shared<Shader>			m_Shader{ nullptr };
	Shared<Model>			m_Model{ nullptr };
	Shared<SphereCollider>	m_HitBox{ nullptr };

	vector<EM3000_PROJECTILE_PORT> m_Ports;
	Float m_CannonFireAccTime{ 0.f };
	Float m_CannonFireRate{ 0.1f }; 
	Bool  m_IsFiring{ false };


public:
	static Shared<Em3000Body> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;
	
};

NS_END