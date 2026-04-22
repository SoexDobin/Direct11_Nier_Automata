#pragma once
#include "Monster.h"

NS_BEGIN(Engine)
class SphereCollider;
class Navigation;
NS_END

NS_BEGIN(Client)

class Em3000Body;
class Em3000Movement;

class CLIENT_DLL Em3000 final : public Monster
{
	RTTR_ENABLE(Monster)
public:
	typedef struct tagEm3000Desc : public MONSTER_CONTAINER_DESC
	{
		
	} EM3000_DESC;
public:
	explicit Em3000() = default;
	explicit Em3000(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Em3000(const Em3000& rhs);
	~Em3000() override = default;
	
public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;
	void On_Destroy() override;

public:
	void Priority_Update(Float timeDelta) override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void Fixed_Update(Float fixedDelta) override;
	HRESULT Render() override;
	void Submit_RenderGroup() override;

public:
	const TRANSFORM_FRAME& Get_BodyModelTransform() const;
	void Set_Navigation(const Shared<Navigation>& navigation) override;
	void TakeDamage(const DAMAGE_INFO& dmgInfo) override;
	void OnDeath() override;

public:
	void OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;
	void OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;
	void OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;

private:
	HRESULT Ready_PartObjects();
	HRESULT Ready_Components();

private:
	Shared<Em3000Body> m_MainBody{ nullptr };
	Shared<Em3000Movement> m_Em3000Movement{ nullptr };
	Shared<SphereCollider> m_PhysicalZone{ nullptr };
	Shared<Navigation> m_Navigation{ nullptr };

public:
	static Shared<Em3000> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;

};

NS_END