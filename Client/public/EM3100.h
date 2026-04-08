#pragma once
#include "Monster.h"

NS_BEGIN(Engine)
class Model;
class Shader;
class SphereCollider;
NS_END

NS_BEGIN(Client)

class CLIENT_DLL Em3100 final : public Monster
{
	RTTR_ENABLE(Monster)
public:
	typedef struct tagEm3100Desc : public MONSTER_CONTAINER_DESC
	{
	} EM3100_CONTAINER_DESC;
public:
	explicit Em3100();
	explicit Em3100(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Em3100(const Em3100& rhs);
	~Em3100() override = default;
	
public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* arg) override;

public:
	void Priority_Update(Float timeDelta) override;
	void Update(Float timeDelta) override;
	void Late_Update(Float timeDelta) override;
	void Fixed_Update(Float fixedDelta) override;
	HRESULT Render() override;
	void Submit_RenderGroup() override;

public:
	void TakeDamage(const DAMAGE_INFO& dmgInfo) override;

public:
	void OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;
	void OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;
	void OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;

private:
	HRESULT Ready_PartObjects();
	HRESULT Ready_Components();

private:
	Shared<SphereCollider> m_InteractionZone{ nullptr };
	Shared<MonsterStateMachine> m_States{ nullptr };

public:
	static Shared<Em3100> Create(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	Shared<GameObject> Clone(void* arg) override;
};

NS_END