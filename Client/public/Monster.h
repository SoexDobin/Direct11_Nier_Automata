#pragma once
#include "Entity.h"

NS_BEGIN(Client)

class MonsterStateMachine;
class HpBarWorldUI;

class CLIENT_DLL Monster abstract : public Entity
{
public:
	typedef struct tagMonsterObjectDesc : public ENTITY_DESC {} MONSTER_DESC;

public:
	explicit Monster();
	explicit Monster(const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context);
	explicit Monster(const Monster& rhs);
	virtual ~Monster() override = default;
	
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
	void Fixed_Update(Float fixedDelta) override;
	HRESULT Render() override;
	void Submit_RenderGroup() override;

protected:
	void OnCollisionEnter(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;
	void OnCollisionStay(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;
	void OnCollisionExit(const Shared<Collider>& ownCollider, const Shared<Collider>& targetCollider) override;

public:
	void TakeDamage(const DAMAGE_INFO& dmgInfo) override;

public:
	Bool Is_TargetFront() const;
	Bool Has_Target() const { return !m_TargetPlayer.expired(); }
	void Set_Target(const Shared<GameObject>& target) { m_TargetPlayer = target; }
	Vector3 Get_DirectionToTarget() const;
	Float Get_DistanceToTarget() const;

protected:
	void Play_HitSFX(const DAMAGE_INFO& dmgInfo) const;
	void DisplaySparkEffect(ATK_TYPE atkType, Vector3 position, Quaternion rotation = { 0.f, 0.f, 0.f, 1.f }) const;

protected:
	Shared<HpBarWorldUI> m_HpBarUI{ nullptr };

protected:
	Shared<MonsterStateMachine> m_States;
	Weak<GameObject> m_TargetPlayer{};

public:
	virtual Shared<GameObject> Clone(void* arg) override PURE;
};

NS_END
